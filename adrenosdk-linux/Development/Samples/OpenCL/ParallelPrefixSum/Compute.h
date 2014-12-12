//--------------------------------------------------------------------------------------
// File: Compute.h
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef COMPUTE_H
#define COMPUTE_H

typedef enum
{
    // Kernel for computing small pow-of-2 based prefix sum
    // in a single workgroup
    PREFIX_SUM_SMALL_POW_OF_2_512_LOCAL = 0,
    PREFIX_SUM_SMALL_POW_OF_2_512_GLOBAL = 1,

    // The following kernels are all involved in computing a large
    // prefix sum across a number of workgroups on pow-of-2 based
    // arrays
    PREFIX_SUM_LARGE_POW_OF_2_LOCAL = 2,

    PREFIX_SUM_LARGE_POW_OF_2_GLOBAL = 3,

    NUM_PREFIX_SUM_ALGS = 4,

    PREFIX_SUM_SMALL_POW_OF_2_256_LOCAL = 4,
    PREFIX_SUM_SMALL_POW_OF_2_256_GLOBAL = 5,
    // Helper kernel for large power-of-2 based arrays
    PREFIX_SUM_ADD_INCR_TO_BLOCKS_POW_OF_2 = 6,

    NUM_PREFIX_SUM_KERNELS = 7
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

    VOID PrefixSumRef( INT32* pRefResult, INT32* pArray, INT32 nArraySize );

private:

    // Timer for timing performance
    CFrmTimer m_Timer;
    CFrmTimer m_Timer_2;

    // Number of elements in the input array
    cl_int m_nArraySize[NUM_PREFIX_SUM_ALGS];


    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernels
    cl_kernel m_kernels[NUM_PREFIX_SUM_KERNELS];

    // Input array (device)
    cl_mem m_array[NUM_PREFIX_SUM_ALGS];

    // Result array (device)
    cl_mem m_result[NUM_PREFIX_SUM_ALGS];

    // Reference result for testing
    INT32* m_pRefResult[NUM_PREFIX_SUM_ALGS];
};

#endif // COMPUTE_H
