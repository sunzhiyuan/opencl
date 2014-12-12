//--------------------------------------------------------------------------------------
// File: ImageSobelFilter.cl
// Desc: Performs sobel filter convolution on an image
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


#ifdef QC_OPTS
#define QC_OPTS_USE_HALF
#define QC_OPTS_WGSIZE
#endif

#if defined(QC_OPTS) && defined(QC_OPTS_USE_HALF)
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#endif

#ifdef QC_OPTS
const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;
#endif

//--------------------------------------------------------------------------------------
// Name: ImageConvertRGBToGray()
// Desc: Convert the input image from RGBA -> grayscale as a preprocess
//--------------------------------------------------------------------------------------
__kernel void ImageConvertRGBToGray(__read_only image2d_t source,
                                    __write_only image2d_t dest,
#ifndef QC_OPTS
                                    sampler_t sampler,
#endif
                                    int imgWidth,
                                    int imgHeight)
{
    int2 coord = (int2) ( get_global_id(0), get_global_id(1) );
#if !defined(QC_OPTS_WGSIZE)
    if( coord.x < imgWidth && coord.y < imgHeight )
#endif
    {

        float3 rgbCol = read_imagef( source, sampler, coord ).xyz;

        // Convert RGB to grayscale value
        float gray = ( 0.299f * rgbCol.x + 0.587f * rgbCol.y + 0.114f * rgbCol.z );

        // Write the grayscale results to the output image
        write_imagef( dest, coord, gray );
    }
}

//--------------------------------------------------------------------------------------
// Name: ImageSobelFilter()
// Desc: Computes a sobel edge detection filter on incoming grayscale image outputing
// to grayscale image.
//--------------------------------------------------------------------------------------
__kernel void ImageSobelFilter(__read_only image2d_t source,
                               __write_only image2d_t dest,
#ifndef QC_OPTS
                               sampler_t sampler,
#endif
                               int imgWidth,
                               int imgHeight)
{
    int2 coord = (int2) ( get_global_id(0), get_global_id(1) );
#if !defined(QC_OPTS_WGSIZE)
    if( coord.x < imgWidth && coord.y < imgHeight )
#endif
    {
        // The kernel weights are as follows, but we can get better performance
        // by avoiding computing the values with 0.0 weights.
        //
        // const float weightsX[9] = { -1.0f, 0.0f, +1.0f,
        //                            -2.0f, 0.0f, +2.0f,
        //                            -1.0f, 0.0f, +1.0f };
        // const float weightsY[9] = { +1.0f, +2.0f, +1.0f,
        //                             0.0f,  0.0f,  0.0f,
        //                            -1.0f, -2.0f, -1.0f };

#ifndef QC_OPTS

        // Fetch all of the neighboring values
        float grayValues[9];
        for( int y = 0; y <= 2; y++ )
        {
            for( int x = 0; x <= 2; x++ )
            {
                grayValues[ y * 3 + x ] = read_imagef( source, sampler, coord + (int2) ( x - 1, y - 1) ).x;
            }
        }

        // Compute the gradients directly using the weights above, skipping the values that have
        // 0 weights
        float2 sum;
        sum.x = -grayValues[0] + grayValues[2]
                -2.0f * grayValues[3] + 2.0f * grayValues[5]
                -grayValues[6] + grayValues[8];
        sum.y = grayValues[0] + 2.0f * grayValues[1] + grayValues[2]
                -grayValues[6] - 2.0f * grayValues[7] - grayValues[8];

        float gradient = length( sum );

        // Write the gradient to the output grayscale image
        write_imagef( dest, coord, gradient );

#elif !defined(QC_OPTS_USE_HALF)
        // Fetch all of the neighboring values
        float8 grayValues;

        grayValues.s0 = read_imagef( source, sampler, coord + (int2) (-1, -1) ).x;
        grayValues.s1 = read_imagef( source, sampler, coord + (int2) (0, -1) ).x;
        grayValues.s2 = read_imagef( source, sampler, coord + (int2) (1, -1) ).x;

        grayValues.s3 = read_imagef( source, sampler, coord + (int2) (-1, 0) ).x;
        grayValues.s4 = read_imagef( source, sampler, coord + (int2) (1, 0) ).x;

        grayValues.s5 = read_imagef( source, sampler, coord + (int2) (-1, 1) ).x;
        grayValues.s6 = read_imagef( source, sampler, coord + (int2) (0, 1) ).x;
        grayValues.s7 = read_imagef( source, sampler, coord + (int2) (1, 1) ).x;


        // Compute the gradients directly using the weights above, skipping the values that have
        // 0 weights
        float2 sum;
        sum.x  = grayValues.s2 + 2.0f * grayValues.s4 + grayValues.s7;
        sum.x -= grayValues.s0 + 2.0f * grayValues.s3 + grayValues.s5;

        sum.y  = grayValues.s5 + 2.0f * grayValues.s6 + grayValues.s7;
        sum.y -= grayValues.s0 + 2.0f * grayValues.s1 + grayValues.s2;

        //float gradient = length( sum );
        float gradient = sqrt(sum.x * sum.x + sum.y * sum.y);

        // Write the gradient to the output grayscale image
        write_imagef( dest, coord, gradient );

#else

        half8 grayValues;

        grayValues.s0 = read_imageh( source, sampler, coord + (int2) (-1, -1) ).x;
        grayValues.s1 = read_imageh( source, sampler, coord + (int2) (0, -1) ).x;
        grayValues.s2 = read_imageh( source, sampler, coord + (int2) (1, -1) ).x;

        grayValues.s3 = read_imageh( source, sampler, coord + (int2) (-1, 0) ).x;
        grayValues.s4 = read_imageh( source, sampler, coord + (int2) (1, 0) ).x;

        grayValues.s5 = read_imageh( source, sampler, coord + (int2) (-1, 1) ).x;
        grayValues.s6 = read_imageh( source, sampler, coord + (int2) (0, 1) ).x;
        grayValues.s7 = read_imageh( source, sampler, coord + (int2) (1, 1) ).x;


        // Compute the gradients directly using the weights above, skipping the values that have
        // 0 weights
        half2 sum;
        sum.x  = grayValues.s2 + 2.0f * grayValues.s4 + grayValues.s7;
        sum.x -= grayValues.s0 + 2.0f * grayValues.s3 + grayValues.s5;

        sum.y  = grayValues.s5 + 2.0f * grayValues.s6 + grayValues.s7;
        sum.y -= grayValues.s0 + 2.0f * grayValues.s1 + grayValues.s2;

        //float gradient = length( sum );
        half gradient = sqrt(sum.x * sum.x + sum.y * sum.y);

        // Write the gradient to the output grayscale image
        write_imageh( dest, coord, gradient );

#endif

    }
}
