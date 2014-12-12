//--------------------------------------------------------------------------------------
// File: MatrixMatrixAdd.cl
// Desc: Matrix addition kernel
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Name: MatrixAdd()
// Desc: Perform addition of two matrices
// Use a 2D work schedule where each work-item computes one element of C
//--------------------------------------------------------------------------------------
__kernel void MatrixMatrixAddSimple(const int matrixRows,
                                    const int matrixCols,
                                    __global float* matrixA,
                                    __global float* matrixB,
                                    __global float* MatrixSum)
{
    int i = get_global_id(0);
    int j = get_global_id(1);

    MatrixSum[i*matrixCols+j] = matrixA[i*matrixCols+j] + matrixB[i*matrixCols+j];
}

//--------------------------------------------------------------------------------------
// Name: MatrixAdd()
// Desc: Perform addition of two matrices
// Optimization 1: Use a 1D work schedule where each work-item computes a whole row of C
//               Vectorize loads and stores
//--------------------------------------------------------------------------------------
__kernel void MatrixMatrixAddOptimized1(const int rows,
                                    const int cols,
                                    __global float* matrixA,
                                    __global float* matrixB,
                                    __global float* MatrixSum)
{
    int gid = get_global_id(0);
    int indexSrc = cols*gid;
    int iters = cols >> 2;

    for(int i=0; i < iters; i++)
    {
        // Vectorizing loads and stores results in better utilization of memory bandwidth
        float4 tmpA = (*((__global float4*)&matrixA[indexSrc]));
        float4 tmpB = (*((__global float4*)&matrixB[indexSrc]));
        float4 tmpSum = tmpA+tmpB;
        (*((__global float4*)&MatrixSum[indexSrc])) = tmpSum;
        indexSrc +=4 ;
    }
}

//--------------------------------------------------------------------------------------
// Name: MatrixAdd()
// Desc: Perform addition of two matrices
// Optimization 2: Use a 2D work schedule where each work-item computes sum of 4 floats
//                 Vectorize loads and stores
//--------------------------------------------------------------------------------------
__kernel void MatrixMatrixAddOptimized2(const int rows,
                                    const int cols,
                                    __global float* matrixA,
                                    __global float* matrixB,
                                    __global float* MatrixSum)
{
    int i = get_global_id(0);
    int j = get_global_id(1);
    int offset = mul24(j, cols);
    int index  = mad24(i, 4, offset);

    // Vectorizing loads and stores results in better utilization of memory bandwidth
    float4 tmpA = (*((__global float4*)&matrixA[index]));
    float4 tmpB = (*((__global float4*)&matrixB[index]));
    (*((__global float4*)&MatrixSum[index])) = (tmpA+tmpB);
}
