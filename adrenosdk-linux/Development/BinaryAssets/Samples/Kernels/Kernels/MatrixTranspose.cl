//--------------------------------------------------------------------------------------
// File: MatrixTranspose.cl
// Desc: Matrix transpose kernel
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
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
    int row = get_global_id(0);

    if( row < rows )
    {
        for( int col = 0; col < cols; col++ )
        {
            matrixTranspose[col * rows + row] = matrix[row * cols + col];
        }
    }
}