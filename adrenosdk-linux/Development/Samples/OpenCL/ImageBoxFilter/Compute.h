//--------------------------------------------------------------------------------------
// File: Compute.h
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2009,2014 QUALCOMM Incorporated.
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

    VOID ComputeRefResults( BYTE* pRGBAImage );

private:

    // Timer for timing performance
    CFrmTimer m_Timer;

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernel
    cl_kernel m_kernel;
    cl_kernel m_kernel2;

    // Source image
    cl_mem m_sourceImage;

    // Destination image
    cl_mem m_destImage;

    // intermediate image
    cl_mem m_bufImage;

    // Sampler to use on source image
    cl_sampler m_sampler;

    // Image width
    cl_int m_nImgWidth;

    // Image height
    cl_int m_nImgHeight;

    // Reference results (for RunTests())
    BYTE* m_pRefResults;
};


#endif // COMPUTE_H
