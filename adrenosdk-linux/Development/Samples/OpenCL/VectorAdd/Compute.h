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

    // Number of vectors to compute dot product
    size_t m_nNumVectors;

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernel
    cl_kernel m_kernel;

    // Source buffer A
    cl_mem m_srcA;

    // Source buffer B
    cl_mem m_srcB;

    // Destination buffer
    cl_mem m_result;

    // Reference results (for RunTests())
    FRMVECTOR4* m_pRefResults;

};


#endif // COMPUTE_H
