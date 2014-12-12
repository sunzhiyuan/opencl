//--------------------------------------------------------------------------------------
// File: MatrixVectorMul.cl
// Desc: Matrix vector product kernel
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
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
    int row = get_global_id(0);

    if( row < rows )
    {
        float rowVal = 0.0;
        for( int col = 0; col < cols; col++ )
        {
            rowVal += matrix[row * cols + col] * vector[col];
        }

        resultVector[row] = rowVal;
    }
}