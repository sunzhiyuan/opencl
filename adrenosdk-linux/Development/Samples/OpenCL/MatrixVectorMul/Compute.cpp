//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2009 QUALCOMM Incorporated.
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
    return new CSample( "MatrixVectorMul" );
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
    m_matrix = 0;
    m_vector = 0;
    m_resultVector = 0;


    m_nMatrixRows = 2048;
    m_nMatrixCols = 2048;

    m_pRefResults = NULL;
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

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/MatrixVectorMul.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }

    // Create kernel
    m_kernel = clCreateKernel( m_program, "MatrixVectorMul", &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to create kernel 'DotProduct'\n" );
        return FALSE;
    }

    // Create device buffers
    size_t nNumFloats = m_nMatrixCols * m_nMatrixRows;
    m_matrix = clCreateBuffer( m_context, CL_MEM_READ_ONLY, nNumFloats * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device matrix" );
        return FALSE;
    }

    m_vector = clCreateBuffer( m_context, CL_MEM_READ_ONLY, m_nMatrixRows * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device vector" );
        return FALSE;
    }

    // Map buffers to host
    FLOAT32 *pMatrix = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_matrix, CL_TRUE, CL_MAP_WRITE, 0, sizeof(FLOAT32) * nNumFloats,
                                                      0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: mapping matrix buffer for writing.");
        return FALSE;
    }

    FLOAT32 *pVector = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_vector, CL_TRUE, CL_MAP_WRITE, 0, sizeof(FLOAT32) * m_nMatrixRows,
                                                      0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: mapping vector buffer for writing.");
        return FALSE;
    }

    // Fill with data
    for( cl_int row = 0; row < m_nMatrixRows; row++ )
    {
        pVector[ row ] = (FLOAT32)row / (FLOAT32)m_nMatrixRows;
        for( cl_int col = 0; col < m_nMatrixCols; col++ )
        {
            FLOAT32 val = (FLOAT32)col / (FLOAT32)m_nMatrixCols + (FLOAT32)row / (FLOAT32)m_nMatrixRows;
            pMatrix[ row * m_nMatrixCols + col ] = val;
        }
    }

    // Compute reference results if running tests
    if ( RunTests() )
    {
        m_pRefResults = new FLOAT32[ m_nMatrixRows ];

        for( cl_int row = 0; row < m_nMatrixRows; row++ )
        {
            FLOAT32 rowVal = 0.0f;
            for( cl_int col = 0; col < m_nMatrixCols; col++ )
            {
                rowVal += pMatrix[ row * m_nMatrixCols + col ] * pVector[col];
            }
            m_pRefResults[ row] = rowVal;
        }
    }

    // Unmap buffers
    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_matrix, pMatrix, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: unmapping matrix for writing.\n" );
        return FALSE;
    }

    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_vector, pVector, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: unmapping vector for writing.\n" );
        return FALSE;
    }

    // Create result vector buffer
    m_resultVector = clCreateBuffer( m_context, CL_MEM_READ_WRITE, m_nMatrixRows * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device buffer result vector" );
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

    if( m_matrix != 0 )
    {
        clReleaseMemObject( m_matrix );
        m_matrix = 0 ;
    }

    if( m_vector != 0 )
    {
        clReleaseMemObject( m_vector );
        m_vector = 0;
    }

    if( m_resultVector != 0 )
    {
        clReleaseMemObject( m_resultVector );
        m_resultVector = 0;
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

    // Set the kernel arguments
    cl_int errNum = 0;
    errNum |= clSetKernelArg( m_kernel, 0, sizeof(int), &m_nMatrixRows);
    errNum |= clSetKernelArg( m_kernel, 1, sizeof(int), &m_nMatrixCols );
    errNum |= clSetKernelArg( m_kernel, 2, sizeof(cl_mem), &m_matrix );
    errNum |= clSetKernelArg( m_kernel, 3, sizeof(cl_mem), &m_vector );
    errNum |= clSetKernelArg( m_kernel, 4, sizeof(cl_mem), &m_resultVector );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments" );
        return FALSE;
    }

    size_t globalWorkSize[1] = { m_nMatrixRows };
    size_t localWorkSize[1] = { 1 };

    // Queue the kernel for execution
    errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernel, 1, NULL,
                                    globalWorkSize, localWorkSize, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error queueing kernel for execution." );
        return FALSE;
    }
    clFinish( m_commandQueue );
    m_Timer.Stop();

    // Read the result back to host memory
    FLOAT32* pResultVector;
    pResultVector = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_resultVector, CL_TRUE, CL_MAP_READ, 0, sizeof(FLOAT32) * m_nMatrixRows,
                                                   0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error enqueuing buffer map." );
        return FALSE;
    }

    char str[256];
    FrmSprintf( str, sizeof(str), "Results: Multiplication of '%d x %d' matrix times vector in '%.6f' seconds.\n",
        m_nMatrixCols, m_nMatrixRows, m_Timer.GetTime() );
    FrmLogMessage( str );

    // Test results again CPU reference
    BOOL result = TRUE;
    if ( RunTests() )
    {
        const FLOAT32 epsilon = 0.1f;

        for( cl_int row = 0; row < m_nMatrixRows; row++ )
        {
            FLOAT32 refVal = m_pRefResults[ row ];
            FLOAT32 val = pResultVector[ row ] ;

            if( FrmAbs( refVal - val ) > epsilon )
            {
                FrmSprintf( str, sizeof(str), "Reference test failure, ref = (%f), result = (%f) Diff = (%f).\n", refVal, val, FrmAbs(refVal - val));
                FrmLogMessage( str );
                result = FALSE;
            }
        }
    }

    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_resultVector, pResultVector, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error unmapping result buffer." );
        return FALSE;
    }

    return result;
}

