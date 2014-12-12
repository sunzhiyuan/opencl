//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <FrmPlatform.h>
#include <FrmComputeApplication.h>
#include <FrmUtils.h>
#include <OpenCL/FrmKernel.h>
#include "Compute.h"

// Macros for referencing pixels in the rows/columns of the reference implementation
// of the kernel
#define HORZIDX(x) ((x) * 4)
#define VERTIDX(y) ((y) * m_nImgHeight * 4)

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
    return new CSample( "ImageRecursiveGaussianFilter" );
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
    m_intermediateImage = 0;
    m_destImage = 0;
    m_sampler = 0;
    m_nImgWidth = 0;
    m_nImgHeight = 0;
    m_pRefResults = NULL;

    for( INT32 i = 0; i < NUM_KERNELS; i++ )
    {
        m_kernels[i] = 0;
    }

    // Compute the parameters to the gaussian kernel based as described in
    // "Recursive Implementation of the Gaussian Filter" by Ian T. Young and Lucas J. van Vilet
    FLOAT32 sigma = 5.0f; // The spread of the kernel
    FLOAT32 q;
    if( sigma >= 0.5f && sigma <= 2.5 )
    {
        q = 3.97156f - 4.14554f * FrmSqrt( 1.0f - 0.26891f * sigma );
    }
    else if( sigma > 2.5f )
    {
        q = -0.9633f + 0.98711f * sigma;
    }
    else
    {
        q = sigma;
    }

    m_gaussParams.b0 = 1.57825f + q * (2.44413f + (1.4281f + 0.422205f * q) * q);
    m_gaussParams.b1 = q * (2.44423f + q * (2.85619f + 1.26661f * q));
    m_gaussParams.b2 = ( -1.4281f - 1.26661f * q) * q * q;
    m_gaussParams.b3 = 0.422205f * q * q * q;
    m_gaussParams.b4 = 1.0f - (m_gaussParams.b1 + m_gaussParams.b2 + m_gaussParams.b3 ) / m_gaussParams.b0 ;
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

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/ImageRecursiveGaussianFilter.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }
    // Create kernels
    const CHAR* kernelNames[NUM_KERNELS] =
    {
        "ImageGaussianFilterHorizontal",
        "ImageGaussianFilterVertical"
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

    // Create OpenCL intermediate image on device to hold result or horizontal kernel and as input to
    // vertical kernel
    m_intermediateImage = clCreateImage2D( m_context, CL_MEM_READ_WRITE, &imageFormat, m_nImgWidth,
        m_nImgHeight, 0, NULL, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: creating intermediate image on device.\n" );
        return FALSE;
    }

    // Create OpenCL destination image on device
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

    if( m_intermediateImage != 0 )
    {
        clReleaseMemObject( m_intermediateImage );
        m_intermediateImage = 0;
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
// Name: RefGaussKernelHorizontal()
// Desc: For RunTests() mode, compute reference gauss kernel on the CPU.  This
// processes the gauss kernel for the rows horizontally
//--------------------------------------------------------------------------------------
VOID CSample::RefGaussKernelHorizontal( FLOAT32* pBGRAImage )
{
    FLOAT32 b0 = m_gaussParams.b0;
    FLOAT32 b1 = m_gaussParams.b1;
    FLOAT32 b2 = m_gaussParams.b2;
    FLOAT32 b3 = m_gaussParams.b3;
    FLOAT32 b4 = m_gaussParams.b4;

    for( int y = 0; y < m_nImgHeight; y++ )
    {
        // For each color channel
        for( int c = 0; c < 4; c++ )
        {
            int index = ( y * m_nImgWidth ) * 4 + c;
            FLOAT32* in = (FLOAT32*) &pBGRAImage[ index ];
            FLOAT32 val = *in;

            in[HORZIDX(0)] = (b4 * val + ( b1 * val + b2 * val + b3 * val ) / b0);
            in[HORZIDX(1)] = (b4 * in[HORZIDX(1)] + ( b1 * in[HORZIDX(0)] + b2 * val + b3 * val ) / b0);
            in[HORZIDX(2)] = (b4 * in[HORZIDX(2)] + ( b1 * in[HORZIDX(1)] + b2 * in[HORZIDX(0)] + b3 * val ) / b0);

            for( int x = 3; x < m_nImgWidth; x++ )
            {
                in[HORZIDX(x)] = (b4 * in[HORZIDX(x)] + (b1 * in[HORZIDX(x-1)] + b2 * in[HORZIDX(x-2)] + b3 * in[HORZIDX(x-3)]) / b0);
            }

            int x = m_nImgWidth - 1;
            val = in[HORZIDX(x)];
            in[HORZIDX(x)] = (b4 * val + ( b1 * val + b2 * val + b3 * val) / b0);
            in[HORZIDX(x-1)] = (b4 * in[HORZIDX(x-1)] + ( b1 * in[HORZIDX(x)] + b2 * val + b3 * val) / b0);
            in[HORZIDX(x-2)] = (b4 * in[HORZIDX(x-2)] + ( b1 * in[HORZIDX(x-1)] + b2 * in[HORZIDX(x)] + b3 * val) / b0);

            for( int i = x - 3; i >= 0; --i )
            {
                in[HORZIDX(i)] = (b4 * in[HORZIDX(i)] + (b1 * in[HORZIDX(i+1)] + b2 * in[HORZIDX(i+2)] + b3 * in[HORZIDX(i+3)]) / b0);
            }
        }
    }
}

//--------------------------------------------------------------------------------------
// Name: RefGaussKernelVertical()
// Desc: For RunTests() mode, compute reference gauss kernel on the CPU.  This
// processes the gauss kernel for the columns vertically/
//--------------------------------------------------------------------------------------
VOID CSample::RefGaussKernelVertical( FLOAT32* pBGRAImage )
{
    FLOAT32 b0 = m_gaussParams.b0;
    FLOAT32 b1 = m_gaussParams.b1;
    FLOAT32 b2 = m_gaussParams.b2;
    FLOAT32 b3 = m_gaussParams.b3;
    FLOAT32 b4 = m_gaussParams.b4;

    for( int x = 0; x < m_nImgWidth; x++ )
    {
        // For each color channel
        for( int c = 0; c < 4; c++ )
        {
            int index = ( x ) * 4 + c;
            FLOAT32* in = (FLOAT32*) &pBGRAImage[ index ];
            FLOAT32 val = (FLOAT32) *in;

            in[0] = (b4 * val + ( b1 * val + b2 * val + b3 * val ) / b0);
            in[VERTIDX(1)] = (b4 * in[VERTIDX(1)] + ( b1 * in[0] + b2 * val + b3 * val ) / b0);
            in[VERTIDX(2)] = (b4 * in[VERTIDX(2)] + ( b1 * in[VERTIDX(1)] + b2 * in[0] + b3 * val ) / b0);

            for( int y = 3; y < m_nImgHeight; y++ )
            {
                in[VERTIDX(y)] = (b4 * in[VERTIDX(y)] + (b1 * in[VERTIDX(y-1)] + b2 * in[VERTIDX(y-2)] + b3 * in[VERTIDX(y-3)]) / b0);
            }

            int y = m_nImgHeight - 1;
            val = in[VERTIDX(y)];
            in[VERTIDX(y)] = (b4 * val + ( b1 * val + b2 * val + b3 * val) / b0);
            in[VERTIDX(y-1)] = (b4 * in[VERTIDX(y-1)] + ( b1 * in[VERTIDX(y)] + b2 * val + b3 * val) / b0);
            in[VERTIDX(y-2)] = (b4 * in[VERTIDX(y-2)] + ( b1 * in[VERTIDX(y-1)] + b2 * in[VERTIDX(y)] + b3 * val) / b0);

            for( int i = y - 3; i >= 0; --i )
            {
                in[VERTIDX(i)] = (b4 * in[VERTIDX(i)] + (b1 * in[VERTIDX(i+1)] + b2 * in[VERTIDX(i+2)] + b3 * in[VERTIDX(i+3)]) / b0);
            }
        }
    }
}

//--------------------------------------------------------------------------------------
// Name: ComputeRefResults()
// Desc: For RunTests() mode, compute recursive gaussian filter on CPU
//--------------------------------------------------------------------------------------
VOID CSample::ComputeRefResults( BYTE* pBGRAImage )
{
    // Compute recursive gaussian filter on the CPU
    m_pRefResults = new BYTE[ m_nImgWidth * m_nImgHeight * 4 ];

    // Copy the contents into a FLOAT32 buffer that will be used for
    // computing the results
    FLOAT32* pBGRAFloat = new FLOAT32[ m_nImgWidth * m_nImgHeight * 4 ];
    for( int i = 0; i < m_nImgWidth * m_nImgHeight * 4; i++ )
    {
        pBGRAFloat[i] = (FLOAT32) pBGRAImage[i];
    }

    // Run the horizontal recursive gaussian kernel
    RefGaussKernelHorizontal( pBGRAFloat );

    // Run the vertical recursive gaussian kernel
    RefGaussKernelVertical( pBGRAFloat );

    // Copy from FLOAT32 RGBA to BYTE RGBA, clamping between [0.0, 255.0]
    for( int i = 0; i < m_nImgWidth * m_nImgHeight * 4; i++ )
    {
        FLOAT32 val = pBGRAFloat[i];
        if( val < 0.0f )
            val = 0.0f;
        if( val > 255.0f )
            val = 255.0f;
        m_pRefResults[i] = (BYTE) val;
    }

    delete [] pBGRAFloat;
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

    // Amount of local memory to hold a row of the image
    size_t rowDataSize = m_nImgWidth * sizeof(cl_float) * 4;

    // Set the kernel arguments for horizontal gaussian filter
    errNum = clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_HORIZONTAL], 0, sizeof(cl_mem), &m_sourceImageRGBA );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_HORIZONTAL], 1, sizeof(cl_mem), &m_intermediateImage );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_HORIZONTAL], 2, sizeof(cl_sampler), &m_sampler );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_HORIZONTAL], 3, sizeof(cl_int), &m_nImgWidth );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_HORIZONTAL], 4, rowDataSize, NULL );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_HORIZONTAL], 5, sizeof(cl_float), &m_gaussParams.b0 );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_HORIZONTAL], 6, sizeof(cl_float), &m_gaussParams.b1 );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_HORIZONTAL], 7, sizeof(cl_float), &m_gaussParams.b2 );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_HORIZONTAL], 8, sizeof(cl_float), &m_gaussParams.b3 );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_HORIZONTAL], 9, sizeof(cl_float), &m_gaussParams.b4 );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments for horizontal kernel.\n" );
        return FALSE;
    }

    // Each work-item computes a full row of data
    size_t globalWorkSize[1] = { m_nImgHeight };
    size_t localWorkSize[1] = { 1 };

    // Queue the horizontal gaussian filter for execution
    errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernels[RECURSIVE_GAUSSIAN_HORIZONTAL], 1, NULL,
                        globalWorkSize, localWorkSize, 0, NULL, NULL );

    if( errNum != CL_SUCCESS )
    {
        FrmSprintf( str, sizeof(str), "Error queueing horizontal gaussian filter kernel for execution (%d).\n", errNum );
        FrmLogMessage( str );
        return FALSE;
    }

    // Amount of local memory to hold a column of the image
    size_t colDataSize = m_nImgHeight * sizeof(cl_float) * 4;

    // Each work-item computes a full column of data
    globalWorkSize[0] = m_nImgWidth;
    localWorkSize[0] = 1;


    // Set the kernel arguments for vertical gaussian filter
    errNum = clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_VERTICAL], 0, sizeof(cl_mem), &m_intermediateImage );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_VERTICAL], 1, sizeof(cl_mem), &m_destImage );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_VERTICAL], 2, sizeof(cl_sampler), &m_sampler );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_VERTICAL], 3, sizeof(cl_int), &m_nImgHeight );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_VERTICAL], 4, colDataSize, NULL );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_VERTICAL], 5, sizeof(cl_float), &m_gaussParams.b0 );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_VERTICAL], 6, sizeof(cl_float), &m_gaussParams.b1 );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_VERTICAL], 7, sizeof(cl_float), &m_gaussParams.b2 );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_VERTICAL], 8, sizeof(cl_float), &m_gaussParams.b3 );
    errNum |= clSetKernelArg( m_kernels[RECURSIVE_GAUSSIAN_VERTICAL], 9, sizeof(cl_float), &m_gaussParams.b4 );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments for vertical kernel.\n" );
        return FALSE;
    }

    // Queue the vertical gaussian filter for execution
    errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernels[RECURSIVE_GAUSSIAN_VERTICAL], 1, NULL,
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

    if( FALSE == FrmSaveImageAsTGA( "ImageRecursiveGaussianFilter.tga", m_nImgWidth, m_nImgHeight, (UINT32*) pRGBABits ) )
    {
        FrmLogMessage( "ERROR: writing output file 'ImageRecursiveGaussianFilter.tga'.\n" );
        return FALSE;
    }

    FrmLogMessage( "Wrote output file 'ImageRecursiveGaussianFilter.tga'.\n" );
    FrmSprintf( str, sizeof(str), "Results: recursive Gaussian filter on image ('%d x %d') in '%.6f' seconds.\n",
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
                // Check each color component
                for ( int c = 0; c < 4; c++ )
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

