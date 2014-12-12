//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include <FrmComputeApplication.h>
#include <FrmUtils.h>
#include <OpenCL/FrmKernel.h>
#include "Compute.h"

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmComputeApplication* FrmCreateComputeApplicationInstance()
{
    return new CSample( "ParallelPrefixSum" );
}

#define NUM_WG         512
//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmComputeApplication( strName )
{
    m_commandQueue = 0;
    m_program = 0;

    m_nArraySize[ PREFIX_SUM_SMALL_POW_OF_2_512_LOCAL ] = (512*NUM_WG);
    m_nArraySize[ PREFIX_SUM_SMALL_POW_OF_2_512_GLOBAL ] = (512*NUM_WG);
    m_nArraySize[ PREFIX_SUM_LARGE_POW_OF_2_LOCAL ] = 131072;
    m_nArraySize[ PREFIX_SUM_LARGE_POW_OF_2_GLOBAL ] = 131072;

    for( int i = 0; i < NUM_PREFIX_SUM_ALGS; i++ )
    {
        m_pRefResult[i] = NULL;
        m_kernels[i] = 0;
        m_array[i] = 0;
        m_result[i] = 0;
    }
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc:
//--------------------------------------------------------------------------------------
//int * pDebugBuff_input;    //removeme
BOOL CSample::Initialize()
{
    cl_int errNum;

    if(!FrmOpenConsole())
        return FALSE;

    // Create the command queue
    m_commandQueue = clCreateCommandQueue( m_context, m_devices[0], CL_QUEUE_PROFILING_ENABLE, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to create command queue" );
        return FALSE;
    }

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/ParallelPrefixSum.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }


    const CHAR* kernelNames[NUM_PREFIX_SUM_KERNELS] =
    {
        "PrefixSumSmallPowOf2_512_Local",
        "PrefixSumSmallPowOf2_512_Global",
        "PrefixSumLargePowOf2_Local",
        "PrefixSumLargePowOf2_Global",
        "PrefixSumSmallPowOf2_256_Local",
        "PrefixSumSmallPowOf2_256_Global",
        "AddIncrToBlocksPowOf2"
    };
    for( int i = 0; i < NUM_PREFIX_SUM_KERNELS; i++)
    {
        // Create kernel
        m_kernels[i] = clCreateKernel( m_program, kernelNames[i], &errNum );
        if ( errNum != CL_SUCCESS )
        {
            CHAR str[256];
            FrmSprintf( str, sizeof(str), "ERROR: Failed to create kernel '%s' (Error = %d).\n", kernelNames[i], errNum );
            FrmLogMessage( str );
            return FALSE;
        }
    }

    for( int i = 0; i < NUM_PREFIX_SUM_ALGS; i++)
    {

         // Create device buffer
        m_array[i] = clCreateBuffer( m_context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR, m_nArraySize[ i ] * sizeof(INT32), NULL, &errNum );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "ERROR: allocation of device input array.\n" );
            return FALSE;
        }

        // Map to host array
        INT32 *pArray = (INT32*) clEnqueueMapBuffer( m_commandQueue, m_array[i], CL_TRUE, CL_MAP_WRITE, 0, sizeof(INT32) * m_nArraySize[ i ],
                                                     0, NULL, NULL, &errNum );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "ERROR: mapping input array" );
            return FALSE;
        }

        // Fill the array with random numbers
        FrmSrand(0);
        for( cl_int j = 0; j < m_nArraySize[i] ; j++ )
        {
            pArray[j] = (INT32)(FrmRand() * (FLOAT32)RAND_MAX) % 1024;
        }

        // Compute reference results if testing
        if ( RunTests() )
        {
            m_pRefResult[i] = new INT32[ m_nArraySize[i] ];
            if( (i==PREFIX_SUM_SMALL_POW_OF_2_512_LOCAL) || (i==PREFIX_SUM_SMALL_POW_OF_2_512_GLOBAL) )
            {
                for(int iWGCnt = 0; iWGCnt < NUM_WG; ++iWGCnt)
                {
                    int * pInBuff = &pArray[512*iWGCnt];
                    int * pRefRes = m_pRefResult[i] + 512*iWGCnt;
                    PrefixSumRef( pRefRes, pInBuff, m_nArraySize[i]/NUM_WG );
                }
            }
            else
            {
                PrefixSumRef( m_pRefResult[i], pArray, m_nArraySize[i] );
            }
        }

        // Unmap array
        errNum = clEnqueueUnmapMemObject( m_commandQueue, m_array[i], pArray, 0, NULL, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "ERROR: unmapping input array." );
            return FALSE;
        }

        // Create result device buffer
        m_result[i] = clCreateBuffer( m_context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, m_nArraySize[ i ] * sizeof(INT32), NULL, &errNum );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "ERROR: allocation of device result array.\n" );
            return FALSE;
        }
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_commandQueue != 0 )
    {
        clReleaseCommandQueue( m_commandQueue );
        m_commandQueue = 0;
    }

    if( m_program != 0 )
    {
        clReleaseProgram( m_program );
        m_program = 0;
    }

    for( int i = 0; i < NUM_PREFIX_SUM_KERNELS; i++ )
    {
        if( m_kernels[i] != 0 )
        {
            clReleaseKernel( m_kernels[i] );
            m_kernels[i] = 0;
        }
    }
    for( int i = 0; i < NUM_PREFIX_SUM_ALGS; i++ )
    {
        if( m_array[i] != 0 )
        {
            clReleaseMemObject( m_array[i] );
            m_array[i] = 0;
        }

        if( m_result[i] != 0 )
        {
            clReleaseMemObject( m_result[i] );
            m_result[i] = 0;
        }

        delete [] m_pRefResult[ i ];
    }
}

//--------------------------------------------------------------------------------------
// Name: PrefixSumRef()
// Desc: Compute reference CPU prefix sum exclusive (scan)
//--------------------------------------------------------------------------------------
VOID CSample::PrefixSumRef( INT32* pRefResult, INT32* pArray, INT32 nArraySize )
{
    pRefResult[ 0 ] = 0;
    for( INT32 i = 1; i < nArraySize; i++ )
    {
        pRefResult[ i ] = pArray[ i - 1 ] + pRefResult[ i - 1 ];
    }
}


//--------------------------------------------------------------------------------------
// Name: Compute()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Compute()
{
    char str[256];
    cl_int errNum = 0;
    const int iNumIter = 100;

    for( int i = 0; i < NUM_PREFIX_SUM_ALGS; i++ )
    {
        m_Timer.Reset();
        m_Timer.Start();

        cl_kernel kernel = m_kernels[i];

        switch (i)
        {
        case PREFIX_SUM_SMALL_POW_OF_2_512_LOCAL:
            {
                errNum |= clSetKernelArg( kernel, 0, sizeof(cl_mem), &m_array[i] );
                errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &m_result[i] );

                if( errNum != CL_SUCCESS )
                {
                    FrmLogMessage( "Error setting kernel arguments.\n" );
                    return FALSE;
                }

                size_t globalWorkSize[1] = { m_nArraySize[i] / 4 };
                size_t localWorkSize[1] = { globalWorkSize[0] / NUM_WG };

                m_Timer_2.Reset();
                m_Timer_2.Start();

                for(int iCnt=0; iCnt < iNumIter; ++iCnt)
                {
                    // Queue the kernel for execution
                    errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                        globalWorkSize, localWorkSize, 0, NULL, NULL );
                    if( errNum != CL_SUCCESS )
                    {
                        FrmSprintf( str, sizeof(str), "Error queueing 'PrefixSumSmallPowOf2_Opt' kernel for execution (%d).\n", errNum );
                        FrmLogMessage( str );
                        return FALSE;
                    }
                }
                FrmLogMessage( "Executed Parallel Prefix Sum (Small Array) with Local Memory\n");
            }
            break;
        case PREFIX_SUM_SMALL_POW_OF_2_512_GLOBAL:
            {
                errNum |= clSetKernelArg( kernel, 0, sizeof(cl_mem), &m_array[i] );
                errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &m_result[i] );

                if( errNum != CL_SUCCESS )
                {
                    FrmLogMessage( "Error setting kernel arguments.\n" );
                    return FALSE;
                }

                size_t globalWorkSize[1] = { m_nArraySize[i] / 4 };
                size_t localWorkSize[1] = { globalWorkSize[0] / NUM_WG };

                m_Timer_2.Reset();
                m_Timer_2.Start();

                for(int iCnt=0; iCnt < iNumIter; ++iCnt)
                {
                    // Queue the kernel for execution
                    errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                        globalWorkSize, localWorkSize, 0, NULL, NULL );
                    if( errNum != CL_SUCCESS )
                    {
                        FrmSprintf( str, sizeof(str), "Error queueing 'PrefixSumSmallPowOf2_Global' kernel for execution (%d).\n", errNum );
                        FrmLogMessage( str );
                        return FALSE;
                    }
                }
                FrmLogMessage( "Executed Parallel Prefix Sum (Small Array) with Global Memory\n");
            }
            break;
        case PREFIX_SUM_LARGE_POW_OF_2_LOCAL:
            {
                cl_short blockSize = 256;
                cl_int numBlocks = ( m_nArraySize[i] / 2 ) / blockSize;
                // Allocate intermediate device buffers - there are two buffers "sumsArray" and
                // "incrArray".  The "sumsArray" stores the sum of each block and the "incrArray"
                // stores the sum of the "sumsArray".
                cl_mem sumsArray = clCreateBuffer( m_context, CL_MEM_READ_WRITE, numBlocks * sizeof(INT32), NULL, &errNum );
                if( errNum != CL_SUCCESS )
                {
                    FrmLogMessage( "Error allocating sums buffer.\n");
                    return FALSE;
                }

                cl_mem incrArray = clCreateBuffer( m_context, CL_MEM_READ_WRITE, numBlocks * sizeof(INT32), NULL, &errNum );
                if( errNum != CL_SUCCESS )
                {
                    FrmLogMessage( "Error allocating incr buffer.\n");
                    return FALSE;
                }

                m_Timer_2.Reset();
                m_Timer_2.Start();

                for(int iCnt=0; iCnt < iNumIter; ++iCnt)
                {
                    kernel = m_kernels[ PREFIX_SUM_LARGE_POW_OF_2_LOCAL ];
                    size_t globalWorkSize[1] = { m_nArraySize[i] / 4 };
                    size_t localWorkSize[1] = { blockSize / 2 };
                    ////////////////////////////////////////////////////////////////////////////
                    // STEP 1 - Run PrefixSumLargePowOf2() to compute the prefix sum in each
                    // block and also to store the sums of each block in sumsArray
                    ////////////////////////////////////////////////////////////////////////////

                    // Set the kernel arguments for PrefixSumLargePowOf2()
                    errNum = clSetKernelArg( kernel, 0, sizeof(short), &blockSize );
                    errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &m_array[i] );
                    errNum |= clSetKernelArg( kernel, 2, sizeof(cl_mem), &m_result[i] );
                    errNum |= clSetKernelArg( kernel, 3, sizeof(cl_mem), &sumsArray );
                    errNum |= clSetKernelArg( kernel, 4, blockSize * 2 * sizeof(INT32), NULL );
                    if( errNum != CL_SUCCESS )
                    {
                        FrmLogMessage( "Error setting kernel arguments for 'PrefixSumLargePowOf2'.\n" );
                        return FALSE;
                    }

                    // Queue the PrefixSumLargePowOf2 kernel for execution
                    errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                        globalWorkSize, localWorkSize, 0, NULL, NULL );
                    if( errNum != CL_SUCCESS )
                    {
                        FrmSprintf( str, sizeof(str), "Error queueing 'PrefixSumLargePowOf2' kernel for execution (%d).\n", errNum );
                        FrmLogMessage( str );
                        return FALSE;
                    }

                    ////////////////////////////////////////////////////////////////////////////
                    // STEP 2 - Run PrefixSumSmallPowOf2() to compute the prefix sum on the
                    // "sumsArray" and store that result in "incrArray".  The "incrArray" will
                    // then be added to each block to get the final sums in Step 3.
                    ////////////////////////////////////////////////////////////////////////////

                    // Now perform prefix sum on the "Sums" array using PrefixSumSmallPoweOf2()
                    kernel = m_kernels[ PREFIX_SUM_SMALL_POW_OF_2_256_LOCAL ];
                    errNum = clSetKernelArg( kernel, 0, sizeof(cl_mem), &sumsArray );
                    errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &incrArray );

                    if( errNum != CL_SUCCESS )
                    {
                        FrmLogMessage( "Error setting kernel arguments for 'PrefixSumSmallPowOf2'.\n" );
                        return FALSE;
                    }

                    // Compute over the sumsArray which is numBlocks sized
                    globalWorkSize[0] = numBlocks/4;
                    localWorkSize[0] = numBlocks/4;

                    // Queue the PrefixSumSmallPoweOf2 for execution
                    errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                        globalWorkSize, localWorkSize, 0, NULL, NULL );
                    if( errNum != CL_SUCCESS )
                    {
                        FrmSprintf( str, sizeof(str), "Error queueing 'PrefixSumSmallPowOf2_256_Opt' kernel for execution (%d).\n", errNum );
                        FrmLogMessage( str );
                        return FALSE;
                    }

                    ////////////////////////////////////////////////////////////////////////////
                    // STEP 3 - Run AddIncrToBlocksPowOf2() to add the incrArray i to the
                    // values in each element of block i+1.  This leaves the final prefix sum
                    // values in the m_result array.
                    ////////////////////////////////////////////////////////////////////////////

                    // Finally add the "Incr" array to the blocks using AddIncrToBlocksPowOf2()
                    kernel = m_kernels[ PREFIX_SUM_ADD_INCR_TO_BLOCKS_POW_OF_2 ];
                    errNum = clSetKernelArg( kernel, 0, sizeof(cl_mem), &incrArray );
                    errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &m_result[i] );
                    if( errNum != CL_SUCCESS )
                    {
                        FrmLogMessage( "Error setting kernel arguments for 'AddIncrToBlocksPowOf2'.\n" );
                        return FALSE;
                    }

                    // Compute over the whole array
                    globalWorkSize[0] = m_nArraySize[i]/4;
                    localWorkSize[0] = blockSize/4;

                    // Queue the AddIncrToBlocksPowOf2 for execution
                    errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                        globalWorkSize, localWorkSize, 0, NULL, NULL );
                    if( errNum != CL_SUCCESS )
                    {
                        FrmSprintf( str, sizeof(str), "Error queueing 'AddIncrToBlocksPowOf2' kernel for execution (%d).\n", errNum );
                        FrmLogMessage( str );
                        return FALSE;
                    }
                }
                FrmLogMessage( "Executed Parallel Prefix Sum (Large Array) with Local Memory\n");

                // Release temporary memory objects
                clReleaseMemObject( sumsArray );
                clReleaseMemObject( incrArray );
            }
            break;

        case PREFIX_SUM_LARGE_POW_OF_2_GLOBAL:
            {
                cl_short blockSize = 256;
                cl_int numBlocks = ( m_nArraySize[i] / 2 ) / blockSize;
                // Allocate intermediate device buffers - there are two buffers "sumsArray" and
                // "incrArray".  The "sumsArray" stores the sum of each block and the "incrArray"
                // stores the sum of the "sumsArray".
                cl_mem sumsArray = clCreateBuffer( m_context, CL_MEM_READ_WRITE, numBlocks * sizeof(INT32), NULL, &errNum );
                if( errNum != CL_SUCCESS )
                {
                    FrmLogMessage( "Error allocating sums buffer.\n");
                    return FALSE;
                }

                cl_mem incrArray = clCreateBuffer( m_context, CL_MEM_READ_WRITE, numBlocks * sizeof(INT32), NULL, &errNum );
                if( errNum != CL_SUCCESS )
                {
                    FrmLogMessage( "Error allocating incr buffer.\n");
                    return FALSE;
                }

                m_Timer_2.Reset();
                m_Timer_2.Start();

                for(int iCnt=0; iCnt < iNumIter; ++iCnt)
                {
                    kernel = m_kernels[ PREFIX_SUM_LARGE_POW_OF_2_GLOBAL ];
                    size_t globalWorkSize[1] = { m_nArraySize[i] / 4 };
                    size_t localWorkSize[1] = { blockSize / 2 };
                    ////////////////////////////////////////////////////////////////////////////
                    // STEP 1 - Run PrefixSumLargePowOf2() to compute the prefix sum in each
                    // block and also to store the sums of each block in sumsArray
                    ////////////////////////////////////////////////////////////////////////////

                    // Set the kernel arguments for PrefixSumLargePowOf2()
                    errNum = clSetKernelArg( kernel, 0, sizeof(short), &blockSize );
                    errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &m_array[i] );
                    errNum |= clSetKernelArg( kernel, 2, sizeof(cl_mem), &m_result[i] );
                    errNum |= clSetKernelArg( kernel, 3, sizeof(cl_mem), &sumsArray );

                    if( errNum != CL_SUCCESS )
                    {
                        FrmLogMessage( "Error setting kernel arguments for 'PrefixSumLargePowOf2'.\n" );
                        return FALSE;
                    }

                    // Queue the PrefixSumLargePowOf2 kernel for execution
                    errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                        globalWorkSize, localWorkSize, 0, NULL, NULL );
                    if( errNum != CL_SUCCESS )
                    {
                        FrmSprintf( str, sizeof(str), "Error queueing 'PrefixSumLargePowOf2_Global' kernel for execution (%d).\n", errNum );
                        FrmLogMessage( str );
                        return FALSE;
                    }

                    ////////////////////////////////////////////////////////////////////////////
                    // STEP 2 - Run PrefixSumSmallPowOf2() to compute the prefix sum on the
                    // "sumsArray" and store that result in "incrArray".  The "incrArray" will
                    // then be added to each block to get the final sums in Step 3.
                    ////////////////////////////////////////////////////////////////////////////

                    // Now perform prefix sum on the "Sums" array using PrefixSumSmallPoweOf2()
                    kernel = m_kernels[ PREFIX_SUM_SMALL_POW_OF_2_256_GLOBAL ];
                    errNum = clSetKernelArg( kernel, 0, sizeof(cl_mem), &sumsArray );
                    errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &incrArray );

                    if( errNum != CL_SUCCESS )
                    {
                        FrmLogMessage( "Error setting kernel arguments for 'PrefixSumSmallPowOf2'.\n" );
                        return FALSE;
                    }

                    // Compute over the sumsArray which is numBlocks sized
                    globalWorkSize[0] = numBlocks/4;
                    localWorkSize[0] = numBlocks/4;

                    // Queue the PrefixSumSmallPoweOf2 for execution
                    errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                        globalWorkSize, localWorkSize, 0, NULL, NULL );
                    if( errNum != CL_SUCCESS )
                    {
                        FrmSprintf( str, sizeof(str), "Error queueing 'PrefixSumSmallPowOf2_256_Global' kernel for execution (%d).\n", errNum );
                        FrmLogMessage( str );
                        return FALSE;
                    }

                    ////////////////////////////////////////////////////////////////////////////
                    // STEP 3 - Run AddIncrToBlocksPowOf2() to add the incrArray i to the
                    // values in each element of block i+1.  This leaves the final prefix sum
                    // values in the m_result array.
                    ////////////////////////////////////////////////////////////////////////////

                    // Finally add the "Incr" array to the blocks using AddIncrToBlocksPowOf2()
                    kernel = m_kernels[ PREFIX_SUM_ADD_INCR_TO_BLOCKS_POW_OF_2 ];
                    errNum = clSetKernelArg( kernel, 0, sizeof(cl_mem), &incrArray );
                    errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &m_result[i] );
                    if( errNum != CL_SUCCESS )
                    {
                        FrmLogMessage( "Error setting kernel arguments for 'AddIncrToBlocksPowOf2'.\n" );
                        return FALSE;
                    }

                    // Compute over the whole array
                    globalWorkSize[0] = m_nArraySize[i]/4;
                    localWorkSize[0] = blockSize/4;

                    // Queue the AddIncrToBlocksPowOf2 for execution
                    errNum = clEnqueueNDRangeKernel( m_commandQueue, kernel, 1, NULL,
                                        globalWorkSize, localWorkSize, 0, NULL, NULL );
                    if( errNum != CL_SUCCESS )
                    {
                        FrmSprintf( str, sizeof(str), "Error queueing 'AddIncrToBlocksPowOf2' kernel for execution (%d).\n", errNum );
                        FrmLogMessage( str );
                        return FALSE;
                    }
                }

                FrmLogMessage( "Executed Parallel Prefix Sum (Large Array) with Global Memory\n");
                // Release temporary memory objects
                clReleaseMemObject( sumsArray );
                clReleaseMemObject( incrArray );
            }
            break;

        }

        clFinish( m_commandQueue );
        m_Timer.Stop();
        m_Timer_2.Stop();

        FrmLogMessage( str );
        FrmSprintf( str, sizeof(str), "Computed parallel prefix sum on '%d' elements in '%.6f seconds (%d iterations).\n",
            m_nArraySize[i], m_Timer_2.GetTime()/(float)iNumIter, iNumIter );
        FrmLogMessage( str );

        // Read the result back to host memory
        INT32* pResult = (INT32*) clEnqueueMapBuffer( m_commandQueue, m_result[i], CL_TRUE, CL_MAP_READ, 0, sizeof(INT32) * m_nArraySize[i],
                                                      0, NULL, NULL, &errNum );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error enqueuing result buffer map." );
            return FALSE;
        }

        BOOL result = TRUE;

        if( RunTests() )
        {
            int iNumErr = 0;
            for( cl_int j = 0; j < m_nArraySize[i]; j++ )
            {
                INT32 refVal = m_pRefResult[i][j];
                INT32 val = pResult[j];

                if( refVal != val )
                {
                    FrmSprintf( str, sizeof(str), "Reference test failure in element %d, ref = (%d), result = (%d), Diff = (%d).\n", j, refVal, val, refVal - val);
                    FrmLogMessage( str );
                    result = FALSE;
                    ++iNumErr;
                    if(iNumErr > 20)
                        break;
                }
            }
        }

        errNum = clEnqueueUnmapMemObject( m_commandQueue, m_result[i], pResult, 0, NULL, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "ERROR: Unmapping result buffer." );
            return FALSE;
        }

        if( result == FALSE )
            return result;

    }

    return TRUE;
}

