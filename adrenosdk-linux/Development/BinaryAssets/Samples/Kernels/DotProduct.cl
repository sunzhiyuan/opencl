//--------------------------------------------------------------------------------------
// File: DotProduct.cl
// Desc: Vector dot product kernel
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Name: DotProduct()
// Desc: Perform a dot-product between two float4 vector arrays
//--------------------------------------------------------------------------------------
__kernel void DotProduct(__global const float4 *a, __global const float4 *b,
                         __global float *result)
{
    int gid = get_global_id(0);

    result[gid] = dot(a[gid], b[gid]);
}