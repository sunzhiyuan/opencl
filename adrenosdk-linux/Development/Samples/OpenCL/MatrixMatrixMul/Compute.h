//--------------------------------------------------------------------------------------
// File: Compute.h
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef COMPUTE_H
#define COMPUTE_H

#define NUM_KERNELS 3

typedef enum
{
    MATRIX_MUL_SIMPLE,
    MATRIX_MUL_OPTIMIZED,
    NUM_MATRIX_MUL_ALGS
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

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernels
    cl_kernel m_kernels[NUM_KERNELS];

    // Matrix A (device)
    cl_mem m_matrixA;

    // Matrix B (device)
    cl_mem m_matrixB;

    // Transpose of Matrix B (device)
    cl_mem m_matrixTranspose;

    // Matrix product result (device)
    cl_mem m_matrixProduct;

    // Reference result for testing
    FLOAT32* m_pRefResult;
};


#endif // COMPUTE_H
