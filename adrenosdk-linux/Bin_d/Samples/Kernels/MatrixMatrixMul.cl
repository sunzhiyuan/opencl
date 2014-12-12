//--------------------------------------------------------------------------------------
// File: MatrixMatrixMul.cl
// Desc: Matrix-matrix product kernels
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Name: MatrixMatrixMulSimple()
// Desc: Compute the multiplication of two matrices.  Simple approach where each
// work-item computes one cell in the resulting matrix
//--------------------------------------------------------------------------------------
__kernel void MatrixMatrixMulSimple(const int matrixRowsA,
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
        float result = 0.0;
        for( int k = 0; k < matrixColsARowsB; k++ )
        {
            int indexA = i * matrixColsARowsB + k;
            int indexB = k * matrixColsB + j;
            result += matrixA[indexA] * matrixB[indexB];
        }

        matrixProduct[i * matrixColsB + j] = result;
    }
}

//--------------------------------------------------------------------------------------
// Name: MatrixTranspose()
// Desc: Compute the transpose of a matrix
// Tranposing Matrix B (for computing product of A and B) helps avoid column accesses
// for Matrix B which in turn helps take advantage of data locality
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
        // Vectorization helps utilize the memory bandwidth better
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

//--------------------------------------------------------------------------------------
// Name: MatrixMatrixMulOptimized()
// Desc: Compute the multiplication of two matrices.  In this case, each work-item
// computes a full row of the resulting matrix C.  Additionally, local memory is
// used to cache intermediate fetched values across work-items. Also, vectorized
// loads are used to utilize the memory bandwidth better.
//--------------------------------------------------------------------------------------
__kernel void MatrixMatrixMulOptimized(const int matrixRowsA,
                                        const int matrixColsARowsB,
                                        const int matrixColsB,
                                        __global float* matrixA,
                                        __global float* matrixBTranspose,
                                        __global float* matrixProduct,
                     __local float* dataCacheB)
{
    int gid = get_global_id(0);
    int lid = get_local_id(0);
    int lsize = get_local_size(0);
    int resultIndex = gid*matrixColsB;
    int iters = matrixColsARowsB >> 2;

    for(int j=0; j < matrixColsB; j++)
    {
        // Use Local Memory to cache BTranspose's rows
        // Fill in the portion of BTranspose's row that this work-item is responsible for
        int offset = j*matrixColsARowsB;
        for(int k=lid; (((k&3)==0) && k<matrixColsARowsB); k+=lsize)
        {
            *((__local float4*)&dataCacheB[k]) = *((__global float4*)&matrixBTranspose[k+offset]);
        }

        // Insert a barrier so all work-items in the workgroup wait until dataCacheB is filled
        barrier( CLK_LOCAL_MEM_FENCE );

        int indexA = matrixColsARowsB*gid;
        int indexBTranspose = 0;
        float result = 0.0f;
        for(int i=0; i < iters; i++)
        {
            // Vectorization of loads help utilize the memory bandwidth better
            float4 tmpRow = (*((__global float4*)&matrixA[indexA]));
            float4 tmpCol = (*((__local float4*)&dataCacheB[indexBTranspose]));
            result += dot(tmpRow, tmpCol);
            indexBTranspose += 4;
            indexA += 4;
        }
        matrixProduct[resultIndex+j] = result;

        // This barrier makes sure all reads from dataCacheB complete before the next iteration
        // where the data will be written to again
        barrier( CLK_LOCAL_MEM_FENCE );
    }
}
