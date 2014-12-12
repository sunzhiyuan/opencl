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
    return new CSample( "VectorAdd" );
}


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
    m_nNumVectors = 1024;
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

    // Create the command queue
    m_commandQueue = clCreateCommandQueue( m_context, m_devices[0], CL_QUEUE_PROFILING_ENABLE, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to create command queue" );
        return FALSE;
    }

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/VectorAdd.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }

    // Create kernel
    m_kernel = clCreateKernel( m_program, "VectorAdd", &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to create kernel 'VectorAdd'\n" );
        return FALSE;
    }

     // Create device buffers
    m_srcA = clCreateBuffer( m_context, CL_MEM_READ_ONLY, m_nNumVectors * sizeof(FRMVECTOR4), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation device host buffer A" );
        return FALSE;
    }

    m_srcB = clCreateBuffer( m_context, CL_MEM_READ_ONLY, m_nNumVectors * sizeof(FRMVECTOR4), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation device host buffer B" );
        return FALSE;
    }

    // Map to host arrys
    FRMVECTOR4 *pHostA = (FRMVECTOR4*) clEnqueueMapBuffer( m_commandQueue, m_srcA, CL_TRUE, CL_MAP_WRITE, 0, sizeof(FRMVECTOR4) * m_nNumVectors,
                                                           0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: mapping device buffer A." );
        return FALSE;
    }

    FRMVECTOR4 *pHostB = (FRMVECTOR4*) clEnqueueMapBuffer( m_commandQueue, m_srcB, CL_TRUE, CL_MAP_WRITE, 0, sizeof(FRMVECTOR4) * m_nNumVectors,
                                                           0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: mapping device buffer B." );
        return FALSE;
    }

    // Fill with data
    for( size_t i = 0; i < m_nNumVectors; i++ )
    {
        FLOAT32 valA = (FLOAT32)i / m_nNumVectors;
        FLOAT32 valB = 1.0f - valA;
        pHostA[i] = FRMVECTOR4( valA, valA, valA, valA );
        pHostB[i] = FRMVECTOR4( valB, valB, valB, valB );
    }



    // Compute reference results on CPU
    if ( RunTests() )
    {
        m_pRefResults = new FRMVECTOR4[ m_nNumVectors ];
        for( size_t i = 0; i < m_nNumVectors; i++ )
        {
            m_pRefResults[ i ] = pHostA[ i ] + pHostB[ i ];
        }
    }

    // Unmap buffers
    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_srcA, pHostA, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: Unmapping buffer A." );
        return FALSE;
    }

    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_srcB, pHostB, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: Unmapping buffer B." );
        return FALSE;
    }

    // Create result buffer
    m_result = clCreateBuffer( m_context, CL_MEM_READ_WRITE, m_nNumVectors * sizeof(FRMVECTOR4), NULL, &errNum );
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

    delete [] m_pRefResults;
}



//--------------------------------------------------------------------------------------
// Name: Compute()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Compute()
{
    m_Timer.Reset();
    m_Timer.Start();

    char str[256];

    // Set the kernel arguments
    cl_int errNum = 0;
    errNum |= clSetKernelArg( m_kernel, 0, sizeof(cl_mem), &m_srcA );
    errNum |= clSetKernelArg( m_kernel, 1, sizeof(cl_mem), &m_srcB );
    errNum |= clSetKernelArg( m_kernel, 2, sizeof(cl_mem), &m_result );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments" );
        return FALSE;
    }

    size_t globalWorkSize[1] = { m_nNumVectors };
    size_t localWorkSize[1] = { 1 };

    cl_event kernel_event;
    cl_ulong t_queued=0, t_submit=0, t_start=0, t_end=0;

    // Queue the kernel for execution
    errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernel, 1, NULL,
                                    globalWorkSize, localWorkSize, 0, NULL, &kernel_event );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error queueing kernel for execution." );
        return FALSE;
    }

    clWaitForEvents(1 , &kernel_event);

    // Query timestamp for kernel profiling
    //   Queued time is when the command is queued to host.
    //   Submit time is when the command is submitted from host to device.
    //   Start time is when the command starts the execution.
    //   End time is when the command finishes the execution.
    // The delta between start and end, marks the total elapsed time to execute a kernel in device.
    errNum = clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_QUEUED,
                sizeof(cl_ulong), &t_queued, NULL);
    if( errNum != CL_SUCCESS ) FrmLogMessage( "Error getting queued timestamp." );
    errNum = clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_SUBMIT,
                sizeof(cl_ulong), &t_submit, NULL);
    if( errNum != CL_SUCCESS ) FrmLogMessage( "Error getting submit timestamp." );
    errNum = clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_START,
                sizeof(cl_ulong), &t_start,    NULL);
    if( errNum != CL_SUCCESS ) FrmLogMessage( "Error getting start timestamp." );
    errNum = clGetEventProfilingInfo(kernel_event, CL_PROFILING_COMMAND_END,
                sizeof(cl_ulong), &t_end, NULL);
    if( errNum != CL_SUCCESS ) FrmLogMessage( "Error getting end timestamp." );

    FrmLogMessage("Kernel event profiling....(nano sec)\n");
    FrmSprintf(str, sizeof(str), "  -> Queued time: %lu\n", t_queued);
    FrmLogMessage( str );
    FrmSprintf(str, sizeof(str), "  -> Submit time: %lu\n", t_submit);
    FrmLogMessage( str );
    FrmSprintf(str, sizeof(str), "  -> Start time:  %lu\n", t_start);
    FrmLogMessage( str );
    FrmSprintf(str, sizeof(str), "  -> End time:    %lu\n", t_end);
    FrmLogMessage( str );

    clReleaseEvent(kernel_event);

    // Read the result back to host memory
    FRMVECTOR4* pResult;
    pResult = (FRMVECTOR4*) clEnqueueMapBuffer( m_commandQueue, m_result, CL_TRUE, CL_MAP_READ, 0, sizeof(FRMVECTOR4) * m_nNumVectors,
                                                0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error enqueuing buffer map." );
        return FALSE;
    }
    m_Timer.Stop();

    FrmSprintf( str, sizeof(str), "Results: '%d' vector additions in '%.6f' seconds.\n", m_nNumVectors, m_Timer.GetTime() );
    FrmLogMessage( str );

    // Test results again CPU reference
    BOOL result = TRUE;
    if ( RunTests() )
    {
        const FLOAT32 epsilon = 0.000001f;

        for( size_t i = 0; i < m_nNumVectors; i++ )
        {
            for ( size_t j = 0; j < 4; j++ )
            {
                FLOAT32 refVal = m_pRefResults[ i ].v[ j ];
                FLOAT32 val = pResult[ i ].v[ j ];

                if( FrmAbs( refVal - val ) > epsilon )
                {
                    FrmSprintf( str, sizeof(str), "Reference test failure, ref = (%f), result = (%f) Diff = (%f).\n", refVal, val, FrmAbs(refVal - val));
                    FrmLogMessage( str );
                    result = FALSE;
                }
            }
        }

    }

    // Unmap buffer
    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_result, pResult, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: Unmapping result buffer." );
        return FALSE;
    }
    return result;
}

