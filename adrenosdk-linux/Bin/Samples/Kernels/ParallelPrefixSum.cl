//--------------------------------------------------------------------------------------
// File: ParallelPrefixSum.cl
// Desc: Parallel Prefix Sum kernels
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Name: PrefixSumSmallPowOf2()
// Desc: Compute the exclusive prefix sum (scan) of the input array.  This
// implementation only works for small power-of-2 sized arrays where the
// maximum supported size is 4 * WORK_GROUP_SIZE.  This algorithm
// is based on "Parallel Prefix Sum (Scan) with CUDA" by Mark Harris,
// Shubhabrata Sengupta and John D. Owens in GPU Gems 3.
//--------------------------------------------------------------------------------------
#define ARRAY_SIZE_512        512
__kernel void PrefixSumSmallPowOf2_512_Local(__global int* pInArray,
                                       __global int* pOutArray)
{
    //General optimization guidelines:
    //Design the kernel for fixed size of input data and use #define to improve execution time
    //Use mul24 and mad24 to speed up 32-bit computation when result is within 24-bit range
    //use right-shift instead of division when possible to avoid complex math operations
    short i = get_local_id(0);
    int iGlobalOffset = mul24((int)get_group_id(0), (int)ARRAY_SIZE_512);
    short sIndA = i * 2 + 1;
    short sIndB = sIndA + 1;
    short sBuffOffsetA = 4 * sIndA -1;
    short sBuffOffsetB = 4 * sIndB -1;
    local int pCache[ARRAY_SIZE_512];
    // Perform the up-sweep phase
    //vectorize memory access to global memory, load 4x32-bit variables to maximize memory bandwidth
    //combine with 2 first steps of sweep-up phase
    int4 i4vec = vload4(i, &pInArray[iGlobalOffset]);
    //When loading data from global to local memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    i4vec.s1 += i4vec.s0;
    i4vec.s3 += i4vec.s2 + i4vec.s1;
    //vectorize memory access local memory, store 4x32-bit variables to maximize memory bandwidth
    vstore4(i4vec, i, pCache);
    barrier( CLK_LOCAL_MEM_FENCE );

    if ( i < (ARRAY_SIZE_512 >> 3) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }

    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(4, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512 >> 4) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }
    barrier( CLK_LOCAL_MEM_FENCE );
    sBuffOffsetA = mad24(8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(8, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>5) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(16, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>6) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(32, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(32, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>7) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }

    barrier( CLK_LOCAL_MEM_FENCE );
    if ( i == 0 )
    {
        pCache[ mad24(128, sIndB+2, -1) ] = pCache[ mad24(128, sIndB+1, -1) ] + pCache[ mad24(128, sIndB, -1) ] + pCache[ mad24(128, sIndA, -1) ];
        pCache[ mad24(128, sIndB+1, -1) ] = pCache[ mad24(128, sIndB, -1) ] + pCache[ mad24(128, sIndA, -1) ];
        pCache[ mad24(128, sIndB, -1) ] = pCache[ mad24(128, sIndA, -1) ];
        pCache[ mad24(128, sIndA, -1) ] = 0;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    if ( i < (ARRAY_SIZE_512>>7) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(-32, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-32, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>6) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(-16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-16, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>5) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(-8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-8, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>4) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(-4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-4, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>3) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    //When loading data from local to global memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    if ( i < (ARRAY_SIZE_512>>2) )
    {
        i4vec = vload4(i, pCache);
        i4vec.s012 += i4vec.s3;
        i4vec.s0123 = i4vec.s3012;
        i4vec.s3 += i4vec.s2 - i4vec.s0;
    }

    // Finally, write the results
    //Vectorize memory access to global memory (4x32-bit)
    vstore4(i4vec, i, &pOutArray[iGlobalOffset]);

}

__kernel void PrefixSumSmallPowOf2_512_Global(__global int* pInArray,
                                       __global int* pOutArray)
{
    //General optimization guidelines:
    //Design the kernel for fixed size of input data and use #define to improve execution time
    //Use mul24 and mad24 to speed up 32-bit computation when result is within 24-bit range
    //use right-shift instead of division when possible to avoid complex math operations
    short i = get_local_id(0);
    int iGlobalOffset = mul24((int)get_group_id(0), (int)ARRAY_SIZE_512);
    __global int * pOutArray_perWG = &pOutArray[iGlobalOffset];
    short sIndA = i * 2 + 1;
    short sIndB = sIndA + 1;
    short sBuffOffsetA = 4 * sIndA -1;
    short sBuffOffsetB = 4 * sIndB -1;
    // Perform the up-sweep phase
    //vectorize memory access to global and local memory
    //combine with 2 first steps of sweep-up phase
    int4 i4vec = vload4(i, &pInArray[iGlobalOffset]);
    //When loading data from global to local memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    i4vec.s1 += i4vec.s0;
    i4vec.s3 += i4vec.s2 + i4vec.s1;
    //vectorize memory access local memory, store 4x32-bit variables to maximize memory bandwidth
    vstore4(i4vec, i, pOutArray_perWG);
    barrier( CLK_GLOBAL_MEM_FENCE );


    if ( i < (ARRAY_SIZE_512>>3) )
    {
        pOutArray_perWG[sBuffOffsetB] += pOutArray_perWG[sBuffOffsetA];
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(4, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>4) )
    {
        pOutArray_perWG[sBuffOffsetB] += pOutArray_perWG[sBuffOffsetA];
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(8, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>5) )
    {
        pOutArray_perWG[sBuffOffsetB] += pOutArray_perWG[sBuffOffsetA];
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(16, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>6) )
    {
        pOutArray_perWG[sBuffOffsetB] += pOutArray_perWG[sBuffOffsetA];
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(32, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(32, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>7) )
    {
        pOutArray_perWG[sBuffOffsetB] += pOutArray_perWG[sBuffOffsetA];
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    if ( i == 0 )
    {
        pOutArray_perWG[mad24(128, sIndB+2, -1) ] = pOutArray_perWG[mad24(128, sIndB+1, -1) ] + pOutArray_perWG[mad24(128, sIndB, -1) ] + pOutArray_perWG[mad24(128, sIndA, -1) ];
        pOutArray_perWG[mad24(128, sIndB+1, -1) ] = pOutArray_perWG[mad24(128, sIndB, -1) ] + pOutArray_perWG[mad24(128, sIndA, -1) ];
        pOutArray_perWG[mad24(128, sIndB, -1) ] = pOutArray_perWG[mad24(128, sIndA, -1) ];
        pOutArray_perWG[mad24(128, sIndA, -1) ] = 0;
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    if ( i < (ARRAY_SIZE_512>>7) )
    {
        int temp = pOutArray_perWG[sBuffOffsetA ];
        pOutArray_perWG[sBuffOffsetA ] = pOutArray_perWG[sBuffOffsetB ];
        pOutArray_perWG[sBuffOffsetB ] += temp;
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(-32, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-32, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>6) )
    {
        int temp = pOutArray_perWG[sBuffOffsetA ];
        pOutArray_perWG[sBuffOffsetA ] = pOutArray_perWG[sBuffOffsetB ];
        pOutArray_perWG[sBuffOffsetB ] += temp;
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(-16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-16, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>5) )
    {
        int temp = pOutArray_perWG[sBuffOffsetA ];
        pOutArray_perWG[sBuffOffsetA ] = pOutArray_perWG[sBuffOffsetB ];
        pOutArray_perWG[sBuffOffsetB ] += temp;
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(-8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-8, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>4) )
    {
        int temp = pOutArray_perWG[sBuffOffsetA ];
        pOutArray_perWG[ sBuffOffsetA ] = pOutArray_perWG[ sBuffOffsetB ];
        pOutArray_perWG[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(-4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-4, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_512>>3) )
    {
        int temp = pOutArray_perWG[sBuffOffsetA ];
        pOutArray_perWG[ sBuffOffsetA ] = pOutArray_perWG[sBuffOffsetB ];
        pOutArray_perWG[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    //When loading data from local to global memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    if ( i < (ARRAY_SIZE_512>>2) )
    {
        i4vec = vload4(i, pOutArray_perWG);
        i4vec.s012 += i4vec.s3;
        i4vec.s0123 = i4vec.s3012;
        i4vec.s3 += i4vec.s2 - i4vec.s0;
    }

    //Vectorize memory access to global memory (4x32-bit)
    vstore4(i4vec, i, pOutArray_perWG);

    return;
}

#define ARRAY_SIZE_256        256
__kernel void PrefixSumSmallPowOf2_256_Local(__global int* pInArray,
                                       __global int* pOutArray)
{
    //General optimization guidelines:
    //Design the kernel for fixed size of input data and use #define to improve execution time
    //Use mul24 and mad24 to speed up 32-bit computation when result is within 24-bit range
    //use right-shift instead of division when possible to avoid complex math operations
    short i = get_local_id(0);
    int iGlobalOffset = mul24((int)get_group_id(0), (int)ARRAY_SIZE_256);
    short sIndA = i * 2 + 1;
    short sIndB = sIndA + 1;
    short sBuffOffsetA = 4 * sIndA - 1;
    short sBuffOffsetB = 4 * sIndB - 1;
    local int pCache[ARRAY_SIZE_256];
    // Perform the up-sweep phase
    //vectorize memory access to global and local memory
    //combine with 2 first steps of sweep-up phase
    int4 i4vec = vload4(i, &pInArray[iGlobalOffset]);
    //When loading data from global to local memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    i4vec.s1 += i4vec.s0;
    i4vec.s3 += i4vec.s2 + i4vec.s1;
    //vectorize memory access local memory, store 4x32-bit variables to maximize memory bandwidth
    vstore4(i4vec, i, pCache);
    barrier( CLK_LOCAL_MEM_FENCE );

    if ( i < (ARRAY_SIZE_256 >> 3) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }

    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(4, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256 >> 4) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }
    barrier( CLK_LOCAL_MEM_FENCE );
    sBuffOffsetA = mad24(8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(8, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256>>5) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(16, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256>>6) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    if ( i == 0 )
    {
        pCache[ mad24(64, sIndB+2, -1) ] = pCache[ mad24(64, sIndB+1, -1) ] + pCache[ mad24(64, sIndB, -1) ] + pCache[ mad24(64, sIndA, -1) ];
        pCache[ mad24(64, sIndB+1, -1) ] = pCache[ mad24(64, sIndB, -1) ] + pCache[ mad24(64, sIndA, -1) ];
        pCache[ mad24(64, sIndB, -1) ] = pCache[ mad24(64, sIndA, -1) ];
        pCache[ mad24(64, sIndA, -1) ] = 0;

    }
    barrier( CLK_LOCAL_MEM_FENCE );

    if ( i < (ARRAY_SIZE_256>>6) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(-16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-16, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256>>5) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(-8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-8, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256>>4) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(-4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-4, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256>>3) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    //When loading data from local to global memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    if ( i < (ARRAY_SIZE_256>>2) )
    {
        i4vec = vload4(i, pCache);
        i4vec.s012 += i4vec.s3;
        i4vec.s0123 = i4vec.s3012;
        i4vec.s3 += i4vec.s2 - i4vec.s0;
    }

    // Finally, write the results
    //Vectorize memory access to global memory (4x32-bit)
    vstore4(i4vec, i, &pOutArray[iGlobalOffset]);

}

__kernel void PrefixSumSmallPowOf2_256_Global(__global int* pInArray,
                                            __global int* pOutArray)
{
    //General optimization guidelines:
    //Design the kernel for fixed size of input data and use #define to improve execution time
    //Use mul24 and mad24 to speed up 32-bit computation when result is within 24-bit range
    //use right-shift instead of division when possible to avoid complex math operations
    short i = get_local_id(0);
    int iGlobalOffset = mul24((int)get_group_id(0), (int)ARRAY_SIZE_256);
    __global int * pOutArray_perWG = &pOutArray[iGlobalOffset];
    short sIndA = i * 2 + 1;
    short sIndB = sIndA + 1;
    short sBuffOffsetA = 4 * sIndA - 1;
    short sBuffOffsetB = 4 * sIndB - 1;
    // Perform the up-sweep phase
    //vectorize memory access to global and local memory
    //combine with 2 first steps of sweep-up phase
    int4 i4vec = vload4(i, &pInArray[iGlobalOffset]);
    //When loading data from global to local memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    i4vec.s1 += i4vec.s0;
    i4vec.s3 += i4vec.s2 + i4vec.s1;
    //vectorize memory access local memory, store 4x32-bit variables to maximize memory bandwidth
    vstore4(i4vec, i, pOutArray_perWG);
    barrier( CLK_GLOBAL_MEM_FENCE );

    if ( i < (ARRAY_SIZE_256 >> 3) )
    {
        pOutArray_perWG[ sBuffOffsetB ] += pOutArray_perWG[ sBuffOffsetA ];
    }

    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(4, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256 >> 4) )
    {
        pOutArray_perWG[ sBuffOffsetB ] += pOutArray_perWG[ sBuffOffsetA ];
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(8, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256>>5) )
    {
        pOutArray_perWG[ sBuffOffsetB ] += pOutArray_perWG[ sBuffOffsetA ];
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(16, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256>>6) )
    {
        pOutArray_perWG[ sBuffOffsetB ] += pOutArray_perWG[ sBuffOffsetA ];
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    if ( i == 0 )
    {
        pOutArray_perWG[ mad24(64, sIndB+2, -1) ] = pOutArray_perWG[ mad24(64, sIndB+1, -1) ] + pOutArray_perWG[ mad24(64, sIndB, -1) ] + pOutArray_perWG[ mad24(64, sIndA, -1) ];
        pOutArray_perWG[ mad24(64, sIndB+1, -1) ] = pOutArray_perWG[ mad24(64, sIndB, -1) ] + pOutArray_perWG[ mad24(64, sIndA, -1) ];
        pOutArray_perWG[ mad24(64, sIndB, -1) ] = pOutArray_perWG[ mad24(64, sIndA, -1) ];
        pOutArray_perWG[ mad24(64, sIndA, -1) ] = 0;

    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    if ( i < (ARRAY_SIZE_256>>6) )
    {
        int temp = pOutArray_perWG[ sBuffOffsetA ];
        pOutArray_perWG[ sBuffOffsetA ] = pOutArray_perWG[ sBuffOffsetB ];
        pOutArray_perWG[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(-16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-16, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256>>5) )
    {
        int temp = pOutArray_perWG[ sBuffOffsetA ];
        pOutArray_perWG[ sBuffOffsetA ] = pOutArray_perWG[ sBuffOffsetB ];
        pOutArray_perWG[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(-8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-8, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256>>4) )
    {
        int temp = pOutArray_perWG[ sBuffOffsetA ];
        pOutArray_perWG[ sBuffOffsetA ] = pOutArray_perWG[ sBuffOffsetB ];
        pOutArray_perWG[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    sBuffOffsetA = mad24(-4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-4, sIndB, sBuffOffsetB);
    if ( i < (ARRAY_SIZE_256>>3) )
    {
        int temp = pOutArray_perWG[ sBuffOffsetA ];
        pOutArray_perWG[ sBuffOffsetA ] = pOutArray_perWG[ sBuffOffsetB ];
        pOutArray_perWG[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_GLOBAL_MEM_FENCE );

    //When loading data from local to global memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    if ( i < (ARRAY_SIZE_256>>2) )
    {
        i4vec = vload4(i, pOutArray_perWG);
        i4vec.s012 += i4vec.s3;
        i4vec.s0123 = i4vec.s3012;
        i4vec.s3 += i4vec.s2 - i4vec.s0;
    }

    // Finally, write the results
    //Vectorize memory access to global memory (4x32-bit)
    vstore4(i4vec, i, pOutArray_perWG);

}


//--------------------------------------------------------------------------------------
// Name: PrefixSumLargePowOf2()
// Desc: Compute the exclusive prefix sum (scan) of *blocks* of the input array.  This
// is the first step in computing the sum of the large array.  It stores the sums of each
// block in pSums and the prefix sums of each block in pOutArray.
//--------------------------------------------------------------------------------------
__kernel void PrefixSumLargePowOf2_Local(const short blockSize,
                                    __global int* pInArray,
                                    __global int* pOutArray,
                                    __global int* pSums,
                                    __local int* pCache )
{
    //General optimization guidelines:
    //Design the kernel for fixed size of input data and use #define to improve execution time
    //Use mul24 and mad24 to speed up 32-bit computation when result is within 24-bit range
    //use right-shift instead of division when possible to avoid complex math operations
    __global int* pInArray_perWG = pInArray + mul24( (int)get_group_id( 0 ), (blockSize << 1) );
    __global int* pOutArray_perWG = pOutArray + mul24( (int)get_group_id( 0 ), (blockSize << 1) );
    __global int* pSum__ = pSums + get_group_id( 0 );
    const short arraySize = 2 * blockSize;
    short i = get_local_id(0);
    short sIndA = i * 2 + 1;
    short sIndB = sIndA + 1;
    short sBuffOffsetA = 4 * sIndA - 1;
    short sBuffOffsetB = 4 * sIndB - 1;
    int4 i4vec;
    //vectorize memory access to global and local memory
    i4vec = vload4(i, pInArray_perWG);
    //When loading data from global to local memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    i4vec.y += i4vec.x;
    i4vec.w += i4vec.z;
    i4vec.w += i4vec.y;
    //vectorize memory access local memory, store 4x32-bit variables to maximize memory bandwidth
    vstore4(i4vec, i, pCache);
    barrier(CLK_LOCAL_MEM_FENCE);

    if( i < (arraySize >> 3) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    sBuffOffsetA = mad24(4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(4, sIndB, sBuffOffsetB);
    if( i < (arraySize >> 4) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    sBuffOffsetA = mad24(8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(8, sIndB, sBuffOffsetB);
    if( i < (arraySize >> 5) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    sBuffOffsetA = mad24(16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(16, sIndB, sBuffOffsetB);
    if( i < (arraySize >> 6) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    sBuffOffsetA = mad24(32, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(32, sIndB, sBuffOffsetB);
    if( i < (arraySize >> 7) )
    {
        pCache[ sBuffOffsetB ] += pCache[ sBuffOffsetA ];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    if ( i == 0 )
    {
        // Store the sum of the block to the sums array before zero'ing it out
        *pSum__ = pCache[ mad24(128, sIndB, -1) ] + pCache[ mad24(128, sIndA, -1) ] + pCache[ mad24(128, sIndB+1, -1) ] + pCache[ mad24(128, sIndB+2, -1) ];
        pCache[ mad24(128, sIndB+2, -1) ] = pCache[ mad24(128, sIndB+1, -1) ] + pCache[ mad24(128, sIndB, -1) ] + pCache[ mad24(128, sIndA, -1) ];
        pCache[ mad24(128, sIndB+1, -1) ] = pCache[ mad24(128, sIndB, -1) ] + pCache[ mad24(128, sIndA, -1) ];
        pCache[ mad24(128, sIndB, -1) ] = pCache[ mad24(128, sIndA, -1) ];
        pCache[ mad24(128, sIndA, -1) ] = 0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    if ( i < (arraySize>>7) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    sBuffOffsetA = mad24(-32, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-32, sIndB, sBuffOffsetB);
    if ( i < (arraySize>>6) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(-16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-16, sIndB, sBuffOffsetB);
    if ( i < (arraySize>>5) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(-8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-8, sIndB, sBuffOffsetB);
    if ( i < (arraySize>>4) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    sBuffOffsetA = mad24(-4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-4, sIndB, sBuffOffsetB);
    if ( i < (arraySize>>3) )
    {
        int temp = pCache[ sBuffOffsetA ];
        pCache[ sBuffOffsetA ] = pCache[ sBuffOffsetB ];
        pCache[ sBuffOffsetB ] += temp;
    }
    barrier( CLK_LOCAL_MEM_FENCE );

    //When loading data from local to global memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    if ( i < (arraySize>>2) )
    {
        i4vec = vload4(i, pCache);
        i4vec.s012 += i4vec.s3;
        i4vec.s0123 = i4vec.s3012;
        i4vec.s3 += i4vec.s2 - i4vec.s0;
    }

    // Finally, write the results
    //Vectorize memory access to global memory (4x32-bit)
    if ( i < (arraySize>>2) )
        vstore4(i4vec, i, pOutArray_perWG);
}

__kernel void PrefixSumLargePowOf2_Global(const short blockSize,
                                    __global int* pInArray,
                                    __global int* pOutArray,
                                    __global int* pSums)
{
    //General optimization guidelines:
    //Design the kernel for fixed size of input data and use #define to improve execution time
    //Use mul24 and mad24 to speed up 32-bit computation when result is within 24-bit range
    //use right-shift instead of division when possible to avoid complex math operations
    __global int* pInArray__ = pInArray + mul24( (int)get_group_id( 0 ), (blockSize << 1) );
    __global int* pOutArray_perWG = pOutArray + mul24((int)get_group_id( 0 ),  blockSize * 2);
    __global int* pSum__ = pSums + get_group_id( 0 );
    const short arraySize = 2 * blockSize;
    short i = get_local_id(0);
    short sIndA = i * 2 + 1;
    short sIndB = sIndA + 1;
    short sBuffOffsetA = 4 * sIndA - 1;
    short sBuffOffsetB = 4 * sIndB - 1;
    int4 i4vec;
    //vectorize memory access to global and local memory
    i4vec = vload4(i, pInArray__);
    //When loading data from global to local memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    i4vec.y += i4vec.x;
    i4vec.w += i4vec.z;
    i4vec.w += i4vec.y;
    //vectorize memory access local memory, store 4x32-bit variables to maximize memory bandwidth
    vstore4(i4vec, i, pOutArray_perWG);
    barrier(CLK_GLOBAL_MEM_FENCE);

    if( i < (arraySize >> 3) )
    {
        pOutArray_perWG[ sBuffOffsetB ] += pOutArray_perWG[ sBuffOffsetA ];
    }
    barrier(CLK_GLOBAL_MEM_FENCE);

    sBuffOffsetA = mad24(4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(4, sIndB, sBuffOffsetB);
    if( i < (arraySize >> 4) )
    {
        pOutArray_perWG[ sBuffOffsetB ] += pOutArray_perWG[ sBuffOffsetA ];
    }
    barrier(CLK_GLOBAL_MEM_FENCE);

    sBuffOffsetA = mad24(8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(8, sIndB, sBuffOffsetB);
    if( i < (arraySize >> 5) )
    {
        pOutArray_perWG[ sBuffOffsetB ] += pOutArray_perWG[ sBuffOffsetA ];
    }
    barrier(CLK_GLOBAL_MEM_FENCE);

    sBuffOffsetA = mad24(16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(16, sIndB, sBuffOffsetB);
    if( i < (arraySize >> 6) )
    {
        pOutArray_perWG[ sBuffOffsetB ] += pOutArray_perWG[ sBuffOffsetA ];
    }
    barrier(CLK_GLOBAL_MEM_FENCE);

    sBuffOffsetA = mad24(32, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(32, sIndB, sBuffOffsetB);
    if( i < (arraySize >> 7) )
    {
        pOutArray_perWG[ sBuffOffsetB ] += pOutArray_perWG[ sBuffOffsetA ];
    }
    barrier(CLK_GLOBAL_MEM_FENCE);

    if ( i == 0 )
    {
        // Store the sum of the block to the sums array before zero'ing it out
        *pSum__ = pOutArray_perWG[ mad24(128, sIndB, -1) ] + pOutArray_perWG[ mad24(128, sIndA, -1) ] + pOutArray_perWG[ mad24(128, sIndB+1, -1) ] + pOutArray_perWG[ mad24(128, sIndB+2, -1) ];
        pOutArray_perWG[mad24(128, sIndB+2, -1) ] = pOutArray_perWG[mad24(128, sIndB+1, -1) ] + pOutArray_perWG[mad24(128, sIndB, -1) ] + pOutArray_perWG[mad24(128, sIndA, -1) ];
        pOutArray_perWG[mad24(128, sIndB+1, -1) ] = pOutArray_perWG[mad24(128, sIndB, -1) ] + pOutArray_perWG[mad24(128, sIndA, -1) ];
        pOutArray_perWG[mad24(128, sIndB, -1) ] = pOutArray_perWG[mad24(128, sIndA, -1) ];
        pOutArray_perWG[mad24(128, sIndA, -1) ] = 0;
    }
    barrier(CLK_GLOBAL_MEM_FENCE);

    if ( i < (arraySize>>7) )
    {
        int temp = pOutArray_perWG[ sBuffOffsetA ];
        pOutArray_perWG[ sBuffOffsetA ] = pOutArray_perWG[ sBuffOffsetB ];
        pOutArray_perWG[ sBuffOffsetB ] += temp;
    }
    barrier(CLK_GLOBAL_MEM_FENCE);

    sBuffOffsetA = mad24(-32, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-32, sIndB, sBuffOffsetB);
    if ( i < (arraySize>>6) )
    {
        int temp = pOutArray_perWG[ sBuffOffsetA ];
        pOutArray_perWG[ sBuffOffsetA ] = pOutArray_perWG[ sBuffOffsetB ];
        pOutArray_perWG[ sBuffOffsetB ] += temp;
    }
    barrier(CLK_GLOBAL_MEM_FENCE);

    sBuffOffsetA = mad24(-16, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-16, sIndB, sBuffOffsetB);
    if ( i < (arraySize>>5) )
    {
        int temp = pOutArray_perWG[ sBuffOffsetA ];
        pOutArray_perWG[ sBuffOffsetA ] = pOutArray_perWG[ sBuffOffsetB ];
        pOutArray_perWG[ sBuffOffsetB ] += temp;
    }
    barrier(CLK_GLOBAL_MEM_FENCE);

    sBuffOffsetA = mad24(-8, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-8, sIndB, sBuffOffsetB);
    if ( i < (arraySize>>4) )
    {
        int temp = pOutArray_perWG[ sBuffOffsetA ];
        pOutArray_perWG[ sBuffOffsetA ] = pOutArray_perWG[ sBuffOffsetB ];
        pOutArray_perWG[ sBuffOffsetB ] += temp;
    }
    barrier(CLK_GLOBAL_MEM_FENCE);

    sBuffOffsetA = mad24(-4, sIndA, sBuffOffsetA);
    sBuffOffsetB = mad24(-4, sIndB, sBuffOffsetB);
    if ( i < (arraySize>>3) )
    {
        int temp = pOutArray_perWG[ sBuffOffsetA ];
        pOutArray_perWG[ sBuffOffsetA ] = pOutArray_perWG[ sBuffOffsetB ];
        pOutArray_perWG[ sBuffOffsetB ] += temp;
    }
    barrier(CLK_GLOBAL_MEM_FENCE);

    //When loading data from local to global memory, processing the data in between the load and store may be more efficient
    //in terms of memory access. In this case it allows each work-item to process 4 elements and removes one barrier
    if ( i < (arraySize>>2) )
    {
        i4vec = vload4(i, pOutArray_perWG);
        i4vec.s012 += i4vec.s3;
        i4vec.s0123 = i4vec.s3012;
        i4vec.s3 += i4vec.s2 - i4vec.s0;
    }

    // Finally, write the results
    //Vectorize memory access to global memory (4x32-bit)
    if ( i < (arraySize>>2) )
        vstore4(i4vec, i, pOutArray_perWG);
}

//--------------------------------------------------------------------------------------
// Name: AddIncrToBlocksPowOf2()
// Desc: The final step in the large prefix sum adds the sums of the preceding blocks
// to each element in the current block.  The final result in pOutArray is the result
// of performing the full parallel prefix sum on the entire input array.
//--------------------------------------------------------------------------------------
__kernel void AddIncrToBlocksPowOf2(__global int* pIncrArray,
                                    __global int4* pOutArray)
{
    if ( get_group_id(0) > 0 )
    {
        //Use right-shift instead of division for faster computation
        __global int* pIncr = pIncrArray + ( get_group_id(0) >> 1 );
        //Each work-item access 4x32-bit elements to maximize global memory bandwidth
        pOutArray[ get_global_id(0) ] += *pIncr;
    }
}
