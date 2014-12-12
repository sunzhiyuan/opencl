//--------------------------------------------------------------------------------------
// File: MatrixVectorMul.cl
// Desc: Matrix vector product kernel
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Name: MatrixVectorMul()
// Desc: Perform multiplication of vector by a matrix
//--------------------------------------------------------------------------------------
__kernel void MatrixVectorMul(const int rows,
                              const int cols,
                              __global float* matrix,
                              __global float* vector,
                              __global float* resultVector)
{
    int gid   = get_global_id(0);
    int index = cols*gid;
    int iters = cols >> 2;

    for(int i=0; i < iters; i++)
    {
        // Vectorizing loads and stores results in better utilization of memory bandwidth
        float4 tmp1 = (*((__global float4*)&matrix[index]));
        float4 tmp2 = (*((__global float4*)&vector[4*i]));

        resultVector[gid] += dot(tmp1, tmp2);
        index += 4;
    }
}
