//--------------------------------------------------------------------------------------
// File: Compute.h
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2014 QUALCOMM Technologies, Inc.
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

    BOOL CompareToRef( INT32 nWidth, INT32 nHeight, FFTComplex* pRefData, FFTComplex* pData,
                       FLOAT32 epsilon );

private:

    // Timer for timing performance
    CFrmTimer m_Timer;

    // X dimension of input array
    cl_int m_nWidth;

    // Y dimension of input array
    cl_int m_nHeight;

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // FFT2D object performing forward/invert FFT with OpenCL and Reference CPU
    CFFT2D* m_pFFT2D;

    // Input to 1D FFT
    FFTComplex* m_pInputArray;

    // Forward 1D FFT reference
    FFTComplex* m_pForwardRefArray;
};

#endif // COMPUTE_H
