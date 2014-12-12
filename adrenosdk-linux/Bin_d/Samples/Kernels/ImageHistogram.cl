//--------------------------------------------------------------------------------------
// File: ImageHistogram.cl
// Desc: Performs histogram on an image
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Name: ImageHistogram()
// Desc:
//--------------------------------------------------------------------------------------

#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;
__constant float4 color_element = (float4)(76, 150, 29, 0);

// Count histogram and use global memory to store result
__kernel void ImageGrayscaleHistogram_global(
                    __read_only image2d_t source,
                    __write_only image2d_t dest,
                    __global int* total,
                    const int width, const int height)
{
    int2 coord = (int2) ( get_global_id(0), get_global_id(1) );

    if (coord.x < width && coord.y < height) {

        float4 color = read_imagef( source, sampler, coord);

        // Following operations can also be done in half/short for better performance,
        // if higher error margin is tolerated.
        unsigned int gray_index = (unsigned int) dot(color, color_element);

        // Notice, CL_R format may be used to save bandwidth.
        // Here, it saves to RGBA format for convenience.
        write_imageui( dest, coord, (uint4)(gray_index, gray_index, gray_index, 255));

        atom_inc(&total[gray_index]);
    }
}

// Count histogram and use local memory to store result for each work group.
// Local result is then tallied into global memory.
// This kernel assumes the workgroup size is greater than 64 as it uses local id as index
// to access local memory of 256 int.
__kernel void ImageGrayscaleHistogram_local(
                    __read_only image2d_t source,
                    __write_only image2d_t dest,
                    __global int* total,
                    const int width, const int height)
{
    __local int stat[256];

    int2 coord = (int2) ( get_global_id(0), get_global_id(1) );
    int l_offset = mad24(get_local_size(0), get_local_id(1), get_local_id(0)) << 2;

    if (l_offset < 256)
    {
        vstore4((int4)(0), 0, &stat[l_offset]);
    }

    // block all work items till local memory is initialized
    barrier( CLK_LOCAL_MEM_FENCE );

    if (coord.x < width && coord.y < height) {

        float4 color = read_imagef( source, sampler, coord);

        // Following operations can also be done in half/short for better performance,
        // if higher error margin is tolerated.
        unsigned int gray_index = (unsigned int) dot(color, color_element);

        // Notice, CL_R format may be used to save bandwidth.
        // Here, it saves to RGBA format for convenience.
        write_imageui( dest, coord, (uint4)(gray_index, gray_index, gray_index, 255));

        atom_inc(&stat[gray_index]);
    }
    // make sure every workitem finished before updating global
    barrier( CLK_LOCAL_MEM_FENCE );

    if (l_offset < 256)
    {
        // add the local stats to global stats
        atom_add(&total[l_offset], stat[l_offset]);
        atom_add(&total[l_offset+1], stat[l_offset+1]);
        atom_add(&total[l_offset+2], stat[l_offset+2]);
        atom_add(&total[l_offset+3], stat[l_offset+3]);
    }
}

// Map image color with a grayscale vector of 0-255
__kernel void ImageMapped(
                __read_only image2d_t source,
                __write_only image2d_t dest,

                // Specifying constant size is optional, it is shown here for clarification.
                __constant unsigned int* lookup
                __attribute__((max_constant_size(1024))),

                const int imgWidth,
                const int imgHeight)
{
    int2 coord = (int2) ( get_global_id(0), get_global_id(1) );

    if( coord.x < imgWidth && coord.y < imgHeight )
    {
        uint4 grayscale = read_imageui( source, sampler, coord);
        unsigned int adj_scale = lookup[grayscale.x];

        // Notice, CL_R format may be used to save bandwidth.
        // Here, it saves to RGBA format for convenience.
        write_imageui( dest, coord, (uint4)(adj_scale, adj_scale, adj_scale, 255));
    }
}

