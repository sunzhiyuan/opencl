//--------------------------------------------------------------------------------------
// File: ImageBoxFilter.cl
// Desc: Performs box filter convolution on an image
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Name: ImageBoxFilter()
// Desc: Computes an 8x8 box filter on an image
//--------------------------------------------------------------------------------------

#ifdef QC_OPTS // defined in android.mk
// This optimized version reduces the image access from 64 per work item to 16+4 per work item.
// It consists 2 stages:
//     1. Sample 2x2 blocks and save to an intermediate image buffer
//     2. Sample 16 neibhboring 2x2 pxiels from the image buffer to calculate the final image

__kernel void ImageBoxFilter(__read_only image2d_t source,
                                __write_only image2d_t dest,
                                sampler_t sampler,
                                int imgWidth,
                                int imgHeight)
{

    int2 outCoord = (int2) ( get_global_id(0), get_global_id(1) );

    if( outCoord.x < imgWidth && outCoord.y < imgHeight )
    {
        int2 inCoord = outCoord;

        // Sample an 2x2 region and average the results
        float4 sum = 0.0f;
        for( int i = 0; i < 2; i++ )
        {
            for( int j = 0; j < 2; j++ )
            {
                sum += read_imagef( source, sampler, inCoord - (int2)(i, j) );
            }
        }

        // Compute the average, equivalent of divided by 4
        // Notice, 1/4 is a recipricol function performed in EFU.
        // Using *0.25 puts the workload into ALU.
        // Each SP has one EFU and 2 ALU.
        float4 avgColor = sum * 0.25f;

        // Write the result to the output image
        write_imagef( dest, outCoord, avgColor );
    }
}
__kernel void ImageBoxFilter16NSampling(__read_only image2d_t source,
                                __write_only image2d_t dest,
                                sampler_t sampler,
                                int imgWidth,
                                int imgHeight)
{

    int2 outCoord = (int2) ( get_global_id(0), get_global_id(1) );

    if( outCoord.x < imgWidth && outCoord.y < imgHeight )
    {
        int2 offset = outCoord - (int2)(3,3);
        float4 sum = 0.0f;
        int2 coord;

        // Sampling 16 of the 2x2 neighbors
        for( int i = 0; i < 4; i++ )
        {
            for( int j = 0; j < 4; j++ )
            {
                coord = mad24((int2)(i,j), (int2)2, offset);
                sum += read_imagef( source, sampler, coord );
            }
        }

        // Compute the average, equivalent of /16
        // See comment from above kernel
        float4 avgColor = sum * 0.0625;

        // Write the result to the output image
        write_imagef( dest, outCoord, avgColor );
    }
}

#else // Not optimized

__kernel void ImageBoxFilter(__read_only image2d_t source,
                                __write_only image2d_t dest,
                                sampler_t sampler,
                                int imgWidth,
                                int imgHeight)
{

    int2 outCoord = (int2) ( get_global_id(0), get_global_id(1) );

    if( outCoord.x < imgWidth && outCoord.y < imgHeight )
    {
        int2 inCoord = outCoord;

        // Sample an 8x8 region and average the results
        float4 sum = 0.0f;
        for( int i = 0; i < 8; i++ )
        {
            for( int j = 0; j < 8; j++ )
            {
                sum += read_imagef( source, sampler, inCoord + (int2) (i - 4, j - 4 ) );
            }
        }

        // Compute the average
        float4 avgColor = sum / 64.0f;

        // Write the result to the output image
        write_imagef( dest, outCoord, avgColor );
    }
}


#endif //QC_OPTS

