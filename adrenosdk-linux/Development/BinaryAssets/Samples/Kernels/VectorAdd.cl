//--------------------------------------------------------------------------------------
// File: VectorAdd.cl
// Desc: Vector addition kernel
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Name: VectorAdd()
// Desc: Perform addition of two float4 vector arrsys
//--------------------------------------------------------------------------------------
__kernel void VectorAdd(__global const float4 *a, __global const float4 *b,
                        __global float4 *result)
{
    int gid = get_global_id(0);

    result[gid] = a[gid] + b[gid];
}