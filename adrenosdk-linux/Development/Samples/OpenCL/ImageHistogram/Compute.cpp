//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2014 QUALCOMM Incorporated.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <FrmPlatform.h>
#include <FrmComputeApplication.h>
#include <FrmUtils.h>
#include <OpenCL/FrmKernel.h>
#include "Compute.h"
#include <cstdlib>

#define CHECK_RESULT(errNum, msg) \
    if( errNum != CL_SUCCESS )\
    {\
        FrmSprintf( str, sizeof(str), msg" (%d).\n", errNum );\
        FrmLogMessage( str );\
        return FALSE;\
    }

// Set the number of iterations of target kernel execution to be averaged
#define TARGET_ITERATIONS 100

extern const char* g_pInternalDataPath;

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
    return new CSample( "ImageHistogram" );
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
    m_grayscaleImage(0),
    m_equalizedImage(0),
    m_colorReducedImage(0),
    m_groupStats(0),
    m_stats(0),
    m_cumulative(0),
    m_colormap(0),
    m_nImgWidth(0),
    m_nImgHeight(0),
    m_kernelHistogram(0),
    m_kernelHistogramLocal(0),
    m_kernelColorMapping(0)
{
    memset(m_refStats, 0, sizeof(unsigned int)*256);
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
    CHECK_RESULT(errNum, "Failed to create command queue");

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/ImageHistogram.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }

    // Create kernel
    m_kernelHistogram = clCreateKernel( m_program, "ImageGrayscaleHistogram_global", &errNum );
    CHECK_RESULT(errNum, "ERROR: Failed to create kernel 'ImageGrayscaleHistogram_global'");

    m_kernelHistogramLocal = clCreateKernel( m_program, "ImageGrayscaleHistogram_local", &errNum );
    CHECK_RESULT(errNum, "ERROR: Failed to create kernel 'ImageGrayscaleHistogram_local'");

    m_kernelColorMapping = clCreateKernel( m_program, "ImageMapped", &errNum );
    CHECK_RESULT(errNum, "ERROR: Failed to create kernel 'ImageMapped'");

    if (m_useLocal) m_opType = HISTOGRAM_LOCAL_SHARED_GLOBAL;
    else m_opType = HISTOGRAM_GLOBAL_ONLY;

    size_t minWorkGroupSize = 1;
    size_t maxWorkGroupSize = 0;

    switch (m_opType)
    {
    case HISTOGRAM_GLOBAL_ONLY:
        curKernel = m_kernelHistogram;
        FrmLogMessage( "Use global memory only.\n" );
        break;
    case HISTOGRAM_LOCAL_SHARED_GLOBAL:
        curKernel = m_kernelHistogramLocal;
        FrmLogMessage( "Use local + global memory.\n" );
        minWorkGroupSize = 64;
        break;
    default:
        break;
    }

    // Load image from file
    BYTE* pBGRAImage = LoadTGA( "Samples/Textures/SampleImageUncompressed.tga", &m_nImgWidth, &m_nImgHeight );
    if ( pBGRAImage == NULL )
    {
        return FALSE;
    }


    // Determine the max workgroup size, this wil be used to determine the local work size for the kernel
    errNum = clGetKernelWorkGroupInfo( curKernel, m_devices[0], CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL );
    CHECK_RESULT(errNum, "ERROR: clGetKernelInfo on CL_KERNEL_WORK_GROUP_SIZE");

    Select2DWGSize(maxWorkGroupSize, m_nImgWidth, m_nImgHeight, minWorkGroupSize);

    // Save the original image to display later
    if( FALSE == FrmSaveImageAsTGA( "ImageOriginal.tga", m_nImgWidth, m_nImgHeight, (UINT32*) pBGRAImage ) )
    {
        FrmLogMessage( "ERROR: writing output file 'ImageOriginal.tga'.\n" );
    }
    FrmLogMessage( "\n" );

    // Create OpenCL source image on device
    cl_image_format imageFormat;
    imageFormat.image_channel_data_type = CL_UNORM_INT8;
    imageFormat.image_channel_order = CL_BGRA;

    m_sourceImage = clCreateImage2D(
                        m_context,
                        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                        &imageFormat,
                        m_nImgWidth,
                        m_nImgHeight,
                        0,
                        pBGRAImage,
                        &errNum );
    CHECK_RESULT(errNum, "ERROR: creating source image on device");

    mapsize = 256 * sizeof(int);

    // Create device buffers
    m_stats = clCreateBuffer(
                m_context,
                CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                mapsize,
                NULL,
                &errNum );
    CHECK_RESULT(errNum, "ERROR: allocation device buffer m_stats");

    m_cumulative = clCreateBuffer(
                    m_context,
                    CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                    mapsize,
                    NULL,
                    &errNum );
    CHECK_RESULT(errNum, "ERROR: allocation device buffer m_cumulative");

    m_colormap = clCreateBuffer(
                    m_context,
                    CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                    mapsize,
                    NULL,
                    &errNum );
    CHECK_RESULT(errNum, "ERROR: allocation device buffer m_colormap");

    // Compute reference results
    if ( RunTests() )
    {
        ComputeRefResults( pBGRAImage );
    }
    delete [] pBGRAImage;

    imageFormat.image_channel_data_type = CL_UNSIGNED_INT8;
    // Create OpenCL destination image on device
    m_grayscaleImage = clCreateImage2D(
                        m_context,
                        CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR,
                        &imageFormat,
                        m_nImgWidth,
                        m_nImgHeight,
                        0,
                        NULL,
                        &errNum );
    CHECK_RESULT(errNum, "ERROR: creating destination grayscale image on device");

    // Create OpenCL destination image on device
    m_equalizedImage = clCreateImage2D(
                        m_context,
                        CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                        &imageFormat,
                        m_nImgWidth,
                        m_nImgHeight,
                        0,
                        NULL,
                        &errNum );
    CHECK_RESULT(errNum, "ERROR: creating destination equalized image on device");

    // Create OpenCL destination image on device
    m_colorReducedImage = clCreateImage2D(
                            m_context,
                            CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
                            &imageFormat,
                            m_nImgWidth,
                            m_nImgHeight,
                            0,
                            NULL,
                            &errNum );
    CHECK_RESULT(errNum, "ERROR: creating destination color reduced image on device");

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

    if( m_kernelHistogram != 0 )
    {
        clReleaseKernel( m_kernelHistogram );
        m_kernelHistogram = 0;
    }

    if( m_kernelHistogramLocal != 0 )
    {
        clReleaseKernel( m_kernelHistogramLocal );
        m_kernelHistogramLocal = 0;
    }

    if( m_kernelColorMapping != 0 )
    {
        clReleaseKernel( m_kernelColorMapping );
        m_kernelColorMapping = 0;
    }

    if( m_sourceImage != 0 )
    {
        clReleaseMemObject( m_sourceImage );
        m_sourceImage = 0;
    }

    if( m_grayscaleImage != 0 )
    {
        clReleaseMemObject( m_grayscaleImage );
        m_grayscaleImage = 0;
    }

    if( m_equalizedImage != 0 )
    {
        clReleaseMemObject( m_equalizedImage );
        m_equalizedImage = 0;
    }

    if( m_colorReducedImage != 0 )
    {
        clReleaseMemObject( m_colorReducedImage );
        m_colorReducedImage = 0;
    }

    if( m_stats != 0 )
    {
        clReleaseMemObject( m_stats );
        m_stats = 0;
    }

    if( m_groupStats != 0 )
    {
        clReleaseMemObject( m_groupStats );
        m_groupStats = 0;
    }

    if( m_cumulative != 0 )
    {
        clReleaseMemObject( m_cumulative );
        m_cumulative = 0;
    }

    if( m_colormap != 0 )
    {
        clReleaseMemObject( m_colormap );
        m_colormap = 0;
    }
}

//--------------------------------------------------------------------------------------
// Name: Compute()
// Desc: Main execution function of the sample
//--------------------------------------------------------------------------------------
BOOL CSample::Compute()
{
    // set the default iterations to 1 for CPU kernel execution
    cl_int iterations = TARGET_ITERATIONS, errNum = 0;
    double elapsed_time = 0;
    BOOL result = TRUE;

    //
    // Apply image histogram
    //
    cl_mem* curStatsBuf = &m_stats;

    // Set the kernel arguments
    errNum  = clSetKernelArg( curKernel, 0, sizeof(cl_mem), &m_sourceImage );
    errNum |= clSetKernelArg( curKernel, 1, sizeof(cl_mem), &m_grayscaleImage );
    errNum |= clSetKernelArg( curKernel, 2, sizeof(cl_mem), curStatsBuf );
    errNum |= clSetKernelArg( curKernel, 3, sizeof(int), &m_nImgWidth );
    errNum |= clSetKernelArg( curKernel, 4, sizeof(int), &m_nImgHeight );
    CHECK_RESULT(errNum, "Error setting histogram kernel arguments");

    size_t *localWorkSize;
    if (m_WG2DWHRatio == 0) localWorkSize = NULL;
    else localWorkSize = m_localWorkSize;

    // Queue the kernel for execution
    cl_event kernel_event;
    m_Timer.Reset();
    // Do multiple iterations to get average kernel execution time
    for (int i = 0; i < iterations; ++i)
    {
        // reset stats buffer before doing histogram
        int* pStats;
        pStats = (int*) clEnqueueMapBuffer(
                            m_commandQueue,
                            m_stats,
                            CL_TRUE,
                            CL_MAP_WRITE,
                            0,
                            mapsize,
                            0,
                            NULL,
                            NULL,
                            &errNum );
        CHECK_RESULT(errNum, "Error mapping buffer read");
        memset (pStats, 0, mapsize);
        // Unmap buffer
        errNum = clEnqueueUnmapMemObject(
                    m_commandQueue,
                    m_stats,
                    pStats,
                    0,
                    NULL,
                    NULL );
        CHECK_RESULT(errNum, "Error unmapping result buffer");

        m_Timer.Start();
        errNum = clEnqueueNDRangeKernel(
                    m_commandQueue,
                    curKernel,
                    2,
                    NULL,
                    m_globalWorkSize,
                    localWorkSize,
                    0,
                    NULL,
                    &kernel_event );
        CHECK_RESULT(errNum, "Error queueing kernel histogram for execution");

        clFinish( m_commandQueue );
        m_Timer.Stop();
    }

    // average the elapsed time over all the iterations
    // Convert seconds into milliseconds
    elapsed_time = m_Timer.GetTime() * 1000.0f / (float)iterations;

    // Read the result back to host memory
    int* pStats;
    pStats = (int*) clEnqueueMapBuffer(
                        m_commandQueue, m_stats,
                        CL_TRUE,
                        CL_MAP_READ,
                        0,
                        mapsize,
                        0,
                        0,
                        0,
                        &errNum );
    CHECK_RESULT(errNum, "Error mapping buffer read");

    int colorStats[256];
    memcpy(colorStats, pStats, mapsize);

    // Unmap buffer
    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_stats, pStats, 0, NULL, NULL );
    CHECK_RESULT(errNum, "Error unmapping result buffer");

    // check histogram result
    if ( RunTests() )
    {
        int totalRef = 0, totalCount = 0;
        for (int i  = 0; i < 256; ++i)
        {
            totalRef += m_refStats[i];
            totalCount += colorStats[i];
            int diff = colorStats[i] - m_refStats[i];
            diff = diff < 0 ? -diff : diff;

            if ( (m_refStats[i] == 0 && diff > 5) ||
                 ((float)diff / (float)m_refStats[i] > 0.05) )
            {
                FrmSprintf( str, sizeof(str), "Reference test failure, %d ref = (%d), result = (%d)\n", i, m_refStats[i], colorStats[i]);
                FrmLogMessage( str );
                result = FALSE;
            }
        }
        FrmSprintf( str, sizeof(str), "Total processed pixel: %d / %d\n", totalCount, totalRef);
        FrmLogMessage( str );

    }

    FrmSprintf( str, sizeof(str), "Results: Histogram on image ('%d x %d') in '%.3f' milliseconds.\n",
            m_nImgWidth, m_nImgHeight, elapsed_time );
    FrmLogMessage( str );

    ApplyUseCase (colorStats);

    return result;
}

//--------------------------------------------------------------------------------------
// Name: ComputeRefResults()
// Desc: For RunTests() mode, compute grayscale histogram
//--------------------------------------------------------------------------------------
VOID CSample::ComputeRefResults( BYTE* pBGRAImage )
{
    float rFactor = 76.f / 255.f;
    float gFactor = 150.f / 255.f;
    float bFactor = 29.f / 255.f;

    for ( int y = 0; y < m_nImgHeight; y++ )
    {
        for ( int x = 0; x < m_nImgWidth; x++ )
        {
            int index = (y * m_nImgWidth + x) * 4;
            // r=2, g=1, b=0
            BYTE grayscale = (float)pBGRAImage[ index+2] * rFactor +
                              (float)pBGRAImage[ index+1] * gFactor +
                              (float)pBGRAImage[ index] * bFactor;
            m_refStats[grayscale]++;
        }
    }

}


//--------------------------------------------------------------------------------------
// Name: ApplyUseCase()
// Desc: Using histogram, apply image equalization and color reduction
//--------------------------------------------------------------------------------------
BOOL CSample::ApplyUseCase (int* colorStats)
{
    cl_int errNum = 0;
    BOOL result = TRUE;
    //
    // Use case: image equalization
    //
    // calculate cumulative function
    int* pCumu;
    pCumu = (int*) clEnqueueMapBuffer(
                    m_commandQueue,
                    m_cumulative,
                    CL_TRUE,
                    CL_MAP_WRITE | CL_MAP_READ,
                    0,
                    mapsize,
                    0,
                    0,
                    0,
                    &errNum );
    CHECK_RESULT(errNum, "Error mapping buffer read");

    EqualizeColors(colorStats, pCumu);

    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_cumulative, pCumu, 0, NULL, NULL );
    CHECK_RESULT(errNum, "Error unmapping result buffer");

    size_t localWG[2] = {8,8};
    size_t globalWG[2] = {m_nImgWidth, m_nImgHeight};
    // apply equalization to the image
    errNum  = clSetKernelArg( m_kernelColorMapping, 0, sizeof(cl_mem), &m_grayscaleImage );
    errNum |= clSetKernelArg( m_kernelColorMapping, 1, sizeof(cl_mem), &m_equalizedImage );
    errNum |= clSetKernelArg( m_kernelColorMapping, 2, sizeof(cl_mem), &m_cumulative );
    errNum |= clSetKernelArg( m_kernelColorMapping, 3, sizeof(int), &m_nImgWidth );
    errNum |= clSetKernelArg( m_kernelColorMapping, 4, sizeof(int), &m_nImgHeight );
    CHECK_RESULT(errNum, "Error setting kernel arguments");

    FrmSprintf( str, sizeof(str), "Use Case: global WG(%dx%d) local WG(%dx%d)\n", globalWG[0], globalWG[1],
                    localWG[0], localWG[1]);
    FrmLogMessage( str );

    errNum = clEnqueueNDRangeKernel(
                m_commandQueue,
                m_kernelColorMapping,
                2,
                NULL,
                globalWG,
                localWG,
                0,
                0,
                NULL );
    CHECK_RESULT(errNum, "Error queueing kernel equalization for execution");
    clFinish( m_commandQueue );

    //
    // Use case: color reduction
    //
    // calculate reduced color
    int* pColorMap;
    pColorMap = (int*) clEnqueueMapBuffer(
                        m_commandQueue,
                        m_colormap,
                        CL_TRUE,
                        CL_MAP_WRITE | CL_MAP_READ,
                        0,
                        mapsize,
                        0,
                        0,
                        0,
                        &errNum );
    CHECK_RESULT(errNum, "Error mapping buffer read");

    // reduce colors to 2^2
    ReduceColors (colorStats, pColorMap, 2);

    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_colormap, pColorMap, 0, NULL, NULL );
    CHECK_RESULT(errNum, "Error unmapping result buffer");

    // apply color reduction to the image
    errNum  = clSetKernelArg( m_kernelColorMapping, 0, sizeof(cl_mem), &m_grayscaleImage );
    errNum |= clSetKernelArg( m_kernelColorMapping, 1, sizeof(cl_mem), &m_colorReducedImage );
    errNum |= clSetKernelArg( m_kernelColorMapping, 2, sizeof(cl_mem), &m_colormap );
    errNum |= clSetKernelArg( m_kernelColorMapping, 3, sizeof(int), &m_nImgWidth );
    errNum |= clSetKernelArg( m_kernelColorMapping, 4, sizeof(int), &m_nImgHeight );
    CHECK_RESULT(errNum, "Error setting kernel arguments");

    errNum = clEnqueueNDRangeKernel(
                m_commandQueue,
                m_kernelColorMapping,
                2,
                NULL,
                globalWG,
                localWG,
                0,
                0,
                NULL );

    CHECK_RESULT(errNum, "Error queueing kernel color reduction for execution");
    clFinish( m_commandQueue );


    //
    // Read the image back to host memory and write to disk
    //
    BYTE* pRGBABits = new BYTE[ m_nImgWidth * m_nImgHeight * 4 ];
    size_t origin[3] = { 0, 0, 0 };
    size_t region[3] = { m_nImgWidth, m_nImgHeight, 1 } ;

    // Write out grayscale image
    errNum = clEnqueueReadImage(
                m_commandQueue,
                m_grayscaleImage,
                CL_TRUE,
                origin,
                region,
                0,
                0,
                pRGBABits,
                0,
                NULL,
                NULL );
    CHECK_RESULT(errNum, "ERROR: reading grayscale image back to host");
    if( FALSE == FrmSaveImageAsTGA( "ImageGrayscale.tga", m_nImgWidth, m_nImgHeight, (UINT32*) pRGBABits ) )
    {
        FrmLogMessage( "ERROR: writing output file 'ImageGrayscale.tga'.\n" );
    }
    FrmLogMessage( "\n" );

    // Write out color equalized image
    errNum = clEnqueueReadImage(
                m_commandQueue,
                m_equalizedImage,
                CL_TRUE,
                origin,
                region,
                0,
                0,
                pRGBABits,
                0,
                NULL,
                NULL );
    CHECK_RESULT(errNum, "ERROR: reading equalized image back to host");

    if( FALSE == FrmSaveImageAsTGA( "ImageEqualized.tga", m_nImgWidth, m_nImgHeight, (UINT32*) pRGBABits ) )
    {
        FrmLogMessage( "ERROR: writing output file 'ImageEqualized.tga'.\n" );
    }
    FrmLogMessage( "\n" );

    // Write out color reduced image
    errNum = clEnqueueReadImage(
                m_commandQueue,
                m_colorReducedImage,
                CL_TRUE,
                origin,
                region,
                0,
                0,
                pRGBABits,
                0,
                NULL,
                NULL );
    CHECK_RESULT(errNum, "ERROR: reading color reduced image back to host");

    if( FALSE == FrmSaveImageAsTGA( "ImageColorReduced.tga", m_nImgWidth, m_nImgHeight, (UINT32*) pRGBABits ) )
    {
        FrmLogMessage( "ERROR: writing output file 'ImageColorReduced.tga'.\n" );
    }
    FrmLogMessage( "\n" );

    delete [] pRGBABits;


    char fullPath[1024];
    FrmSprintf( fullPath, sizeof(fullPath), "%s/%s", g_pInternalDataPath, "ImageOriginal.tga" );
    FrmAndroidUpdateImage( 0, fullPath );
    FrmSprintf( fullPath, sizeof(fullPath), "%s/%s", g_pInternalDataPath, "ImageGrayscale.tga" );
    FrmAndroidUpdateImage( 1, fullPath );
    FrmSprintf( fullPath, sizeof(fullPath), "%s/%s", g_pInternalDataPath, "ImageEqualized.tga" );
    FrmAndroidUpdateImage( 2, fullPath );
    FrmSprintf( fullPath, sizeof(fullPath), "%s/%s", g_pInternalDataPath, "ImageColorReduced.tga" );
    FrmAndroidUpdateImage( 3, fullPath );

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: PickMedian()
// Desc: Return the median of an integer array colorStats from the range of start to end
//--------------------------------------------------------------------------------------
int CSample::PickMedian (int* colorStats, int start, int end)
{
    int range = 0;
    for (int i = start; i <= end; ++i)
    {
        range += colorStats[i];
    }
    range /= 2;
    int sum = 0;
    for (int i = start; i <= end; ++i)
    {
        sum += colorStats[i];
        if (sum >= range)
        {
            return i;
        }
    }
    return start + (end-start)/2;
}

//--------------------------------------------------------------------------------------
// Name: PickColor()
// Desc: In grayscale (0-255), for each level, pick 2 most common colors to represent
//       the range from start to end. Recursively pick colors until no more level.
//       Most common color is defined as the median color within a range.
//--------------------------------------------------------------------------------------
void CSample::PickColors (int* colorStats, int start, int end, int *picked, int* numPicked, int level)
{
    int range = 0;
    for (int i = start; i <= end; ++i)
    {
        range += colorStats[i];
    }
    range /= 2;
    int mid = PickMedian(colorStats, start, end);

    if (level == 1)
    {
        picked[*numPicked] = PickMedian(colorStats, start, mid);
        picked[*numPicked+1] = PickMedian(colorStats, mid, end);
        *numPicked += 2;
        return;
    }

    PickColors(colorStats, start, mid, picked, numPicked, level-1);
    PickColors(colorStats, mid, end, picked, numPicked, level-1);
}

//--------------------------------------------------------------------------------------
// Name: ReduceColors()
// Desc: Based on grayscale histogram, pick N colors and store color mapping in pColorMap.
//       N is (2 ^ divided_level).
//--------------------------------------------------------------------------------------
void CSample::ReduceColors (int* colorStats, int* pColorMap, int divided_level)
{
    memset(pColorMap, 0, mapsize);
    int numColors = 1 << divided_level;
    int* pickedColor = new int[numColors];
    int numPicked = 0;
    // pick N number of colors
    PickColors (colorStats, 0, 255, pickedColor, &numPicked, divided_level);

    // map the colors to the closest picked colors
    int curPicked = 0;
    for (int i = 0; i < 256; ++i)
    {
        int min = 256;
        int mapped = i;
        for (int j = 0; j < numColors; ++j)
        {
            if (abs(pickedColor[j] - i) < min)
            {
                mapped = pickedColor[j];
                min = abs(pickedColor[j] - i);
            }
        }
        pColorMap[i] = mapped;
    }
    delete [] pickedColor;
}

//--------------------------------------------------------------------------------------
// Name: CumulativeFunc()
// Desc: Based on grayscale histogram, calculate cumulative distribution function then
//       apply it to produce an equalized grayscale vector pCumu.
//--------------------------------------------------------------------------------------
void CSample::EqualizeColors(int* colorStats, int* pCumu)
{
    pCumu[0] = colorStats[0];
    int primeColor = 0;
    for (int i = 1; i < 256; ++i)
    {
        pCumu[i] = pCumu[i-1] + colorStats[i];
    }
    for (int i = 0; i < 255; ++i)
    {
        pCumu[i] = (int)(255.f * (float)pCumu[i] / (float)pCumu[255]);
    }
    pCumu[255] = 255;
}

