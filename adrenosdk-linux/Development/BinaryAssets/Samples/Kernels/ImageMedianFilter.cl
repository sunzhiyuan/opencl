//--------------------------------------------------------------------------------------
// File: ImageMedianFilter.cl
// Desc: Performs median filter on an image
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


#ifdef QC_OPTS
#if 1
#pragma OPENCL EXTENSION cl_khr_fp16 : enable

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

#endif

#define s2(a,b)            { half4 tmp = a; a = fmin(a,b); b = fmax(tmp,b); }

#define mn3(a,b,c)         s2(a,b); s2(a,c);
#define mx3(a,b,c)         s2(b,c); s2(a,c);

#define mnmx3(a,b,c)       mx3(a,b,c); s2(a,b);
#define mnmx4(a,b,c,d)     s2(a,b); s2(c,d); s2(a,c); s2(b,d);
#define mnmx5(a,b,c,d,e)   s2(a,b); s2(c,d); mn3(a,c,e); mx3(b,d,e);
#define mnmx6(a,b,c,d,e,f) s2(a,d); s2(b,e); s2(c,f); mn3(a,b,c); mx3(d,e,f);

//--------------------------------------------------------------------------------------
// Name: ImageMedianFilter()
// Desc: Perform a 3x3 median filter on the image
//--------------------------------------------------------------------------------------
__kernel void ImageMedianFilter(__read_only image2d_t source,
                                __write_only image2d_t dest,
                                int imgWidth,
                                int imgHeight)
{
    int2 coord = (int2) ( get_global_id(0), get_global_id(1) );

    if( coord.x < imgWidth && coord.y < imgHeight )
    {
        half4 v[6];

        // The median filter works by taking the median of the nine neighboring
        // pixels.  Please see the CPU reference code in Compute.cpp for a
        // more clear (and unoptimized version) of the code that sorts each
        // 9 pixel blocks by using qsort() and takes the middle value.  The
        // technique below is an optimized version as described in:
        // "A Fast Small-Radius GPU Median Filter" by Morgan McGuire
        // in ShaderX3

        // Read in 6 of the neighboring pixels from the image
        v[0] = read_imageh( source, sampler, coord + (int2) ( -1, -1 ) );
        v[1] = read_imageh( source, sampler, coord + (int2) (  0, -1 ) );
        v[2] = read_imageh( source, sampler, coord + (int2) ( +1, -1 ) );
        v[3] = read_imageh( source, sampler, coord + (int2) ( -1,  0 ) );
        v[4] = read_imageh( source, sampler, coord + (int2) (  0,  0 ) );
        v[5] = read_imageh( source, sampler, coord + (int2) (  1,  0 ) );

        // Starting with 6 neighbors, remove the min/max
        mnmx6(v[0], v[1], v[2], v[3], v[4], v[5]);

        // add the 7th pixel to test and then remove the min/max again
        v[5] = read_imageh( source, sampler, coord + (int2) (  -1,  +1 ) );
        mnmx5(v[1], v[2], v[3], v[4], v[5]);

        // add the 8th pixel to test and then remove the min/max again
        v[5] = read_imageh( source, sampler, coord + (int2) (  0,  +1 ) );
        mnmx4(v[2], v[3], v[4], v[5]);

        // Finally add the 9th pixel, and remove the min max
        v[5] = read_imageh( source, sampler, coord + (int2) ( +1,  +1 ) );
        mnmx3(v[3], v[4], v[5]);

        // The median for each of the color components is now in the center value which
        // is index 4
        half4 median = (half4) ( v[4].xyz, 1.0 );

        // Write the median to the output image
        write_imageh( dest, coord, median );
    }
}


#else
// Use technique described in "A Fast Small-Radius GPU Median Filter" by Morgan McGuire
// in ShaderX3.  This technique uses a series of min/max operations to whittle the min/max vals
// until left with the middle value.  The slight adaptation is that this is vectorized so
// it sorts each color component separately.
#define s2(a,b)            { float4 tmp = a; a = min(a,b); b = max(tmp,b); }

#define mn3(a,b,c)         s2(a,b); s2(a,c);
#define mx3(a,b,c)         s2(b,c); s2(a,c);

#define mnmx3(a,b,c)       mx3(a,b,c); s2(a,b);
#define mnmx4(a,b,c,d)     s2(a,b); s2(c,d); s2(a,c); s2(b,d);
#define mnmx5(a,b,c,d,e)   s2(a,b); s2(c,d); mn3(a,c,e); mx3(b,d,e);
#define mnmx6(a,b,c,d,e,f) s2(a,d); s2(b,e); s2(c,f); mn3(a,b,c); mx3(d,e,f);

//--------------------------------------------------------------------------------------
// Name: ImageMedianFilter()
// Desc: Perform a 3x3 median filter on the image
//--------------------------------------------------------------------------------------
__kernel void ImageMedianFilter(__read_only image2d_t source,
                                __write_only image2d_t dest,
                                sampler_t sampler,
                                int imgWidth,
                                int imgHeight)
{
    int2 coord = (int2) ( get_global_id(0), get_global_id(1) );
 
    if( coord.x < imgWidth && coord.y < imgHeight )
    {
        float4 v[6]; 
       
        // The median filter works by taking the median of the nine neighboring
        // pixels.  Please see the CPU reference code in Compute.cpp for a 
        // more clear (and unoptimized version) of the code that sorts each
        // 9 pixel blocks by using qsort() and takes the middle value.  The
        // technique below is an optimized version as described in:
        // "A Fast Small-Radius GPU Median Filter" by Morgan McGuire
        // in ShaderX3

        // Read in 6 of the neighboring pixels from the image
        v[0] = read_imagef( source, sampler, coord + (int2) ( -1, -1 ) );
        v[1] = read_imagef( source, sampler, coord + (int2) (  0, -1 ) );
        v[2] = read_imagef( source, sampler, coord + (int2) ( +1, -1 ) );
        v[3] = read_imagef( source, sampler, coord + (int2) ( -1,  0 ) );
        v[4] = read_imagef( source, sampler, coord + (int2) (  0,  0 ) );
        v[5] = read_imagef( source, sampler, coord + (int2) (  1,  0 ) );
       
        // Starting with 6 neighbors, remove the min/max
        mnmx6(v[0], v[1], v[2], v[3], v[4], v[5]);
            
        // add the 7th pixel to test and then remove the min/max again
        v[5] = read_imagef( source, sampler, coord + (int2) (  -1,  +1 ) );
        mnmx5(v[1], v[2], v[3], v[4], v[5]);

        // add the 8th pixel to test and then remove the min/max again
        v[5] = read_imagef( source, sampler, coord + (int2) (  0,  +1 ) );
        mnmx4(v[2], v[3], v[4], v[5]);

        // Finally add the 9th pixel, and remove the min max
        v[5] = read_imagef( source, sampler, coord + (int2) ( +1,  +1 ) );
        mnmx3(v[3], v[4], v[5]);

        // The median for each of the color components is now in the center value which
        // is index 4
        float4 median = (float4) ( v[4].xyz, 1.0 );

        // Write the median to the output image
        write_imagef( dest, coord, median );
    }
}
#endif