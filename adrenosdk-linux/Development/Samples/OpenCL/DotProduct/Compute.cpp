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
#include <FrmPlatform.h>
#include <FrmComputeApplication.h>
#include <FrmUtils.h>
#include <OpenCL/FrmKernel.h>
#include "Compute.h"

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmComputeApplication* FrmCreateComputeApplicationInstance()
{
    return new CSample( "DotProduct" );
}

// To compile without optimization, remove QC_OPTS from android.mk

//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmComputeApplication( strName )
{
    m_commandQueue = 0;
    m_program = 0;
    m_kernel = 0;
    m_srcA = 0;
    m_srcB = 0;
    m_result = 0;
    m_nNumVectors = 5000000;
    m_pRefResults = NULL;
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    cl_int errNum;
    char str[256];

    if(!FrmOpenConsole())
        return FALSE;

    // Create the command queue
    m_commandQueue = clCreateCommandQueue( m_context, m_devices[0], CL_QUEUE_PROFILING_ENABLE, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to create command queue" );
        return FALSE;
    }

    const char* options;
    #ifdef QC_OPTS
        options = "-cl-fast-relaxed-math -DQC_OPTS";
    #else
        options = "-cl-fast-relaxed-math";
    #endif // QC_OPTS

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/DotProduct.cl", &m_program, m_context,
                                                  &m_devices[0], 1, options ) )
    {
        return FALSE;
    }

    // Create kernel
    m_kernel = clCreateKernel( m_program, "DotProduct", &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmSprintf(str, sizeof(str), "OpenCL Error: %d", errNum);
        FrmLogMessage( str );
        FrmLogMessage( "Failed to create kernel 'DotProduct'\n" );
        return FALSE;
    }

    FRMVECTOR4* pHostA;
    FRMVECTOR4* pHostB;

#ifdef QC_OPTS

    // This optimization uses 2D image as buffer.
    // We need to chop the data into 2D array for the image.
    size_t max_h, max_w;
    clGetDeviceInfo( m_devices[0], CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(max_w), &max_w, NULL );
    clGetDeviceInfo( m_devices[0], CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(max_h), &max_h, NULL );

    // Calculate the height and plus one additional row for remainder
    // if the total size is not divisible by max width
    m_bufHeight = (m_nNumVectors / max_w) + 1;
    m_bufWidth = max_w;

    FrmSprintf(str, sizeof(str), "Max image size %dx%d. create image %dx%d\n", max_w, max_h, m_bufWidth, m_bufHeight);
    FrmLogMessage( str );

    // creating image to hold buffer
    cl_image_format imageFormat;
    imageFormat.image_channel_data_type = CL_FLOAT;
    imageFormat.image_channel_order = CL_RGBA;
    cl_image_desc image_desc = { CL_MEM_OBJECT_IMAGE2D, m_bufWidth, m_bufHeight, 0, 0, 0, 0, 0, 0, 0};

    m_srcA = clCreateImage( m_context, CL_MEM_READ_ONLY, &imageFormat, &image_desc, 0, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmSprintf(str, sizeof(str), "ERROR: allocation device img buffer A: %d", errNum);
        FrmLogMessage( str );
        return FALSE;
    }

    m_srcB = clCreateImage( m_context, CL_MEM_READ_ONLY, &imageFormat, &image_desc, 0, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmSprintf(str, sizeof(str), "ERROR: allocation device img buffer B: %d", errNum);
        FrmLogMessage( str );
        return FALSE;
    }

    size_t origin[] = {0, 0, 0};
    size_t region[] = {m_bufWidth, m_bufHeight, 1};
    size_t row_pitch;

    // Map to host arrays
    pHostA = (FRMVECTOR4*) clEnqueueMapImage( m_commandQueue, m_srcA, CL_TRUE,
                                            CL_MAP_WRITE, origin, region, &row_pitch,
                                                           0, 0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmSprintf(str, sizeof(str), "ERROR: mapping device buffer A: %d", errNum);
        FrmLogMessage( str );
        return FALSE;
    }

    pHostB = (FRMVECTOR4*) clEnqueueMapImage( m_commandQueue, m_srcB, CL_TRUE,
                                            CL_MAP_WRITE, origin, region, &row_pitch,
                                                           0, 0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmSprintf(str, sizeof(str), "ERROR: mapping device buffer B: %d", errNum);
        FrmLogMessage( str );
        return FALSE;
    }

#else // unoptimized version

    // Create device buffers
    m_srcA = clCreateBuffer( m_context, CL_MEM_READ_ONLY, m_nNumVectors * sizeof(FRMVECTOR4), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation device buffer A" );
        return FALSE;
    }

    m_srcB = clCreateBuffer( m_context, CL_MEM_READ_ONLY, m_nNumVectors * sizeof(FRMVECTOR4), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation device buffer B" );
        return FALSE;
    }

    // Map to host arrays
    pHostA = (FRMVECTOR4*) clEnqueueMapBuffer( m_commandQueue, m_srcA, CL_TRUE, CL_MAP_WRITE, 0, m_nNumVectors * sizeof(FRMVECTOR4),
                                                           0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: mapping device buffer A" );
        return FALSE;
    }

    pHostB = (FRMVECTOR4*) clEnqueueMapBuffer( m_commandQueue, m_srcB, CL_TRUE, CL_MAP_WRITE, 0, m_nNumVectors * sizeof(FRMVECTOR4),
                                                           0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: mapping device buffer A" );
        return FALSE;
    }
#endif // QC_OPTS

    FrmSprintf(str, sizeof(str), "Initializing test data...\n");
    FrmLogMessage( str );
    // Fill with data
    FLOAT32 factor = 1 / m_nNumVectors;
    for( size_t i = 0; i < m_nNumVectors; i++ )
    {
        FLOAT32 valA = (FLOAT32)i * factor;
        FLOAT32 valB = 1.0f - valA;
        pHostA[i] = FRMVECTOR4( valA, valA, valA, valA );
        pHostB[i] = FRMVECTOR4( valB, valB, valB, valB );
    }

    FrmLogMessage( "Computing ref data...\n" );
    // Compute reference result if running test
    if( RunTests() == TRUE )
    {
        m_pRefResults = new FLOAT32 [ m_nNumVectors ];
        for( size_t i = 0; i < m_nNumVectors; i++ )
        {
            m_pRefResults[i] = FrmVector4Dot( pHostA[i], pHostB[i] );
        }
    }

    // Unmap buffers
    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_srcA, pHostA, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: unmapping device buffer A for writing.\n" );
        return FALSE;
    }

    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_srcB, pHostB, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: unmapping device buffer B for writing.\n" );
        return FALSE;
    }



    // Create result buffer
    m_result = clCreateBuffer( m_context, CL_MEM_READ_WRITE, m_nNumVectors * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation device host buffer result" );
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

    if( m_srcA != 0 )
    {
        clReleaseMemObject( m_srcA );
        m_srcA = 0 ;
    }

    if( m_srcB != 0 )
    {
        clReleaseMemObject( m_srcB );
        m_srcB = 0;
    }

    if( m_result != 0 )
    {
        clReleaseMemObject( m_result );
        m_result = 0;
    }

    if( m_pRefResults != NULL )
    {
        delete [] m_pRefResults;
        m_pRefResults = NULL;
    }
}



//--------------------------------------------------------------------------------------
// Name: Compute()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Compute()
{
    char str[256];

    m_Timer.Reset();
    m_Timer.Start();

    // Set the kernel arguments
    cl_int errNum = 0;

    size_t maxWorkGroupSize = 0;
    errNum = clGetKernelWorkGroupInfo( m_kernel, m_devices[0], CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL );
    if (errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: clGetKernelInfo on CL_KERNEL_WORK_GROUP_SIZE\n");
        return FALSE;
    }

    int dimension = 1;
#ifdef QC_OPTS // optimizing buffer access using image
    dimension = 2;
    errNum |= clSetKernelArg( m_kernel, 0, sizeof(cl_mem), &m_srcA );
    errNum |= clSetKernelArg( m_kernel, 1, sizeof(cl_mem), &m_srcB );
    errNum |= clSetKernelArg( m_kernel, 2, sizeof(cl_mem), &m_result );
    errNum |= clSetKernelArg( m_kernel, 3, sizeof(cl_int), &m_bufWidth );
    errNum |= clSetKernelArg( m_kernel, 4, sizeof(cl_int), &m_bufHeight );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments" );
        return FALSE;
    }

    size_t localWorkSize[2] = { (size_t) FrmSqrt( (FLOAT32) maxWorkGroupSize), (size_t) FrmSqrt( (FLOAT32)  maxWorkGroupSize) };
    size_t globalWorkSize[2];

    // Compute the next global size that is a multiple of the local size
    size_t remndr = m_bufWidth % localWorkSize[0];
    if( remndr == 0 )
        globalWorkSize[0] = m_bufWidth;
    else
        globalWorkSize[0] = m_bufWidth + localWorkSize[0] - remndr;

    remndr = m_bufHeight % localWorkSize[1];
    if( remndr == 0 )
        globalWorkSize[1] = m_bufHeight;
    else
        globalWorkSize[1] = m_bufHeight + localWorkSize[1] - remndr;

    FrmSprintf( str, sizeof(str), "Global WG:%dx%d. Local WG:%dx%d\n",
                    globalWorkSize[0], globalWorkSize[1], localWorkSize[0], localWorkSize[1] );
    FrmLogMessage( str );

#else // unoptimized version

    errNum |= clSetKernelArg( m_kernel, 0, sizeof(cl_mem), &m_srcA );
    errNum |= clSetKernelArg( m_kernel, 1, sizeof(cl_mem), &m_srcB );
    errNum |= clSetKernelArg( m_kernel, 2, sizeof(cl_mem), &m_result );
    errNum |= clSetKernelArg( m_kernel, 3, sizeof(cl_int), &m_nNumVectors );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments" );
        return FALSE;
    }

    size_t globalSize = m_nNumVectors;
    size_t remndr = m_nNumVectors % maxWorkGroupSize;
    globalSize = remndr? (m_nNumVectors+(maxWorkGroupSize-remndr)):m_nNumVectors;
    size_t globalWorkSize[1] = { globalSize };
    size_t localWorkSize[1] = { maxWorkGroupSize };

    FrmSprintf( str, sizeof(str), "Global WG:%d. Local WG: %d\n", globalWorkSize[0], localWorkSize[0] );
    FrmLogMessage( str );

#endif // QC_OPTS

    // Queue the kernel for execution
    errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernel, dimension, NULL,
                                    globalWorkSize, localWorkSize, 0, NULL, NULL );

    if( errNum != CL_SUCCESS )
    {
        FrmSprintf( str, sizeof(str), "Error queueing kernel for execution %d\n", errNum );
        FrmLogMessage( str );
        return FALSE;
    }

    // Read the result back to host memory
    FLOAT32* pResult;
    pResult = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_result, CL_TRUE, CL_MAP_READ, 0, sizeof(FLOAT32) * m_nNumVectors,
                                             0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error mapping buffer read." );
        return FALSE;
    }
    m_Timer.Stop();

    FrmSprintf( str, sizeof(str), "Results: '%d' dot products in '%.6f' seconds.\n", m_nNumVectors, m_Timer.GetTime() );
    FrmLogMessage( str );

    // Check against reference results
    BOOL result = true;
    if ( RunTests() )
    {
        BOOL result = TRUE;
        const FLOAT32 epsilon = 0.000001f;
        for ( size_t i = 0; i < m_nNumVectors; i++ )
        {
            if ( FrmAbs( pResult[i] - m_pRefResults[i] ) > epsilon )
            {
                FrmSprintf( str, sizeof(str), "Reference test failure, ref = (%f), result = (%f).\n", m_pRefResults[i], pResult[i] );
                FrmLogMessage( str );
                result = FALSE;
            }
        }
    }

    // Unmap buffer
    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_result, pResult, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error unmapping result buffer." );
        return FALSE;
    }

    return result;
}

