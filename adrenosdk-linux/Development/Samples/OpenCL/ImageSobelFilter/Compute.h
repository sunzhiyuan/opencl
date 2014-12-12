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

typedef enum
{
    RGB_TO_GRAYSCALE = 0,
    SOBEL_FILTER,
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
    BOOL ComputeWorkSizes( cl_kernel kernel, size_t localworkSize[2], size_t globalWorkSize[2] );

private:

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

    // Image converted to grayscale
    cl_mem m_imageGray;

    // Destination image (grayscale)
    cl_mem m_destImage;

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
