//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
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
    return new CSample( "MatrixMatrixAdd" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmComputeApplication( strName )
{
    m_commandQueue = 0;
    m_program = 0;
    m_matrixA = 0;
    m_matrixB = 0;
    m_matrixSum = 0;

    m_nMatrixRows = 2048;
    m_nMatrixCols = 2048;

    m_pRefResult = NULL;


    for( int i = 0; i < NUM_MATRIX_ADD_ALGS; i++ )
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

    // Create the command queue
    m_commandQueue = clCreateCommandQueue( m_context, m_devices[0], CL_QUEUE_PROFILING_ENABLE, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to create command queue" );
        return FALSE;
    }

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/MatrixMatrixAdd.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }


    const CHAR* kernelNames[NUM_MATRIX_ADD_ALGS] =
    {
        "MatrixMatrixAddSimple",
        "MatrixMatrixAddOptimized1",
        "MatrixMatrixAddOptimized2",
    };
    for( int i = 0; i < NUM_MATRIX_ADD_ALGS; i++)
    {
        // Create kernel
        m_kernels[i] = clCreateKernel( m_program, kernelNames[i], &errNum );
        if ( errNum != CL_SUCCESS )
        {
            CHAR str[256];
            FrmSprintf( str, sizeof(str), "ERROR: Failed to create kernel '%s'.\n", kernelNames[i] );
            FrmLogMessage( str );
            return FALSE;
        }
    }

    size_t nNumFloats = m_nMatrixRows * m_nMatrixCols;

    // Create device buffers
    m_matrixA = clCreateBuffer( m_context, CL_MEM_READ_ONLY, nNumFloats * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device matrix A.\n" );
        return FALSE;
    }

    m_matrixB = clCreateBuffer( m_context, CL_MEM_READ_ONLY, nNumFloats * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device matrix B.\n" );
        return FALSE;
    }

    // Map A for writing
    FLOAT32 *pMatrixA = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_matrixA, CL_TRUE, CL_MAP_WRITE, 0, nNumFloats * sizeof(FLOAT32),
        0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: mapping buffer for writing.\n" );
        return FALSE;
    }

    // Fill A with data
    for( cl_int row = 0; row < m_nMatrixRows; row++ )
    {
        for( cl_int col = 0; col < m_nMatrixCols; col++ )
        {
            FLOAT32 val = (FLOAT32)col / (FLOAT32)m_nMatrixCols + (FLOAT32)row / (FLOAT32)m_nMatrixRows;
            pMatrixA[ row * m_nMatrixCols + col ] = val;
        }
    }

    // Map B for writing
    FLOAT32 *pMatrixB = (FLOAT32*)clEnqueueMapBuffer( m_commandQueue, m_matrixB, CL_TRUE, CL_MAP_WRITE, 0, nNumFloats * sizeof(FLOAT32),
        0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: mapping buffer for writing.\n" );
        return FALSE;
    }

    // Fill B with data
    for( cl_int row = 0; row < m_nMatrixRows; row++ )
    {
        for( cl_int col = 0; col < m_nMatrixCols; col++ )
        {
            FLOAT32 val = 2.0f - (FLOAT32)col / (FLOAT32)m_nMatrixCols + (FLOAT32)row / (FLOAT32)m_nMatrixCols;
            pMatrixB[ row * m_nMatrixCols + col ] = val;
        }
    }

    // Compute reference results if testing
    if ( RunTests() )
    {
        FrmLogMessage( "Computing reference results on CPU, this may take a while...\n");
        TestAdd( pMatrixA, pMatrixB );
        FrmLogMessage( "Computed reference results on CPU.\n");
    }

    // Unmap A
    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_matrixA, pMatrixA, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: unmapping matrixA for writing.\n" );
        return FALSE;
    }

    // Unmap B
    errNum = clEnqueueUnmapMemObject( m_commandQueue, m_matrixB, pMatrixB, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: unmapping matrixB for writing.\n" );
        return FALSE;
    }

    // Create result matrixSum
    m_matrixSum = clCreateBuffer( m_context, CL_MEM_READ_WRITE, nNumFloats * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device result matrix C.\n" );
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

    for( int i = 0; i < NUM_MATRIX_ADD_ALGS; i++ )
    {
        if( m_kernels[i] != 0 )
        {
            clReleaseKernel( m_kernels[i] );
            m_kernels[i] = 0;
        }
    }

    if( m_matrixA != 0 )
    {
        clReleaseMemObject( m_matrixA );
        m_matrixA = 0;
    }

    if( m_matrixB != 0 )
    {
        clReleaseMemObject( m_matrixB );
        m_matrixB = 0;
    }

    if( m_matrixSum != 0 )
    {
        clReleaseMemObject( m_matrixSum );
        m_matrixSum = 0;
    }

    delete [] m_pRefResult;
}

//--------------------------------------------------------------------------------------
// Name: TestAdd()
// Desc: Compute reference CPU addition of matrices
//--------------------------------------------------------------------------------------
VOID CSample::TestAdd( const FLOAT32* pMatrixA, const FLOAT32* pMatrixB )
{
    m_pRefResult = new FLOAT32[ m_nMatrixRows * m_nMatrixCols ];

    for( INT32 i = 0; i < m_nMatrixRows; i++ )
    {
        for( INT32 j = 0; j < m_nMatrixCols; j++ )
        {
            FLOAT32 result = 0.0f;
        int index      = i * m_nMatrixCols + j;

            m_pRefResult[index] = pMatrixA[index] + pMatrixB[index];
        }
    }
}

//--------------------------------------------------------------------------------------
// Name: PrintMatrix()
// Desc: Print matrix contents to log
//--------------------------------------------------------------------------------------
VOID CSample::PrintMatrix( INT32 rows, INT32 cols, const FLOAT32* pMatrix)
{
    CHAR str[256];
    for( INT32 row = 0; row < rows; row++)
    {
        FrmLogMessage( "[ " );
        for( INT32 col = 0; col < cols; col++)
        {
            FrmSprintf( str, sizeof(str), "%0.2f ", pMatrix[ row * cols + col] );
            FrmLogMessage( str );
        }
        FrmLogMessage( "]\n" );
    }

    FrmLogMessage( "\n" );
}

//--------------------------------------------------------------------------------------
// Name: Compute()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Compute()
{
    char str[256];
    cl_int errNum = 0;
    cl_device_type devType;

    for( int i = 0; i < NUM_MATRIX_ADD_ALGS; i++ )
    {
        cl_kernel kernel = m_kernels[i];

        errNum = clGetDeviceInfo(m_devices[0], CL_DEVICE_TYPE, sizeof(devType), &devType, NULL);
        if ( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Failed to obtain device type" );
            return FALSE;
        }

        /*
         * CPU version only works on simple mode. There is issue with work group size with the optimized version for CPU.
         */
        if (devType == CL_DEVICE_TYPE_CPU && i == MATRIX_ADD_OPTIMIZED2)
        {
            continue;
        }

        // Set the kernel arguments
        errNum = clSetKernelArg( kernel, 0, sizeof(int), &m_nMatrixRows);
        errNum |= clSetKernelArg( kernel, 1, sizeof(int), &m_nMatrixCols );
        errNum |= clSetKernelArg( kernel, 2, sizeof(cl_mem), &m_matrixA );
        errNum |= clSetKernelArg( kernel, 3, sizeof(cl_mem), &m_matrixB );
        errNum |= clSetKernelArg( kernel, 4, sizeof(cl_mem), &m_matrixSum );

        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error setting kernel arguments.\n" );
            return FALSE;
        }

    // memset matrixSum to 0 as it gets reused for both simple and optimized algorithms
        FLOAT32* pMatrixSum = NULL;
        pMatrixSum = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_matrixSum, CL_TRUE, CL_MAP_WRITE, 0,
                                                  sizeof(FLOAT32) * m_nMatrixRows * m_nMatrixCols, 0, NULL, NULL, &errNum );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error enqueuing map buffer." );
            return FALSE;
        }

    memset(pMatrixSum, 0, sizeof(FLOAT32) * m_nMatrixRows * m_nMatrixCols);

        errNum = clEnqueueUnmapMemObject( m_commandQueue, m_matrixSum, pMatrixSum, 0, NULL, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error unmapping mem object" );
            return FALSE;
        }
        switch (i)
        {
        case MATRIX_ADD_SIMPLE:
            // Use a 2D global work size where each work-item computes an element of C
            {
                size_t globalWorkSize[2] = { m_nMatrixRows, m_nMatrixCols };

                FrmSprintf( str, sizeof(str), "---- Running Simple Matrix Addition ---\n");
                FrmLogMessage( str );

                m_Timer.Reset();
                m_Timer.Start();

                // Queue the kernel for execution
                errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 2, NULL,
                                    globalWorkSize, NULL, 0, NULL, NULL );
                if( errNum != CL_SUCCESS )
                {
                    FrmSprintf( str, sizeof(str), "Error queueing kernel for execution (%d).\n", errNum );
                    FrmLogMessage( str );
                    return FALSE;
                }
            }
            break;

        case MATRIX_ADD_OPTIMIZED1:
            // Use a 1D global work size where each work-item computes a row of C.  Partition
            // the workgroups based on device limits
            {
                FrmSprintf( str, sizeof(str), "---- Running Optimized1 Matrix Addition ---\n");
                FrmLogMessage( str );
                // Determine the max workgroup size, this wil be used to determine the local work size
                // for the optimized version of the kernel
                size_t maxWorkGroupSize = 0;
                errNum = clGetKernelWorkGroupInfo( kernel, m_devices[0], CL_KERNEL_WORK_GROUP_SIZE,
                sizeof(size_t), &maxWorkGroupSize, NULL );
                if (errNum != CL_SUCCESS )
                {
                    FrmLogMessage( "ERROR: clGetKernelInfo on CL_KERNEL_WORK_GROUP_SIZE\n");
                    return FALSE;
                }
                FrmSprintf( str, sizeof(str), "Max workgroup size: %d\n", maxWorkGroupSize );
                FrmLogMessage( str );

                size_t localSize = maxWorkGroupSize;
                if ( localSize > (size_t)m_nMatrixCols )
                {
                    localSize = m_nMatrixCols;
                }

                // Compute the next global size that is a multiple of the local size
                size_t globalSize;
                size_t remndr = m_nMatrixRows % localSize;
                if( remndr == 0 )
                    globalSize = m_nMatrixRows;
                else
                    globalSize = m_nMatrixRows + localSize - remndr;

                m_Timer.Reset();
                m_Timer.Start();

                // Queue the kernel for execution
                errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                    &globalSize, &localSize, 0, NULL, NULL );
                if( errNum != CL_SUCCESS )
                {
                    FrmSprintf( str, sizeof(str), "Error queueing kernel for execution (%d).\n", errNum );
                    FrmLogMessage( str );
                    return FALSE;
                }
            }
            break;
        case MATRIX_ADD_OPTIMIZED2:
            {
                FrmSprintf( str, sizeof(str), "---- Running Optimized2 Matrix Addition ---\n");
                FrmLogMessage( str );

                // Use a 2D global work size where each work-item computes the sum of 4 floats.
                size_t globalSize[2] = {m_nMatrixCols/4, m_nMatrixRows};
                // Tune local work size automatically or manually and choose the one that gives best performance
                // The value {64, 1} gave the best performance for this case
                // Other sizes tried were {128, 1}, {128, 2}, {64, 2}
                size_t localSize[2] = {64, 1};

                m_Timer.Reset();
                m_Timer.Start();

                // Queue the kernel for execution
                errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 2, NULL,
                                    globalSize, localSize, 0, NULL, NULL );
                if( errNum != CL_SUCCESS )
                {
                    FrmSprintf( str, sizeof(str), "Error queueing kernel for execution (%d).\n", errNum );
                    FrmLogMessage( str );
                    return FALSE;
                }
            }
            break;
        }

        clFinish( m_commandQueue );
        m_Timer.Stop();

        // Read the result back to host memory
        pMatrixSum = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_matrixSum, CL_TRUE, CL_MAP_READ, 0,
                                                  sizeof(FLOAT32) * m_nMatrixRows * m_nMatrixCols, 0, NULL, NULL, &errNum );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error enqueuing map buffer." );
            return FALSE;
        }

        FrmSprintf( str, sizeof(str), "Results: Addition of '%d x %d' and '%d x %d' matrix in '%.6f' ms.\n",
            m_nMatrixRows, m_nMatrixCols, m_nMatrixRows, m_nMatrixCols, m_Timer.GetTime()*1000);
        FrmLogMessage( str );


        BOOL result = TRUE;
        if( RunTests() )
        {
            const FLOAT32 epsilon = 0.1f;

            for( cl_int i = 0; i < m_nMatrixRows; i++ )
            {
                for( cl_int j = 0; j < m_nMatrixCols; j++ )
                {
                    FLOAT32 refVal = m_pRefResult[ i * m_nMatrixCols + j ];
                    FLOAT32 val = pMatrixSum[ i * m_nMatrixCols + j ];

                    if( FrmAbs( refVal - val ) > epsilon )
                    {
                        FrmSprintf( str, sizeof(str), "Reference test failure, ref = (%f), result = (%f) Diff = (%f).\n", refVal, val, FrmAbs(refVal - val));
                        FrmLogMessage( str );
                        result = FALSE;
                    }
                }
            }
        }

        errNum = clEnqueueUnmapMemObject( m_commandQueue, m_matrixSum, pMatrixSum, 0, NULL, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error unmapping mem object" );
            return FALSE;
        }

        if( result == FALSE )
            return result;
    }

    return TRUE;
}

