//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Incorporated.
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
    return new CSample( "Concurrency" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmComputeApplication( strName )
{
    m_commandQueues = NULL;
    m_program = 0;
    m_matrixA = 0;
    m_matrixB = 0;
    m_matrixProduct = 0;
    m_matrixProductSubBuffers = NULL;
    m_kernel = 0;

    m_nMatrixRowsA = 512;
    m_nMatrixColsARowsB = 512;
    m_nMatrixColsB = 512;

    m_pRefResult = NULL;
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


    // Create a command queue for each device
    m_commandQueues = new cl_command_queue[ m_deviceCount ];
    for( INT32 i = 0; i < m_deviceCount; i++ )
    {
        m_commandQueues[i] = clCreateCommandQueue( m_context, m_devices[i], CL_QUEUE_PROFILING_ENABLE, &errNum );
        if ( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Failed to create command queue" );
            return FALSE;
        }
    }

    // Build the program for all devices
    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/Concurrency.cl", &m_program, m_context,
                                                  m_devices, m_deviceCount, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }

    // Create kernel
    m_kernel = clCreateKernel( m_program, "ConcurrentMatrixMatrixMul", &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: Failed to create kernel 'ConcurrentMatrixMatrixMul'.\n" );
        return FALSE;
    }

    // Allocate device arrys
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
    FLOAT32 *pMatrixA = (FLOAT32*) clEnqueueMapBuffer( m_commandQueues[0], m_matrixA, CL_TRUE, CL_MAP_WRITE, 0, nNumFloatsA * sizeof(FLOAT32),
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
    FLOAT32 *pMatrixB = (FLOAT32*)clEnqueueMapBuffer(  m_commandQueues[0], m_matrixB, CL_TRUE, CL_MAP_WRITE, 0, nNumFloatsB * sizeof(FLOAT32),
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
    if( RunTests() )
    {
        FrmLogMessage( "Computing reference results on CPU, this may take a while...\n");
        TestMul( pMatrixA, pMatrixB );
        FrmLogMessage( "Computed reference results on CPU.\n");
    }

    // Unmap A
    errNum = clEnqueueUnmapMemObject( m_commandQueues[0], m_matrixA, pMatrixA, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "ERROR: unmapping matrixA for writing.\n" );
        return FALSE;
    }

    // Unmap B
    errNum = clEnqueueUnmapMemObject( m_commandQueues[0], m_matrixB, pMatrixB, 0, NULL, NULL );
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

    // Each device computes an equal portion of the matrix.  Create a subbuffer for each region
    m_matrixProductSubBuffers = new cl_mem[ m_deviceCount ];
    size_t rowsPerDevice = m_nMatrixRowsA / m_deviceCount;
    INT32 floatsPerDevice = rowsPerDevice * m_nMatrixColsB;
    for( INT32 i = 0; i < m_deviceCount; i++ )
    {
        cl_buffer_region region;
        region.origin = ( i * floatsPerDevice * sizeof(FLOAT32) );
        region.size = floatsPerDevice * sizeof(FLOAT32);

        m_matrixProductSubBuffers[i] = clCreateSubBuffer( m_matrixProduct, CL_MEM_READ_WRITE,
            CL_BUFFER_CREATE_TYPE_REGION, &region, &errNum );

        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "ERROR: allocation of subbuffer for device.\n" );
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
    for( int i = 0; i < m_deviceCount; i++ )
    {
        if( m_commandQueues != NULL && m_commandQueues[i] != 0 )
        {
            clReleaseCommandQueue( m_commandQueues[i] );
        }

        if( m_matrixProductSubBuffers != NULL && m_matrixProductSubBuffers[i] != 0 )
        {
            clReleaseMemObject( m_matrixProductSubBuffers[i] );
        }
    }

    delete [] m_commandQueues;
    delete [] m_matrixProductSubBuffers;

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

    // Set the kernel arguments
    errNum = clSetKernelArg( m_kernel, 0, sizeof(int), &m_nMatrixRowsA);
    errNum |= clSetKernelArg( m_kernel, 1, sizeof(int), &m_nMatrixColsARowsB );
    errNum |= clSetKernelArg( m_kernel, 2, sizeof(int), &m_nMatrixColsB );
    errNum |= clSetKernelArg( m_kernel, 3, sizeof(cl_mem), &m_matrixA );
    errNum |= clSetKernelArg( m_kernel, 4, sizeof(cl_mem), &m_matrixB );
    errNum |= clSetKernelArg( m_kernel, 5, sizeof(cl_mem), &m_matrixProduct );

    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments.\n" );
        return FALSE;
    }

    // Divide the work up across all devices equally.
    size_t* rowsPerDevice = new size_t[m_deviceCount];
    size_t* maxWorkGroupSize = new size_t[m_deviceCount];
    size_t colsPerDevice = m_nMatrixColsB;

    // The tricky part about using multiple devices to process kernel is workload distribution.
    // We want the device that has greater work group size to take more workload.
    // So the workload can be distributed evenly.
    // However, for image manipulation, it is best not to use CPU due to lack of TP (Texture pipeline) in CPU.
    size_t totalWGSize = 0;
    for ( INT32 i = 0; i < m_deviceCount; i++)
    {
        errNum = clGetKernelWorkGroupInfo( m_kernel, m_devices[i], CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize[i], NULL );
        if (errNum != CL_SUCCESS )
        {
            FrmLogMessage( "ERROR: clGetKernelInfo on CL_KERNEL_WORK_GROUP_SIZE\n");
            return FALSE;
        }
        totalWGSize += maxWorkGroupSize[i];
    }

    // Calculate workload ratio
    float ratio = (float)m_nMatrixRowsA / (float)totalWGSize;
    size_t remain = m_nMatrixRowsA;
    for ( INT32 i = 0; i < m_deviceCount-1; i++)
    {
        rowsPerDevice[i] = ratio * maxWorkGroupSize[i];
        remain -= rowsPerDevice[i];
        FrmSprintf( str, sizeof(str), "Dev %d. Max WG: %d. Global size: %dx%d\n", i, maxWorkGroupSize[i], rowsPerDevice[i], colsPerDevice );
        FrmLogMessage( str );
    }
    // assign the remain items to last device
    rowsPerDevice[m_deviceCount-1] = remain;
    int lastDevice = m_deviceCount-1;
    FrmSprintf( str, sizeof(str), "Dev %d. Max WG: %d. Global size: %dx%d\n", lastDevice, maxWorkGroupSize[lastDevice], rowsPerDevice[lastDevice], colsPerDevice );
    FrmLogMessage( str );

    size_t globalWorkOffset[2] = { 0, 0 };

    m_Timer.Reset();
    m_Timer.Start();

    for ( INT32 i = 0; i < m_deviceCount; i++)
    {

        // Setup the globalWorkSize and globalWorkOffset to partition the work
        // on each device
        size_t globalWorkSize[2] = { rowsPerDevice[i], colsPerDevice };

        // Queue the work for this device on its command queue
        errNum = clEnqueueNDRangeKernel( m_commandQueues[i], m_kernel, 2, globalWorkOffset,
                                    globalWorkSize, NULL, 0, NULL, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmSprintf( str, sizeof(str), "Error queueing kernel for execution (%d).\n", errNum );
            FrmLogMessage( str );
            return FALSE;
        }

        // Setup the global work offset for the next queuring
        globalWorkOffset[0] += rowsPerDevice[i];
    }

    // Finish the work on the devices
    for ( INT32 i = 0; i < m_deviceCount; i++)
    {
        clFinish( m_commandQueues[i] );
    }
    m_Timer.Stop();

    FrmSprintf( str, sizeof(str), "Results: Multiplication of '%d x %d' and '%d x %d' matrix in '%.6f' ms.\n",
        m_nMatrixRowsA, m_nMatrixColsARowsB, m_nMatrixColsARowsB, m_nMatrixColsB, m_Timer.GetTime()*1000.0f );
    FrmLogMessage( str );

    delete [] rowsPerDevice;
    delete [] maxWorkGroupSize;

    // Read the result back to host memory from subbuffers
    FLOAT32* pMatrixC = new FLOAT32[ m_nMatrixRowsA * m_nMatrixColsB ];
    for ( INT32 i = 0; i < m_deviceCount; i++ )
    {
        // Each device computes an equal portion of the rows of the matrix
        size_t rowsPerDevice = m_nMatrixRowsA / m_deviceCount;
        size_t floatsPerDevice = rowsPerDevice * m_nMatrixColsB;

        // Compute location in host matrix to read back from this device (the portion
        // that this device computed)
        FLOAT32 *pResultLoc = pMatrixC + (i * floatsPerDevice);

        // Each subbuffer holds the result for its device, so read that back to the matrix
        errNum = clEnqueueReadBuffer( m_commandQueues[i], m_matrixProductSubBuffers[i], CL_TRUE, 0, sizeof(FLOAT32) * floatsPerDevice,
                pResultLoc, 0, NULL, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error enqueuing buffer read." );
            return FALSE;
        }
    }


    if( RunTests() )
    {
        BOOL result = true;
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

        if ( result == FALSE )
        {
            delete[] pMatrixC;
            return FALSE;
        }
    }

    delete[] pMatrixC;
    return TRUE;
}

