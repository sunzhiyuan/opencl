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

private:

    // Timer for timing performance
    CFrmTimer m_Timer;

    // Size of matrix in rows
    cl_int m_nMatrixRows;

    // Size of matrix in cols
    cl_int m_nMatrixCols;

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernel
    cl_kernel m_kernel;

    // Matrix (device)
    cl_mem m_matrix;

    // Transposed matrix (device)
    cl_mem m_matrixTranspose;

    // Reference results (for RunTests() mode)
    FLOAT32* m_pRefResults;
};


#endif // COMPUTE_H
