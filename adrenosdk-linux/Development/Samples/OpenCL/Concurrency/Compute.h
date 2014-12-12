//--------------------------------------------------------------------------------------
// File: Compute.h
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef COMPUTE_H
#define COMPUTE_H


//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: This sample shows how to use CPU and GPU concurrently for maxtrix multiplication.
//       It is meant to demonstrate workload distribution among multiple devices.
//       For matrix multiplication optimization, please see MatrixMatrixMul sample.
//--------------------------------------------------------------------------------------
class CSample : public CFrmComputeApplication
{
public:
    virtual BOOL Initialize();
    virtual VOID Destroy();
    virtual BOOL Compute();

    CSample( const CHAR* strName );
private:

    VOID TestMul( const FLOAT32* pMatrixA, const FLOAT32* pMatrixB );
    VOID PrintMatrix( INT32 rows, INT32 cols, const FLOAT32* pMatrix );

private:

    // Timer for timing performance
    CFrmTimer m_Timer;

    // Size of matrix A in rows
    cl_int m_nMatrixRowsA;

    // Size of matrix A in cols and B in rows
    cl_int m_nMatrixColsARowsB;

    // Size of matrix B in cols
    cl_int m_nMatrixColsB;

    // OpenCL command queues, one per device
    cl_command_queue* m_commandQueues;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernel
    cl_kernel m_kernel;

    // Matrix A (device)
    cl_mem m_matrixA;

    // Matrix B (device)
    cl_mem m_matrixB;

    // Matrix product result (device)
    cl_mem m_matrixProduct;

    // Sub-buffers for each device to hold
    cl_mem* m_matrixProductSubBuffers;

    // Reference result for testing
    FLOAT32* m_pRefResult;
};


#endif // COMPUTE_H
