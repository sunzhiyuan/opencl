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
    return new CSample( "ImageSobelFilter" );
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
    m_imageGray = 0;
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

    // use optimized version for GPU and un-optimized version for CPU.
    const char* options;
    if (devType == CL_DEVICE_TYPE_CPU)
        options = "-cl-fast-relaxed-math";
    else if (devType == CL_DEVICE_TYPE_GPU)
        options = "-cl-fast-relaxed-math -DQC_OPTS";

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/ImageSobelFilter.cl", &m_program, m_context,
                                                  &m_devices[0], 1, options ) )
    {
        return FALSE;
    }
    // Create kernels
    const CHAR* kernelNames[NUM_KERNELS] =
    {
        "ImageConvertRGBToGray",
        "ImageSobelFilter"
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

    // Create grayscle filtered image
    m_imageGray = clCreateImage2D( m_context, CL_MEM_READ_WRITE, &imageFormat, m_nImgWidth,
        m_nImgHeight, 0, NULL, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: creating grayscale image on device.\n" );
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
    if (devType == CL_DEVICE_TYPE_CPU)
    {
        // Create sampler
        m_sampler = clCreateSampler( m_context, CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST, &errNum );
        if ( errNum != CL_SUCCESS )
        {
             FrmLogMessage( "ERROR: creating sampler object.\n" );
             return FALSE;
        }
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

    if( m_imageGray != 0 )
    {
        clReleaseMemObject( m_imageGray );
        m_imageGray = 0;
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
// Name: ComputeRefResults()
// Desc: For RunTests() mode, compute sobel filter on CPU
//--------------------------------------------------------------------------------------
VOID CSample::ComputeRefResults( BYTE* pBGRAImage )
{
    // Compute sobel filter on the CPU
    m_pRefResults = new BYTE[ m_nImgWidth * m_nImgHeight ];


    // Convolution kernel weights for the gradient in the horizontal direction
    const float weightsX[9] = { -1.0f, 0.0f, +1.0f,
                                -2.0f, 0.0f, +2.0f,
                                -1.0f, 0.0f, +1.0f };

    // Convolution kernel weights for the gradient in the vertical direction
    const float weightsY[9] = { +1.0f, +2.0f, +1.0f,
                                 0.0f,  0.0f,  0.0f,
                                -1.0f, -2.0f, -1.0f };

    for ( int y = 0; y < m_nImgHeight; y++ )
    {
        for ( int x = 0; x < m_nImgWidth; x++ )
        {
            float sumX = 0.0f;
            float sumY = 0.0f;

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

                    // Convert to grayscale
                    FRMVECTOR4 val;

                    val.r = (float)pBGRAImage[ (yCoord * m_nImgWidth + xCoord ) * 4 + 2];
                    val.g = (float)pBGRAImage[ (yCoord * m_nImgWidth + xCoord ) * 4 + 1];
                    val.b = (float)pBGRAImage[ (yCoord * m_nImgWidth + xCoord ) * 4 + 0];

                    float grayScale = ( 0.299f * val.r + 0.587f * val.g + 0.114f * val.b + 0.5f );
                    if ( grayScale < 0.0f )
                        grayScale = 0.0f;
                    if ( grayScale > 255.0f )
                        grayScale = 255.0f;
                    grayScale = (BYTE)grayScale;


                    sumX += weightsX[i * 3 + j] * grayScale;
                    sumY += weightsY[i * 3 + j] * grayScale;
                }
            }

            float gradient = FrmSqrt( sumX * sumX + sumY * sumY );
            if ( gradient < 0.0f )
                gradient = 0.0f;
            if ( gradient > 255.0f )
                gradient = 255.0f;

            m_pRefResults[ (y * m_nImgWidth + x)] = (BYTE) gradient;
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

    if (devType == CL_DEVICE_TYPE_CPU)
    {
        localWorkSize[0] = (size_t) FrmSqrt( (FLOAT32) maxWorkGroupSize );
        localWorkSize[1] = (size_t) FrmSqrt( (FLOAT32) maxWorkGroupSize );
    }
    else if (devType == CL_DEVICE_TYPE_GPU)
    {
        // try to match 128x2
        localWorkSize[0] = (size_t)128;
        localWorkSize[1] = (size_t) 2;
        if (localWorkSize[0]*localWorkSize[1] > maxWorkGroupSize)
        {
            localWorkSize[0] = maxWorkGroupSize / localWorkSize[1];
        }
    }

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
    cl_int errNum = 0;

    m_Timer.Reset();
    m_Timer.Start();

    if (devType == CL_DEVICE_TYPE_CPU)
    {
        // Set the kernel arguments for RGB -> grayscale conversion
        errNum = clSetKernelArg( m_kernels[RGB_TO_GRAYSCALE], 0, sizeof(cl_mem), &m_sourceImageRGBA );
        errNum |= clSetKernelArg( m_kernels[RGB_TO_GRAYSCALE], 1, sizeof(cl_mem), &m_imageGray );
        errNum |= clSetKernelArg( m_kernels[RGB_TO_GRAYSCALE], 2, sizeof(cl_sampler), &m_sampler );
        errNum |= clSetKernelArg( m_kernels[RGB_TO_GRAYSCALE], 3, sizeof(cl_int), &m_nImgWidth );
        errNum |= clSetKernelArg( m_kernels[RGB_TO_GRAYSCALE], 4, sizeof(cl_int), &m_nImgHeight );
    }
    else if (devType == CL_DEVICE_TYPE_GPU)
    {
        // Set the kernel arguments for RGB -> grayscale conversion
        errNum = clSetKernelArg( m_kernels[RGB_TO_GRAYSCALE], 0, sizeof(cl_mem), &m_sourceImageRGBA );
        errNum |= clSetKernelArg( m_kernels[RGB_TO_GRAYSCALE], 1, sizeof(cl_mem), &m_imageGray );
        errNum |= clSetKernelArg( m_kernels[RGB_TO_GRAYSCALE], 2, sizeof(cl_int), &m_nImgWidth );
        errNum |= clSetKernelArg( m_kernels[RGB_TO_GRAYSCALE], 3, sizeof(cl_int), &m_nImgHeight );
    }

    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments.\n" );
        return FALSE;
    }

    // Compute global/local work sizes for the kernel
    size_t localWorkSize[2];
    size_t globalWorkSize[2];
    if ( ComputeWorkSizes( m_kernels[RGB_TO_GRAYSCALE], localWorkSize, globalWorkSize ) == FALSE )
        return FALSE;

    // Queue the RGB -> grayscale conversion kernel for execution
    errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernels[RGB_TO_GRAYSCALE], 2, NULL,
                        globalWorkSize, localWorkSize, 0, NULL, NULL );

    if( errNum != CL_SUCCESS )
    {
        FrmSprintf( str, sizeof(str), "Error queueing RGB -> gray kernel for execution (%d).\n", errNum );
        FrmLogMessage( str );
        return FALSE;
    }

    if (devType == CL_DEVICE_TYPE_CPU)
    {
        // Set the kernel arguments for sobel filter
        errNum = clSetKernelArg( m_kernels[SOBEL_FILTER], 0, sizeof(cl_mem), &m_imageGray );
        errNum |= clSetKernelArg( m_kernels[SOBEL_FILTER], 1, sizeof(cl_mem), &m_destImage );
        errNum |= clSetKernelArg( m_kernels[SOBEL_FILTER], 2, sizeof(cl_sampler), &m_sampler );
        errNum |= clSetKernelArg( m_kernels[SOBEL_FILTER], 3, sizeof(cl_int), &m_nImgWidth );
        errNum |= clSetKernelArg( m_kernels[SOBEL_FILTER], 4, sizeof(cl_int), &m_nImgHeight );
    }
    else if (devType == CL_DEVICE_TYPE_GPU)
    {
        // Set the kernel arguments for sobel filter
        errNum = clSetKernelArg( m_kernels[SOBEL_FILTER], 0, sizeof(cl_mem), &m_imageGray );
        errNum |= clSetKernelArg( m_kernels[SOBEL_FILTER], 1, sizeof(cl_mem), &m_destImage );
        errNum |= clSetKernelArg( m_kernels[SOBEL_FILTER], 2, sizeof(cl_int), &m_nImgWidth );
        errNum |= clSetKernelArg( m_kernels[SOBEL_FILTER], 3, sizeof(cl_int), &m_nImgHeight );
    }

    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments.\n" );
        return FALSE;
    }

    if ( ComputeWorkSizes( m_kernels[SOBEL_FILTER], localWorkSize, globalWorkSize ) == FALSE )
        return FALSE;


    // Queue the sobel filter kernel for execution
    errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernels[SOBEL_FILTER], 2, NULL,
                        globalWorkSize, localWorkSize, 0, NULL, NULL );

    if( errNum != CL_SUCCESS )
    {
        FrmSprintf( str, sizeof(str), "Error queueing sobel kernel for execution (%d).\n", errNum );
        FrmLogMessage( str );
        return FALSE;
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

    if( FALSE == FrmSaveImageAsTGA( "ImageSobelFilter.tga", m_nImgWidth, m_nImgHeight, (UINT32*) pRGBABits ) )
    {
        FrmLogMessage( "ERROR: writing output file 'ImageSobelFilter.tga'.\n" );
        return FALSE;
    }

    FrmLogMessage( "Wrote output file 'ImageSobelFilter.tga'.\n" );
    FrmSprintf( str, sizeof(str), "Results: Sobel filter on image ('%d x %d') in '%.6f' seconds.\n",
            m_nImgWidth, m_nImgHeight, m_Timer.GetTime() );
    FrmLogMessage( str );

    if ( RunTests() )
    {
        BOOL result = TRUE;
        const FLOAT32 epsilon = 0.000001f;

        for ( int y = 0; y < m_nImgHeight; y++ )
        {
            for ( int x = 0; x < m_nImgWidth; x++ )
            {
                BYTE val = pRGBABits[ (y * m_nImgWidth + x) * 4];
                BYTE refVal = m_pRefResults[ (y * m_nImgWidth + x) ];

                int diff = val - refVal;
                diff = diff < 0 ? -diff : diff;
                if ( diff > 5 )
                {
                    FrmSprintf( str, sizeof(str), "Reference test failure, ref = (%d), result = (%d)\n", refVal, val);
                    FrmLogMessage( str );
                    result = FALSE;
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

