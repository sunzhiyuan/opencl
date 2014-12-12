//--------------------------------------------------------------------------------------
// File: MatrixTranspose.cl
// Desc: Matrix transpose kernel
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Name: MatrixTranspose()
// Desc: Compute the transpose of a matrix
//--------------------------------------------------------------------------------------
__kernel void MatrixTranspose(const int rows,
                              const int cols,
                              __global float* matrix,
                              __global float* matrixTranspose)
{
    int gid = get_global_id(0);
    int indexSrc = cols*gid;
    int iters = cols >> 2;
    int offset = 0;

    for(int i=0; i < iters; i++)
    {
        // Vectorization of loads results in better utilization of memory bandwidth
        float4 tmp1 = (*((__global float4*)&matrix[indexSrc]));

        matrixTranspose[gid+offset] = tmp1.x;
        offset += rows;
        matrixTranspose[gid+offset] = tmp1.y;
        offset += rows;
        matrixTranspose[gid+offset] = tmp1.z;
        offset += rows;
        matrixTranspose[gid+offset] = tmp1.w;
        offset += rows;

        indexSrc += 4;
    }
}
