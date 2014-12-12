//--------------------------------------------------------------------------------------
// File: ParallelPrefixSum.cl
// Desc: Parallel Prefix Sum kernels
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Name: PrefixSumSmallPowOf2()
// Desc: Compute the exclusive prefix sum (scan) of the input array.  This 
// implementation only works for small power-of-2 sized arrays where the
// maximum supported size is 2 * CL_KERNEL_WORK_GROUP_SIZE.  This algorithm
// is based on "Parallel Prefix Sum (Scan) with CUDA" by Mark Harris, 
// Shubhabrata Sengupta and John D. Owens in GPU Gems 3.
//--------------------------------------------------------------------------------------
__kernel void PrefixSumSmallPowOf2(const int arraySize,
                                   __global int* pInArray,
                                   __global int* pOutArray,
                                   __local int* pCache )
{
    int i = get_local_id(0);
    int offset = 1;

    pCache[ 2 * i ] = pInArray[ 2 * i ];
    pCache[ 2 * i + 1 ] = pInArray[ 2 * i + 1 ];

    // Perform the up-sweep phase
    for( int d = arraySize / 2; d > 0; d /= 2 )
    {
        // Make sure all the work-items have up-to-date values for pCache
        barrier( CLK_LOCAL_MEM_FENCE );
        if ( i < d )
        {
            pCache[ offset * ( 2 * i + 2 ) - 1 ] += pCache[ offset * ( 2 * i + 1 ) - 1 ];
        }

        offset *= 2;
    }

    // The first thread is responsible for setting the last element
    if ( i == 0 )
    {
        pCache[ arraySize - 1 ] = 0;
    }

    // Perform the down-sweep phase
    for( int d = 1; d < arraySize; d *= 2 )
    {
        offset /= 2;
        
        // Make sure all the work-items have up-to-date values for pCache
        barrier( CLK_LOCAL_MEM_FENCE );

        if ( i < d )
        {
            int ai = offset * ( 2 * i + 1 ) - 1;
            int bi = offset * ( 2 * i + 2 ) - 1; 

            int temp = pCache[ ai ]; 
            pCache[ ai ] = pCache[ bi ];
            pCache[ bi ] += temp;
        }
    }

    // Make sure all the work-items have up-to-date values for pCache
    barrier( CLK_LOCAL_MEM_FENCE );

    // Finally, write the results
    pOutArray[ 2 * i ] = pCache[ 2 * i ];
    pOutArray[ 2 * i + 1 ] = pCache[ 2 * i + 1 ];
}



//--------------------------------------------------------------------------------------
// Name: BlockPrefixSumPow2()
// Desc: Identical to PrefixSumSmallPowOf2, except that it writes the sum of the
// block to pSum.  This function is used by PrefixSumLargePowOf2 kernel for 
// computing the sum inside blocks.
//--------------------------------------------------------------------------------------
__kernel void BlockPrefixSumPow2(const int arraySize,
                                 __global int* pInArray,
                                 __global int* pOutArray,
                                 __global int* pSum,
                                 __local int* pCache )
{
    int i = get_local_id(0);
    int offset = 1;

    pCache[ 2 * i ] = pInArray[ 2 * i ];
    pCache[ 2 * i + 1 ] = pInArray[ 2 * i + 1 ];

    // Perform the up-sweep phase
    for( int d = arraySize / 2; d > 0; d /= 2 )
    {
        // Make sure all the work-items have up-to-date values for pCache
        barrier( CLK_LOCAL_MEM_FENCE );
        if ( i < d )
        {
            pCache[ offset * ( 2 * i + 2 ) - 1 ] += pCache[ offset * ( 2 * i + 1 ) - 1 ];
        }

        offset *= 2;
    }

    barrier( CLK_LOCAL_MEM_FENCE );

    // The first thread is responsible for setting the last element
    if ( i == 0 )
    {
        // Store the sum of the block to the sums array before zero'ing it out
        *pSum = pCache[ arraySize - 1 ];
        pCache[ arraySize - 1 ] = 0;
    }

    // Perform the down-sweep phase
    for( int d = 1; d < arraySize; d *= 2 )
    {
        offset /= 2;
        
        // Make sure all the work-items have up-to-date values for pCache
        barrier( CLK_LOCAL_MEM_FENCE );

        if ( i < d )
        {
            int ai = offset * ( 2 * i + 1 ) - 1;
            int bi = offset * ( 2 * i + 2 ) - 1; 

            int temp = pCache[ ai ]; 
            pCache[ ai ] = pCache[ bi ];
            pCache[ bi ] += temp;
        }
    }

    // Make sure all the work-items have up-to-date values for pCache
    barrier( CLK_LOCAL_MEM_FENCE );

    // Finally, write the results
    pOutArray[ 2 * i ] = pCache[ 2 * i ];
    pOutArray[ 2 * i + 1 ] = pCache[ 2 * i + 1 ];
}

//--------------------------------------------------------------------------------------
// Name: PrefixSumLargePowOf2()
// Desc: Compute the exclusive prefix sum (scan) of *blocks* of the input array.  This
// is the first step in computing the sum of the large array.  It stores the sums of each
// block in pSums and the prefix sums of each block in pOutArray.
//--------------------------------------------------------------------------------------
__kernel void PrefixSumLargePowOf2(const int arraySize,
                                    const int blockSize,
                                    __global int* pInArray,
                                    __global int* pOutArray,
                                    __global int* pSums,
                                    __local int* pCache )
{
    __global int* pInBlock = pInArray + get_group_id( 0 ) * (blockSize * 2);
    __global int* pOutBlock = pOutArray + get_group_id( 0 ) * (blockSize * 2);
    __global int* pBlockSum = pSums + get_group_id( 0 );

    // Perform the parallel prefix sum on this block, having thread 0 store
    // the sum of the blocks.
    BlockPrefixSumPow2( blockSize * 2, pInBlock, pOutBlock, pBlockSum, pCache );
}

//--------------------------------------------------------------------------------------
// Name: AddIncrToBlocksPowOf2()
// Desc: The final step in the large prefix sum adds the sums of the preceding blocks
// to each element in the current block.  The final result in pOutArray is the result
// of performing the full parallel prefix sum on the entire input array.
//--------------------------------------------------------------------------------------
__kernel void AddIncrToBlocksPowOf2(__global int* pIncrArray, 
                                    __global int* pOutArray)
{
    // Add all block sums i to scanned block i+1
    if ( get_group_id(0) > 0 )
    {
        __global int* pIncr = pIncrArray + ( get_group_id( 0 ) / 2);
        pOutArray[ get_global_id(0) ] += *pIncr;
    }
}
