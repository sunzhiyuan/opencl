//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2009,2014 QUALCOMM Incorporated.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <FrmPlatform.h>
#include <FrmComputeApplication.h>
#include <FrmUtils.h>
#include <OpenCL/FrmKernel.h>
#include "Compute.h"

// QC_OPTS is defined in android.mk. Without it, it will show the non-optimized version.

// Set the number of iterations of target kernel execution to be averaged
#define TARGET_ITERATIONS 20
//--------------------------------------------------------------------------------------
// Name: LoadTGA()
// Desc: Helper function to load a 32-bit TGA image file
//--------------------------------------------------------------------------------------
BYTE* LoadTGA( const CHAR* strFileName, INT32* pWidth, INT32* pHeight )
{
    struct TARGA_HEADER
    {
        BYTE   IDLength, ColormapType, ImageType;
        BYTE   ColormapSpecification[5];
        UINT16 XOrigin, YOrigin;
        UINT16 ImageWidth, ImageHeight;
        BYTE   PixelDepth;
        BYTE   ImageDescriptor;
    };

    TARGA_HEADER header;

    // Read the TGA file
    CHAR logMsg[1024];
    BYTE* pBuffer;
    if( FALSE == FrmLoadFile( strFileName, (VOID**)&pBuffer) )
    {
        FrmSprintf( logMsg, sizeof(logMsg), "ERROR: Could not image file '%s'\n", strFileName );
        FrmLogMessage( logMsg );
        return NULL;
    }

    FrmMemcpy( &header, pBuffer, sizeof(header) );

    UINT32 nPixelSize = header.PixelDepth / 8;
    (*pWidth)  = header.ImageWidth;
    (*pHeight) = header.ImageHeight;

    BYTE* pBits = (pBuffer + sizeof(header));
    BYTE* pBGRABits = new BYTE[ 4 * header.ImageWidth * header.ImageHeight ];

    // Make sure it is 4 component BGRA
    BYTE* p = pBits;
    BYTE* p2 = pBGRABits;
    for( UINT32 y=0; y<header.ImageHeight; y++ )
    {
        for( UINT32 x=0; x<header.ImageWidth; x++ )
        {
            p2[0] = p[0];
            p2[1] = p[1];
            p2[2] = p[2];
            p2[3] = 255;

            p += nPixelSize;
            p2 += 4;
        }
    }

    delete [] pBuffer;
    return pBGRABits;
}

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmComputeApplication* FrmCreateComputeApplicationInstance()
{
    return new CSample( "ImageBoxFilter" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) :
    CFrmComputeApplication( strName ),
    m_commandQueue(0),
    m_program(0),
    m_sourceImage(0),
    m_destImage(0),
    m_bufImage(0),
    m_sampler(0),
    m_nImgWidth(0),
    m_nImgHeight(0),
    m_pRefResults(0),
    m_kernel(0)
{

}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    cl_int errNum;

    if(!FrmOpenConsole())
        return FALSE;

    // Make sure the device supports images
    cl_bool imageSupport = CL_FALSE;
    clGetDeviceInfo( m_devices[0], CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), &imageSupport, NULL );
    if ( imageSupport != CL_TRUE )
    {
        FrmLogMessage( "ERROR: OpenCL device does not support images, this sample cannot be run.\n" );
        return FALSE;
    }

    // Create the command queue
    m_commandQueue = clCreateCommandQueue( m_context, m_devices[0], CL_QUEUE_PROFILING_ENABLE, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to create command queue" );
        return FALSE;
    }

    #ifdef QC_OPTS
    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/ImageBoxFilter.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math -DQC_OPTS" ) )
    {
        return FALSE;
    }
    #else
    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/ImageBoxFilter.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }
    #endif


    // Create kernel
    m_kernel = clCreateKernel( m_program, "ImageBoxFilter", &errNum );
    if ( errNum != CL_SUCCESS )
    {
         CHAR str[256];
         FrmSprintf( str, sizeof(str), "ERROR: Failed to create kernel 'ImageBoxFilter'.\n" );
         FrmLogMessage( str );
         return FALSE;
    }

    // Load image from file
    BYTE* pBGRAImage = LoadTGA( "Samples/Textures/SampleImageUncompressed.tga", &m_nImgWidth, &m_nImgHeight );
    if ( pBGRAImage == NULL )
    {
        return FALSE;
    }

    // Create OpenCL source image on device
    cl_image_format imageFormat;
    imageFormat.image_channel_data_type = CL_UNORM_INT8;
    imageFormat.image_channel_order = CL_BGRA;

    m_sourceImage = clCreateImage2D( m_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &imageFormat, m_nImgWidth,
        m_nImgHeight, 0, pBGRAImage, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: creating source image on device.\n" );
        return FALSE;
    }

    // Compute reference results
    if ( RunTests() )
    {
        ComputeRefResults( pBGRAImage );
    }
    delete [] pBGRAImage;

    // Create OpenCL destination image on device
    m_destImage = clCreateImage2D( m_context, CL_MEM_WRITE_ONLY, &imageFormat, m_nImgWidth,
        m_nImgHeight, 0, NULL, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: creating destination image on device.\n" );
        return FALSE;
    }

    #ifdef QC_OPTS
    // Create kernel
    m_kernel2 = clCreateKernel( m_program, "ImageBoxFilter16NSampling", &errNum );
    if ( errNum != CL_SUCCESS )
    {
         CHAR str[256];
         FrmSprintf( str, sizeof(str), "ERROR: Failed to create kernel 'ImageBoxFilter16NSampling'.\n" );
         FrmLogMessage( str );
         return FALSE;
    }
    // Create OpenCL destination image on device
    m_bufImage = clCreateImage2D( m_context, CL_MEM_READ_WRITE, &imageFormat, m_nImgWidth,
        m_nImgHeight, 0, NULL, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: creating destination image on device.\n" );
        return FALSE;
    }
    #endif //QC_OPTS

    // Create sampler
    m_sampler = clCreateSampler( m_context, CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: creating sampler object.\n" );
        return FALSE;
    }
    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_commandQueue != 0 )
    {
        clReleaseCommandQueue( m_commandQueue );
        m_commandQueue = 0;
    }

    if( m_program != 0 )
    {
        clReleaseProgram( m_program );
        m_program = 0;
    }

    if( m_kernel != 0 )
    {
        clReleaseKernel( m_kernel );
        m_kernel = 0;
    }

    if( m_sourceImage != 0 )
    {
        clReleaseMemObject( m_sourceImage );
        m_sourceImage = 0;
    }

    if( m_destImage != 0 )
    {
        clReleaseMemObject( m_destImage );
        m_destImage = 0;
    }

    if( m_bufImage != 0 )
    {
        clReleaseMemObject( m_bufImage );
        m_bufImage = 0;
    }


    if( m_sampler != 0 )
    {
        clReleaseSampler( m_sampler );
        m_sampler = 0;
    }

    delete [] m_pRefResults;
}

//--------------------------------------------------------------------------------------
// Name: ComputeRefResults()
// Desc: For RunTests() mode, compute 8x8 box filter on CPU
//--------------------------------------------------------------------------------------
VOID CSample::ComputeRefResults( BYTE* pBGRAImage )
{
    // Compute 8x8 box filter on the CPU
    m_pRefResults = new BYTE[ m_nImgWidth * m_nImgHeight * 4 ];

    for ( int y = 0; y < m_nImgHeight; y++ )
    {
        for ( int x = 0; x < m_nImgWidth; x++ )
        {
            FRMVECTOR4 sum = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f );

            for( int i = 0; i < 8; i++ )
            {
                for( int j = 0; j < 8; j++ )
                {
                    int xOffset = i - 4;
                    int yOffset = j - 4;

                    int xCoord = x + xOffset;
                    int yCoord = y + yOffset;

                    // Clamp the coordinates
                    xCoord = FrmMax( FrmMin( xCoord, m_nImgWidth - 1 ), 0 );
                    yCoord = FrmMax( FrmMin( yCoord, m_nImgHeight - 1 ), 0 );

                    FRMVECTOR4 val;

                    val.r = (float)pBGRAImage[ (yCoord * m_nImgWidth + xCoord ) * 4 + 2];
                    val.g = (float)pBGRAImage[ (yCoord * m_nImgWidth + xCoord ) * 4 + 1];
                    val.b = (float)pBGRAImage[ (yCoord * m_nImgWidth + xCoord ) * 4 + 0];
                    val.a = (float)pBGRAImage[ (yCoord * m_nImgWidth + xCoord ) * 4 + 3];

                    sum += val;
                }
            }

            // Compute the average
            sum /= 64.0f;

            m_pRefResults[ (y * m_nImgWidth + x) * 4 + 2] = (BYTE) (sum.r);
            m_pRefResults[ (y * m_nImgWidth + x) * 4 + 1] = (BYTE) (sum.g);
            m_pRefResults[ (y * m_nImgWidth + x) * 4 + 0] = (BYTE) (sum.b);
            m_pRefResults[ (y * m_nImgWidth + x) * 4 + 3] = (BYTE) (sum.a);
        }
    }

}

//--------------------------------------------------------------------------------------
// Name: Compute()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Compute()
{
    char str[256];
    // set the default iterations to 1 for CPU kernel execution
    cl_int iterations = 1, errNum = 0;
    double elapsed_time = 0;

    // Determine the max workgroup size, this wil be used to determine the local work size for the kernel
    size_t maxWorkGroupSize = 0;
    errNum = clGetKernelWorkGroupInfo( m_kernel, m_devices[0], CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL );
    if (errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: clGetKernelInfo on CL_KERNEL_WORK_GROUP_SIZE\n");
        return FALSE;
    }
    FrmSprintf( str, sizeof(str), "Max workgroup size: %d\n", maxWorkGroupSize );
    FrmLogMessage( str );

    size_t localWorkSize[2] = { (size_t) FrmSqrt( (FLOAT32) maxWorkGroupSize), (size_t) FrmSqrt( (FLOAT32)  maxWorkGroupSize) };

    FrmSprintf( str, sizeof(str), "Local work size: (%d x %d)\n", localWorkSize[0], localWorkSize[1] );
    FrmLogMessage( str );

    size_t globalWorkSize[2];

    // Compute the next global size that is a multiple of the local size
    size_t remndr = m_nImgWidth % localWorkSize[0];
    if( remndr == 0 )
        globalWorkSize[0] = m_nImgWidth;
    else
        globalWorkSize[0] = m_nImgWidth + localWorkSize[0] - remndr;

    remndr = m_nImgHeight % localWorkSize[1];
    if( remndr == 0 )
        globalWorkSize[1] = m_nImgHeight;
    else
        globalWorkSize[1] = m_nImgHeight + localWorkSize[1] - remndr;

    m_Timer.Reset();
    m_Timer.Start();

    // Set the kernel arguments
    #ifdef QC_OPTS
    errNum = clSetKernelArg( m_kernel, 0, sizeof(cl_mem), &m_sourceImage );
    errNum |= clSetKernelArg( m_kernel, 1, sizeof(cl_mem), &m_bufImage );
    errNum |= clSetKernelArg( m_kernel, 2, sizeof(cl_sampler), &m_sampler );
    errNum |= clSetKernelArg( m_kernel, 3, sizeof(cl_int), &m_nImgWidth );
    errNum |= clSetKernelArg( m_kernel, 4, sizeof(cl_int), &m_nImgHeight );
    errNum |= clSetKernelArg( m_kernel2, 0, sizeof(cl_mem), &m_bufImage );
    errNum |= clSetKernelArg( m_kernel2, 1, sizeof(cl_mem), &m_destImage );
    errNum |= clSetKernelArg( m_kernel2, 2, sizeof(cl_sampler), &m_sampler );
    errNum |= clSetKernelArg( m_kernel2, 3, sizeof(cl_int), &m_nImgWidth );
    errNum |= clSetKernelArg( m_kernel2, 4, sizeof(cl_int), &m_nImgHeight );

    #else
    errNum = clSetKernelArg( m_kernel, 0, sizeof(cl_mem), &m_sourceImage );
    errNum |= clSetKernelArg( m_kernel, 1, sizeof(cl_mem), &m_destImage );
    errNum |= clSetKernelArg( m_kernel, 2, sizeof(cl_sampler), &m_sampler );
    errNum |= clSetKernelArg( m_kernel, 3, sizeof(cl_int), &m_nImgWidth );
    errNum |= clSetKernelArg( m_kernel, 4, sizeof(cl_int), &m_nImgHeight );
    #endif //QC_OPTS

    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments.\n" );
        return FALSE;
    }

    // if the device type is GPU, then prime the GPU first and average the multiple iterations of kernel execution.
    cl_device_type deviceType;
    errNum = clGetDeviceInfo(m_devices[0], CL_DEVICE_TYPE, sizeof(deviceType), &deviceType,NULL);

    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error getting the device info.\n" );
        return FALSE;
    }

    if (deviceType == CL_DEVICE_TYPE_GPU)
    {
        errNum = FrmPrimeGPU(m_commandQueue, m_kernel, 2, globalWorkSize, localWorkSize);
        if( errNum != TRUE )
        {
            FrmSprintf( str, sizeof(str), "Error priming the GPU.\n" );
            FrmLogMessage( str );
            return FALSE;
        }
        iterations = TARGET_ITERATIONS;
    }

    m_Timer.Reset();
    m_Timer.Start();

    for (int i = 0; i < iterations; i++)
    {
        cl_event kernel_event;
        // Queue the kernel for execution
        errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernel, 2, NULL,
                        globalWorkSize, localWorkSize, 0, NULL, &kernel_event );
        if( errNum != CL_SUCCESS )
        {
            FrmSprintf( str, sizeof(str), "Error queueing kernel for execution (%d).\n", errNum );
            FrmLogMessage( str );
            return FALSE;
        }

        #ifdef QC_OPTS
        // Queue the kernel for execution
        errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernel2, 2, NULL,
                        globalWorkSize, localWorkSize, 1, &kernel_event, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmSprintf( str, sizeof(str), "Error queueing kernel for execution (%d).\n", errNum );
            FrmLogMessage( str );
            return FALSE;
        }
        #endif // QC_OPTS

        clFinish( m_commandQueue );
    }

    m_Timer.Stop();

    // average the elapsed time over all the iterations
    elapsed_time = m_Timer.GetTime() / iterations;
    // Convert seconds into milliseconds
    elapsed_time *= 1000;

    // Read the image back to host memory and write to disk
    BYTE* pRGBABits = new BYTE[ m_nImgWidth * m_nImgHeight * 4 ];
    size_t origin[3] = { 0, 0, 0 };
    size_t region[3] = { m_nImgWidth, m_nImgHeight, 1 } ;
    errNum = clEnqueueReadImage( m_commandQueue, m_destImage, CL_TRUE, origin, region, 0, 0, pRGBABits,
        0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: reading filtered image back to host.\n" );
        return FALSE;
    }

    if( FALSE == FrmSaveImageAsTGA( "ImageFilter8x8.tga", m_nImgWidth, m_nImgHeight, (UINT32*) pRGBABits ) )
    {
        FrmLogMessage( "ERROR: writing output file 'ImageFilter8x8.tga'.\n" );
        return FALSE;
    }
    FrmLogMessage( "Wrote output file 'ImageFilter8x8.tga'.\n" );

    FrmSprintf( str, sizeof(str), "Results: 8x8 Box filter on image ('%d x %d') in '%.3f' milliseconds.\n",
            m_nImgWidth, m_nImgHeight, elapsed_time );
    FrmLogMessage( str );

    if ( RunTests() )
    {
        BOOL result = TRUE;
        const FLOAT32 epsilon = 0.000001f;

        for ( int y = 0; y < m_nImgHeight; y++ )
        {
            for ( int x = 0; x < m_nImgWidth; x++ )
            {
                // Check each color component
                for ( int c = 0; c < 4; c++ )
                {
                    BYTE val = pRGBABits[ (y * m_nImgWidth + x) * 4 + c ];
                    BYTE refVal = m_pRefResults[ (y * m_nImgWidth + x) * 4 + c ];

                    int diff = val - refVal;
                    diff = diff < 0 ? -diff : diff;
                    // 2 passes kernel operation creates more error margin due to more float op
                    // So we set the error margin to to 3
                    if ( diff > 3 )
                    {
                        FrmSprintf( str, sizeof(str), "Reference test failure, ref = (%d), result = (%d)\n", refVal, val);
                        FrmLogMessage( str );
                        result = FALSE;
                    }
                }
            }
        }

        if ( result == FALSE )
        {
            delete[] pRGBABits;
            return FALSE;
        }
    }

    delete [] pRGBABits;

    return TRUE;
}

