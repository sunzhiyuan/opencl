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
    return new CSample( "MatrixMatrixMul" );
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
    m_matrixProduct = 0;

    m_nMatrixRowsA = 512;
    m_nMatrixColsARowsB = 512; // If this changes, modify MATRIX_COLS_A_ROWS_B in MatrixMatrixMul.cl
    m_nMatrixColsB = 512;

    m_pRefResult = NULL;


    for( int i = 0; i < NUM_KERNELS; i++ )
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

    // Create the command queue
    m_commandQueue = clCreateCommandQueue( m_context, m_devices[0], CL_QUEUE_PROFILING_ENABLE, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to create command queue" );
        return FALSE;
    }

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/MatrixMatrixMul.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }

    const CHAR* kernelNames[NUM_KERNELS] =
    {
        "MatrixMatrixMulSimple",
        "MatrixMatrixMulOptimized",
        "MatrixTranspose"
    };
    for( int i = 0; i < NUM_KERNELS; i++)
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

    size_t nNumFloatsA = m_nMatrixRowsA * m_nMatrixColsARowsB;
    size_t nNumFloatsB = m_nMatrixColsARowsB * m_nMatrixColsB;

    // Create device buffers
    m_matrixA = clCreateBuffer( m_context, CL_MEM_READ_ONLY, nNumFloatsA * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device matrix A.\n" );
        return FALSE;
    }

    m_matrixB = clCreateBuffer( m_context, CL_MEM_READ_ONLY, nNumFloatsB * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device matrix B.\n" );
        return FALSE;
    }

    // Map A for writing
    FLOAT32 *pMatrixA = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_matrixA, CL_TRUE, CL_MAP_WRITE, 0, nNumFloatsA * sizeof(FLOAT32),
        0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: mapping buffer for writing.\n" );
        return FALSE;
    }

    // Fill A with data
    for( cl_int row = 0; row < m_nMatrixRowsA; row++ )
    {
        for( cl_int col = 0; col < m_nMatrixColsARowsB; col++ )
        {
            FLOAT32 val = (FLOAT32)col / (FLOAT32)m_nMatrixColsARowsB + (FLOAT32)row / (FLOAT32)m_nMatrixRowsA;
            pMatrixA[ row * m_nMatrixColsARowsB + col ] = val;
        }
    }

    // Map B for writing
    FLOAT32 *pMatrixB = (FLOAT32*)clEnqueueMapBuffer( m_commandQueue, m_matrixB, CL_TRUE, CL_MAP_WRITE, 0, nNumFloatsB * sizeof(FLOAT32),
        0, NULL, NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: mapping buffer for writing.\n" );
        return FALSE;
    }

    // Fill B with data
    for( cl_int row = 0; row < m_nMatrixColsARowsB; row++ )
    {
        for( cl_int col = 0; col < m_nMatrixColsB; col++ )
        {
            FLOAT32 val = 2.0f - (FLOAT32)col / (FLOAT32)m_nMatrixColsB + (FLOAT32)row / (FLOAT32)m_nMatrixColsARowsB;
            pMatrixB[ row * m_nMatrixColsB + col ] = val;
        }
    }

    // Compute reference results if testing
    if ( RunTests() )
    {
        FrmLogMessage( "Computing reference results on CPU, this may take a while...\n");
        TestMul( pMatrixA, pMatrixB );
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

    // Create result matrix product
    m_matrixProduct = clCreateBuffer( m_context, CL_MEM_READ_WRITE, m_nMatrixRowsA * m_nMatrixColsB * sizeof(FLOAT32), NULL, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: allocation of device result matrix C.\n" );
        return FALSE;
    }

    m_matrixTranspose = clCreateBuffer( m_context, CL_MEM_READ_WRITE, m_nMatrixColsARowsB * m_nMatrixColsB * sizeof(FLOAT32), NULL, &errNum );
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

    for( int i = 0; i < NUM_KERNELS; i++ )
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

    if( m_matrixProduct != 0 )
    {
        clReleaseMemObject( m_matrixProduct );
        m_matrixProduct = 0;
    }

    delete [] m_pRefResult;
}

//--------------------------------------------------------------------------------------
// Name: TestMul()
// Desc: Compute reference CPU multiplication of matrices
//--------------------------------------------------------------------------------------
VOID CSample::TestMul( const FLOAT32* pMatrixA, const FLOAT32* pMatrixB )
{
    m_pRefResult = new FLOAT32[ m_nMatrixRowsA * m_nMatrixColsB ];

    for( INT32 i = 0; i < m_nMatrixRowsA; i++ )
    {
        for( INT32 j = 0; j < m_nMatrixColsB; j++ )
        {
            FLOAT32 result = 0.0f;

            for( INT32 k = 0; k < m_nMatrixColsARowsB; k++ )
            {
                result += pMatrixA[ i * m_nMatrixColsARowsB + k ] * pMatrixB[ k * m_nMatrixColsB + j];
            }

            m_pRefResult[i * m_nMatrixColsB + j] = result;
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

    for( int i = 0; i < NUM_MATRIX_MUL_ALGS; i++ )
    {
        errNum = clGetDeviceInfo(m_devices[0], CL_DEVICE_TYPE, sizeof(devType), &devType, NULL);
        if ( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Failed to obtain device type" );
            return FALSE;
        }

        /*
         * CPU version only works on simple mode. There is issue with work group size with the optimized version for CPU.
         */
        if (devType == CL_DEVICE_TYPE_CPU && i != MATRIX_MUL_SIMPLE)
        {
            continue;
        }

        cl_kernel kernel = m_kernels[i];

        // Set the kernel arguments
        errNum = clSetKernelArg( kernel, 0, sizeof(int), &m_nMatrixRowsA);
        errNum |= clSetKernelArg( kernel, 1, sizeof(int), &m_nMatrixColsARowsB );
        errNum |= clSetKernelArg( kernel, 2, sizeof(int), &m_nMatrixColsB );
        errNum |= clSetKernelArg( kernel, 3, sizeof(cl_mem), &m_matrixA );
        errNum |= clSetKernelArg( kernel, 5, sizeof(cl_mem), &m_matrixProduct);

        // memset matrixProduct to 0 as it gets reused for both simple and optimized algorithms
        FLOAT32* pMatrixC = NULL;
        pMatrixC = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_matrixProduct, CL_TRUE, CL_MAP_WRITE, 0,
                                                  sizeof(FLOAT32) * m_nMatrixRowsA * m_nMatrixColsB, 0, NULL, NULL, &errNum );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error enqueuing map buffer." );
            return FALSE;
        }

        memset(pMatrixC, 0, sizeof(FLOAT32) * m_nMatrixRowsA * m_nMatrixColsB);

        errNum = clEnqueueUnmapMemObject( m_commandQueue, m_matrixProduct, pMatrixC, 0, NULL, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error unmapping mem object" );
            return FALSE;
        }

        switch (i)
        {
        case MATRIX_MUL_SIMPLE:

            {
                FrmSprintf( str, sizeof(str), "--- Running Simple Matrix Multiplication ---\n");
                FrmLogMessage( str );

                errNum |= clSetKernelArg( kernel, 4, sizeof(cl_mem), &m_matrixB );
                if( errNum != CL_SUCCESS )
                {
                    FrmLogMessage( "Error setting kernel arguments.\n" );
                    return FALSE;
                }
                // Use a 2D global work size where each work-item computes an element of C
                size_t globalWorkSize[2] = { m_nMatrixRowsA, m_nMatrixColsB };

                m_Timer.Reset();
                m_Timer.Start();

                // Queue the kernel for execution
                errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 2, NULL,
                                    globalWorkSize, NULL, 0, NULL, NULL );

            }
            break;
        case MATRIX_MUL_OPTIMIZED:

            {
                FrmSprintf( str, sizeof(str), "--- Running Optimized Matrix Multiplication ---\n");
                FrmLogMessage( str );

                errNum |= clSetKernelArg( kernel, 4, sizeof(cl_mem), &m_matrixTranspose );
                errNum |= clSetKernelArg( kernel, 6, sizeof(cl_float) * m_nMatrixColsARowsB, NULL);
                if( errNum != CL_SUCCESS )
                {
                    FrmLogMessage( "Error setting kernel arguments.\n" );
                    return FALSE;
                }

                size_t globalSize = m_nMatrixColsARowsB;
                errNum = clSetKernelArg( m_kernels[NUM_KERNELS-1], 0, sizeof(int), &m_nMatrixColsARowsB);
                errNum |= clSetKernelArg( m_kernels[NUM_KERNELS-1], 1, sizeof(int), &m_nMatrixColsB );
                errNum |= clSetKernelArg( m_kernels[NUM_KERNELS-1], 2, sizeof(cl_mem), &m_matrixB );
                errNum |= clSetKernelArg( m_kernels[NUM_KERNELS-1], 3, sizeof(cl_mem), &m_matrixTranspose );
                if( errNum != CL_SUCCESS )
                {
                    FrmLogMessage( "Error setting kernel arguments.\n" );
                    return FALSE;
                }
                m_Timer.Reset();
                m_Timer.Start();

                // Compute transpose of Matrix B
                errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernels[NUM_KERNELS-1], 1, NULL,
                                    &globalSize, NULL, 0, NULL, NULL );
                if( errNum != CL_SUCCESS )
                {
                    FrmSprintf( str, sizeof(str), "Error queueing transpose kernel for execution (%d).\n", errNum );
                    FrmLogMessage( str );
                    return FALSE;
                }
                clFinish( m_commandQueue );
                m_Timer.Stop();

                FrmSprintf( str, sizeof(str), "Transposed '%d x %d' matrix in '%.6f' ms.\n",
                     m_nMatrixColsARowsB, m_nMatrixColsB, m_Timer.GetTime()*1000 );
                FrmLogMessage( str );

                // Use a 1D global work size where each work-item computes a row of C.
                // Tune local work size automatically or manually and choose the one that gives best performance
                // The below value gave best performance for this case. Other sizes tried 64, 256, 512.
                size_t localSize = 128;
                if ( localSize > (size_t)m_nMatrixColsB )
                {
                    localSize = m_nMatrixColsB;
                }

                size_t remndr = m_nMatrixRowsA % localSize;
                if( remndr == 0 )
                    globalSize = m_nMatrixRowsA;
                else
                    globalSize = m_nMatrixRowsA + localSize - remndr;

                m_Timer.Reset();
                m_Timer.Start();

                // Queue the kernel for execution
                errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                    &globalSize, &localSize, 0, NULL, NULL );
            }
            break;
        }
        if( errNum != CL_SUCCESS )
        {
            FrmSprintf( str, sizeof(str), "Error queueing kernel for execution (%d).\n", errNum );
            FrmLogMessage( str );
            return FALSE;
        }
        clFinish( m_commandQueue );
        m_Timer.Stop();

        // Read the result back to host memory
        pMatrixC = (FLOAT32*) clEnqueueMapBuffer( m_commandQueue, m_matrixProduct, CL_TRUE, CL_MAP_READ, 0,
                                                  sizeof(FLOAT32) * m_nMatrixRowsA * m_nMatrixColsB, 0, NULL, NULL, &errNum );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error enqueuing map buffer." );
            return FALSE;
        }

        FrmSprintf( str, sizeof(str), "Results: multiplication of '%d x %d' and '%d x %d' matrix in '%.6f' ms.\n",
            m_nMatrixRowsA, m_nMatrixColsARowsB, m_nMatrixColsARowsB, m_nMatrixColsB, m_Timer.GetTime()*1000);
        FrmLogMessage( str );


        BOOL result = TRUE;
        if( RunTests() )
        {
            const FLOAT32 epsilon = 0.1f;

            for( cl_int i = 0; i < m_nMatrixRowsA; i++ )
            {
                for( cl_int j = 0; j < m_nMatrixColsB; j++ )
                {
                    FLOAT32 refVal = m_pRefResult[ i * m_nMatrixColsB + j ];
                    FLOAT32 val = pMatrixC[ i * m_nMatrixColsB + j ];

                    if( FrmAbs( refVal - val ) > epsilon )
                    {
                        FrmSprintf( str, sizeof(str), "Reference test failure, ref = (%f), result = (%f) Diff = (%f).\n", refVal, val, FrmAbs(refVal - val));
                        FrmLogMessage( str );
                        result = FALSE;
                    }
                }
            }
        }

        errNum = clEnqueueUnmapMemObject( m_commandQueue, m_matrixProduct, pMatrixC, 0, NULL, NULL );
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
