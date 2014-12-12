//--------------------------------------------------------------------------------------
// File: Compute.h
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef COMPUTE_H
#define COMPUTE_H

typedef enum
{
    MATRIX_ADD_SIMPLE,
    MATRIX_ADD_OPTIMIZED1,
    MATRIX_ADD_OPTIMIZED2,
    NUM_MATRIX_ADD_ALGS
};

//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmComputeApplication
{
public:
    virtual BOOL Initialize();
    virtual VOID Destroy();
    virtual BOOL Compute();

    CSample( const CHAR* strName );
private:

    VOID TestAdd( const FLOAT32* pMatrixA, const FLOAT32* pMatrixB );
    VOID PrintMatrix( INT32 rows, INT32 cols, const FLOAT32* pMatrix );

private:

    // Timer for timing performance
    CFrmTimer m_Timer;

    // Size of matrix in rows
    cl_int m_nMatrixRows;
    cl_int m_nMatrixCols;

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernels
    cl_kernel m_kernels[NUM_MATRIX_ADD_ALGS];

    // Matrix A (device)
    cl_mem m_matrixA;

    // Matrix B (device)
    cl_mem m_matrixB;

    // Matrix product result (device)
    cl_mem m_matrixSum;

    // Reference result for testing
    FLOAT32* m_pRefResult;
};


#endif // COMPUTE_H
