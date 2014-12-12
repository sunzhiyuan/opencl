//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
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
    return new CSample( "MatrixTranspose" );
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
    m_matrixTranspose = 0;

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

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/MatrixTranspose.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }

    // Create kernel
    m_kernel = clCreateKernel( m_program, "MatrixTranspose", &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to create kernel 'MatrixTranspose'\n" );
        return FALSE;
    }

    // Allocate host arrys
    size_t nNumFloats = m_nMatrixCols * m_nMatrixRows;

    // Create device buffer
    m_matrix = clCreateBuffer( m_context, CL_MEM_READ_ONLY, nNumFloats * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device matrix" );
        return FALSE;
    }

    FLOAT32 *pMatrix;
    pMatrix = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_matrix, CL_TRUE, CL_MAP_WRITE, 0, sizeof(FLOAT32) * nNumFloats,
                                             0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error mapping device matrix buffer." );
        return FALSE;
    }

    // Fill with data
    for( cl_int row = 0; row < m_nMatrixRows; row++ )
    {
        for( cl_int col = 0; col < m_nMatrixCols; col++ )
        {
            FLOAT32 val;
            if ( row > col)
            {
                val = (FLOAT32)col / (FLOAT32)m_nMatrixCols + (FLOAT32)row / (FLOAT32)m_nMatrixRows;
            }
            else
            {
                val = 2.0f - (FLOAT32)col / (FLOAT32)m_nMatrixCols + (FLOAT32)row / (FLOAT32)m_nMatrixRows;
            }
            pMatrix[ row * m_nMatrixCols + col ] = val;
        }
    }

    // Compute transpose on the CPU if running tests
    if ( RunTests() )
    {
        m_pRefResults = new FLOAT32[ nNumFloats ];
        for( cl_int row = 0; row < m_nMatrixRows; row++ )
        {
            for( cl_int col = 0; col < m_nMatrixCols; col++ )
            {
                m_pRefResults[ col * m_nMatrixRows + row ] = pMatrix[ row * m_nMatrixCols + col ];
            }
        }
    }

    // Unmap matrix
    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_matrix, pMatrix, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error unmapping device matrix buffer" );
        return FALSE;
    }

    // Create result transposed matrix
    m_matrixTranspose = clCreateBuffer( m_context, CL_MEM_READ_WRITE, nNumFloats * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device transposed matrix result" );
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

    if( m_matrixTranspose != 0 )
    {
        clReleaseMemObject( m_matrixTranspose );
        m_matrixTranspose = 0;
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
    errNum |= clSetKernelArg( m_kernel, 3, sizeof(cl_mem), &m_matrixTranspose );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments.\n" );
        return FALSE;
    }

    size_t globalWorkSize[1] = { m_nMatrixRows };

    // Queue the kernel for execution
    errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernel, 1, NULL,
                                    globalWorkSize, NULL, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error queueing kernel for execution.\n" );
        return FALSE;
    }
    clFinish( m_commandQueue );
    m_Timer.Stop();

    // Read the result back to host memory
    FLOAT32* pResultMatrix;
    pResultMatrix = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_matrixTranspose, CL_TRUE, CL_MAP_READ, 0, sizeof(FLOAT32) * m_nMatrixCols * m_nMatrixRows,
                                                   0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error enqueuing map buffer." );
        return FALSE;
    }

    char str[256];

    FrmSprintf( str, sizeof(str), "Results: transpose of '%d x %d' matrix in '%.6f' seconds.\n",
        m_nMatrixRows, m_nMatrixCols, m_Timer.GetTime() );
    FrmLogMessage( str );
    BOOL result = TRUE;
    if ( RunTests() )
    {
        cl_int nTransposedRows = m_nMatrixCols;
        cl_int nTransposeCols = m_nMatrixRows;
        const FLOAT32 epsilon = 0.000001f;

        for( cl_int row = 0; row < nTransposedRows; row++)
        {
            for( cl_int col = 0; col < nTransposeCols; col++)
            {
                FLOAT32 refVal = m_pRefResults[ row * nTransposeCols + col ];
                FLOAT32 val = pResultMatrix[row * nTransposeCols + col] ;

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
    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_matrixTranspose, pResultMatrix, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error unmapping buffer object." );
        return FALSE;
    }
    return result;
}

