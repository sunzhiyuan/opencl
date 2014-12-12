//--------------------------------------------------------------------------------------
// File: Compute.h
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef COMPUTE_H
#define COMPUTE_H

typedef enum
{
    RECURSIVE_GAUSSIAN_HORIZONTAL = 0,
    RECURSIVE_GAUSSIAN_VERTICAL,
    NUM_KERNELS

} Kernels;

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

    VOID RefGaussKernelHorizontal( FLOAT32* pBGRAImage );
    VOID RefGaussKernelVertical( FLOAT32* pBGRAImage );


private:

    // Parameters to the gaussian kernel.  These are described in the paper
    // "Recursive Implementation of the Gaussian Filter" by Ian T. Young and Lucas J. van Vilet
    typedef struct
    {
        FLOAT32 b0;
        FLOAT32 b1;
        FLOAT32 b2;
        FLOAT32 b3;
        FLOAT32 b4;
    } GaussParams;

    // Timer for timing performance
    CFrmTimer m_Timer;

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernel
    cl_kernel m_kernels[NUM_KERNELS];

    // Source image
    cl_mem m_sourceImageRGBA;

    // Intermediate image (horizontal processed)
    cl_mem m_intermediateImage;

    // Destination image
    cl_mem m_destImage;

    // Sampler to use on source image
    cl_sampler m_sampler;

    // Image width
    cl_int m_nImgWidth;

    // Image height
    cl_int m_nImgHeight;

    // Reference results (for RunTests())
    BYTE* m_pRefResults;

    // Paramaters for the gaussian kernel
    GaussParams m_gaussParams;
};


#endif // COMPUTE_H
