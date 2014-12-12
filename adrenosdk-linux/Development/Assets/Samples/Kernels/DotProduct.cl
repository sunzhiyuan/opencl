//--------------------------------------------------------------------------------------
// File: DotProduct.cl
// Desc: Vector dot product kernel
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Name: DotProduct()
// Desc: Perform a dot-product between two float4 vector arrays
//--------------------------------------------------------------------------------------

#ifdef QC_OPTS

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

// This kernel uses image to access data which utilize TP to fetch external data.
// TP still gets data through UCHE, but it generally has advantage for data access due
// to its dedicated L1 cache.
__kernel void DotProduct(__read_only image2d_t a, __read_only image2d_t b,
                         __global float *result, const int w, const int h)
{
    int2 gid = (get_global_id(0), get_global_id(1));
    if ( gid.x < w && gid.y < h)
    {
        result[gid.y * w + gid.x] = dot(read_imagef(a, sampler, gid), read_imagef(b, sampler, gid));
    }
}

#else // not optimized

// This kernel access global memory directly, which goes through UCHE.
// UCHE has L2 cache, but it's shared among a few components, not just SP.
__kernel void DotProduct(__global const float4 *a, __global const float4 *b,
                         __global float *result, const int numVec)
{
    int gid = get_global_id(0);

    if (gid < numVec) result[gid] = dot(a[gid], b[gid]);
}

#endif // QC_OPTS

