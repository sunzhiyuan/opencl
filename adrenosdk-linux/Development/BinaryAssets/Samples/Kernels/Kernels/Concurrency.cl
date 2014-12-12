//--------------------------------------------------------------------------------------
// File: Concurrency.cl
// Desc: Kernel used for demonstrating doing concurrent computation across devices
//       (GPU + CPU)
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Name: ConcurrentMarixMatrixMul()
// Desc: Compute the multiplication of two matrices.  The work is spread across
// multiple devices by breaking up the work-items across CPU + GPU
//--------------------------------------------------------------------------------------
__kernel void ConcurrentMatrixMatrixMul(const int matrixRowsA,
                                        const int matrixColsARowsB,
                                        const int matrixColsB,
                                        __global float* matrixA,
                                        __global float* matrixB,
                                        __global float* matrixProduct)
{
    int i = get_global_id(0);
    int j = get_global_id(1);

    if( i < matrixRowsA && j < matrixColsB )
    {
        float result = 0.0f;
        for( int k = 0; k < matrixColsARowsB; k++ )
        {
            int indexA = i * matrixColsARowsB + k;
            int indexB = k * matrixColsB + j;
            result += matrixA[indexA] * matrixB[indexB];
        }

        matrixProduct[i * matrixColsB + j] = result;
    }
}

