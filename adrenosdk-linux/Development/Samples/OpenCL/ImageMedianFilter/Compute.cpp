//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <FrmPlatform.h>
#include <FrmComputeApplication.h>
#include <FrmUtils.h>
#include <OpenCL/FrmKernel.h>
#include "Compute.h"

#define findDevType(devType, device) clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(devType), &devType,NULL);
#define TARGET_ITERATIONS 100

cl_device_type devType;
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
    return new CSample( "ImageMedianFilter" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmComputeApplication( strName )
{
    m_commandQueue = 0;
    m_program = 0;
    m_sourceImageRGBA = 0;
    m_destImage = 0;
    m_sampler = 0;
    m_nImgWidth = 0;
    m_nImgHeight = 0;
    m_pRefResults = NULL;

    for( INT32 i = 0; i < NUM_KERNELS; i++ )
    {
        m_kernels[i] = 0;
    }
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

    errNum = findDevType(devType, m_devices[0]);
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to obtain device type" );
        return FALSE;
    }

    const char* options;
    if (devType == CL_DEVICE_TYPE_CPU)
        options = "-cl-fast-relaxed-math";
    else if (devType == CL_DEVICE_TYPE_GPU)
        options = "-cl-fast-relaxed-math -qcom-accelerate-16-bit  -DQC_OPTS";

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/ImageMedianFilter.cl", &m_program, m_context,
                                                                    &m_devices[0], 1, options ) )
    {
        return FALSE;
    }

    // Create kernels
    const CHAR* kernelNames[NUM_KERNELS] =
    {
        "ImageMedianFilter"
    };
    for( INT32 i = 0; i < NUM_KERNELS; i++ )
    {
        m_kernels[i] = clCreateKernel( m_program, kernelNames[i], &errNum );
        if ( errNum != CL_SUCCESS )
        {
            CHAR str[256];
            FrmSprintf( str, sizeof(str), "ERROR: Failed to create kernel '%s'.\n", kernelNames[i] );
            FrmLogMessage( str );
            return FALSE;
        }
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
    m_sourceImageRGBA = clCreateImage2D( m_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &imageFormat, m_nImgWidth,
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

    // Create OpenCL destination image on device (grayscale)
    m_destImage = clCreateImage2D( m_context, CL_MEM_WRITE_ONLY, &imageFormat, m_nImgWidth,
        m_nImgHeight, 0, NULL, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: creating destination image on device.\n" );
        return FALSE;
    }

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

    for( INT32 i = 0; i < NUM_KERNELS; i++ )
    {
        if( m_kernels[i] != 0 )
        {
            clReleaseKernel( m_kernels[i] );
            m_kernels[i] = 0;
        }
    }

    if( m_sourceImageRGBA != 0 )
    {
        clReleaseMemObject( m_sourceImageRGBA );
        m_sourceImageRGBA = 0;
    }

    if( m_destImage != 0 )
    {
        clReleaseMemObject( m_destImage );
        m_destImage = 0;
    }

    if( m_sampler != 0 )
    {
        clReleaseSampler( m_sampler );
        m_sampler = 0;
    }

    delete [] m_pRefResults;
}

//--------------------------------------------------------------------------------------
// Name: Compare()
// Desc: Callback compare() function for qsort
//--------------------------------------------------------------------------------------
int Compare( const void *a, const void *b )
{
    BYTE valA = *(BYTE*)a;
    BYTE valB = *(BYTE*)b;

    return( valA - valB );
}

//--------------------------------------------------------------------------------------
// Name: ComputeRefResults()
// Desc: For RunTests() mode, compute median filter on CPU
//--------------------------------------------------------------------------------------
VOID CSample::ComputeRefResults( BYTE* pBGRAImage )
{
    // Compute sobel filter on the CPU
    m_pRefResults = new BYTE[ m_nImgWidth * m_nImgHeight * 4];


    for ( int y = 0; y < m_nImgHeight; y++ )
    {
        for ( int x = 0; x < m_nImgWidth; x++ )
        {
            BYTE values[9 * 3]; // 9 values (3x3), 3 channel (RGB)
            int index = 0;

            // Load all of the pixel neighbors including itself into values[]
            for( int i = 0; i <= 2; i++ )
            {
                for( int j = 0; j <= 2; j++ )
                {
                    int xOffset = j - 1;
                    int yOffset = i - 1;

                    int xCoord = x + xOffset;
                    int yCoord = y + yOffset;

                    // Clamp the coordinates
                    xCoord = FrmMax( FrmMin( xCoord, m_nImgWidth - 1 ), 0 );
                    yCoord = FrmMax( FrmMin( yCoord, m_nImgHeight - 1 ), 0 );

                    values[index] = pBGRAImage[ (yCoord * m_nImgWidth + xCoord ) * 4 + 2];
                    values[index + 9] = pBGRAImage[ (yCoord * m_nImgWidth + xCoord ) * 4 + 1];
                    values[index + 18] = pBGRAImage[ (yCoord * m_nImgWidth + xCoord ) * 4 + 0];
                    index++;
                }
            }

            // Sort the values independently for each color channel
            for( int c = 0; c < 3; c++ )
            {
                qsort( &values[ c * 9 ], 9, sizeof(BYTE), Compare );
            }

            // Take the median value for each channel (the center of the 9 values)
            m_pRefResults[ (y * m_nImgWidth + x) * 4 + 2] = (values[4]);
            m_pRefResults[ (y * m_nImgWidth + x) * 4 + 1] = (values[4 + 9]);
            m_pRefResults[ (y * m_nImgWidth + x) * 4 + 0] = (values[4 + 18]);
        }
    }

}

//--------------------------------------------------------------------------------------
// Name: ComputeWorkSizes()
// Desc: Compute global and local work sizes based on the size of the image and
// the maximum workgroup size for the kernel
//--------------------------------------------------------------------------------------
BOOL CSample::ComputeWorkSizes( cl_kernel kernel, size_t localWorkSize[2], size_t globalWorkSize[2] )
{
    cl_int errNum;
    char str[256];

    // Determine the localWorkSize based on the maximum allowed by the device for this kernel
    size_t maxWorkGroupSize = 0;
    errNum = clGetKernelWorkGroupInfo( kernel, m_devices[0], CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL );
    if (errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: clGetKernelInfo on CL_KERNEL_WORK_GROUP_SIZE\n");
        return FALSE;
    }
    FrmSprintf( str, sizeof(str), "Max workgroup size: %d\n", maxWorkGroupSize );
    FrmLogMessage( str );

    localWorkSize[0] = (size_t) FrmSqrt( (FLOAT32) maxWorkGroupSize );
    localWorkSize[1] = (size_t) FrmSqrt( (FLOAT32) maxWorkGroupSize );

    FrmSprintf( str, sizeof(str), "Local work size: (%d x %d)\n", localWorkSize[0], localWorkSize[1] );
    FrmLogMessage( str );


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

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: Compute()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Compute()
{
    char str[256];
    cl_int iterations = 1, errNum = 0;

    // Set the kernel arguments for median filter
    errNum = clSetKernelArg( m_kernels[MEDIAN_FILTER], 0, sizeof(cl_mem), &m_sourceImageRGBA);
    errNum |= clSetKernelArg( m_kernels[MEDIAN_FILTER], 1, sizeof(cl_mem), &m_destImage );
    if (devType == CL_DEVICE_TYPE_CPU)
    {
        errNum |= clSetKernelArg( m_kernels[MEDIAN_FILTER], 2, sizeof(cl_sampler), &m_sampler );
        errNum |= clSetKernelArg( m_kernels[MEDIAN_FILTER], 3, sizeof(cl_int), &m_nImgWidth );
        errNum |= clSetKernelArg( m_kernels[MEDIAN_FILTER], 4, sizeof(cl_int), &m_nImgHeight );
    }
    else if (devType == CL_DEVICE_TYPE_GPU)
    {
        errNum |= clSetKernelArg( m_kernels[MEDIAN_FILTER], 2, sizeof(cl_int), &m_nImgWidth );
        errNum |= clSetKernelArg( m_kernels[MEDIAN_FILTER], 3, sizeof(cl_int), &m_nImgHeight );
    }
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments.\n" );
        return FALSE;
    }

    // Compute global/local work sizes for the kernel
    size_t localWorkSize[2];
    size_t globalWorkSize[2];
    if ( ComputeWorkSizes( m_kernels[MEDIAN_FILTER], localWorkSize, globalWorkSize ) == FALSE )
        return FALSE;

    // if the device type is GPU, then prime the GPU first and average the multiple iterations of kernel execution.
    if (devType == CL_DEVICE_TYPE_GPU)
    {
        errNum = FrmPrimeGPU(m_commandQueue, m_kernels[MEDIAN_FILTER], 2, globalWorkSize, localWorkSize);
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

    // average the multiple iterations if device is GPU.
    for (int i = 0; i < iterations; i++)
    {
        // Queue the median filter kernel for execution
        errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernels[MEDIAN_FILTER], 2, NULL,
                            globalWorkSize, localWorkSize, 0, NULL, NULL );

        if( errNum != CL_SUCCESS )
        {
            FrmSprintf( str, sizeof(str), "Error queueing median filter kernel for execution (%d).\n", errNum );
            FrmLogMessage( str );
            return FALSE;
        }
    }
    clFinish( m_commandQueue );
    m_Timer.Stop();

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

    if( FALSE == FrmSaveImageAsTGA( "ImageMedianFilter.tga", m_nImgWidth, m_nImgHeight, (UINT32*) pRGBABits ) )
    {
        FrmLogMessage( "ERROR: writing output file 'ImageMedianFilter.tga'.\n" );
        return FALSE;
    }
    FrmLogMessage( "Wrote output file 'ImageMedianFilter.tga'.\n" );

    FrmSprintf( str, sizeof(str), "Performed median filter on image ('%d x %d') in '%.6f' seconds.\n",
            m_nImgWidth, m_nImgHeight, m_Timer.GetTime() / iterations );
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
                for ( int c = 0; c < 3; c++ )
                {
                    BYTE val = pRGBABits[ (y * m_nImgWidth + x) * 4 + c ];
                    BYTE refVal = m_pRefResults[ (y * m_nImgWidth + x) * 4 + c ];

                    int diff = val - refVal;
                    diff = diff < 0 ? -diff : diff;
                    if ( diff > 1 )
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

