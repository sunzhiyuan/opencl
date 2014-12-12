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

    // Reference results
    FLOAT32* m_pRefResults;

    size_t m_bufWidth;
    size_t m_bufHeight;
};


#endif // COMPUTE_H
