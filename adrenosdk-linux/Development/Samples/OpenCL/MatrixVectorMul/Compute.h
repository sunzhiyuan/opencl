//--------------------------------------------------------------------------------------
// File: Compute.h
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2009 QUALCOMM Incorporated.
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

    // Vector (device)
    cl_mem m_vector;

    // Result Vector (device)
    cl_mem m_resultVector;

    // Reference results (for RunTests())
    FLOAT32* m_pRefResults;
};


#endif // COMPUTE_H
