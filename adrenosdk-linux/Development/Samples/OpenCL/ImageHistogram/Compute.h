//--------------------------------------------------------------------------------------
// File: Compute.h
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2014 QUALCOMM Incorporated.
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

    enum OperationType
    {
        HISTOGRAM_GLOBAL_ONLY,
        HISTOGRAM_LOCAL_SHARED_GLOBAL
    };
private:

    VOID    ComputeRefResults   ( BYTE* pRGBAImage );

    VOID    EqualizeColors      (int* colorStats, int* pCumu);

    VOID    ReduceColors        (int* colorStats, int* pColorMap, int divided_level);
    VOID    PickColors          (int* colorStats, int start, int end, int *picked, int* numPicked, int level);
    int     PickMedian          (int* colorStats, int start, int end);

    BOOL    ApplyUseCase        (int* colorStats);

private:

    OperationType m_opType;

    // Timer for timing performance
    CFrmTimer m_Timer;

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernel
    cl_kernel m_kernelHistogram;
    cl_kernel m_kernelHistogramLocal;
    cl_kernel m_kernelColorMapping;

    cl_kernel curKernel;

    // Source image
    cl_mem m_sourceImage;

    // Output images
    cl_mem m_grayscaleImage;
    cl_mem m_equalizedImage;
    cl_mem m_colorReducedImage;

    // intermediate buffers
    cl_mem m_stats;
    cl_mem m_groupStats;
    cl_mem m_cumulative;
    cl_mem m_colormap;

    // Image width
    cl_int m_nImgWidth;

    // Image height
    cl_int m_nImgHeight;

    unsigned int m_refStats[256];

    char str[256];
    int bufsize;
    int mapsize;

};


#endif // COMPUTE_H
