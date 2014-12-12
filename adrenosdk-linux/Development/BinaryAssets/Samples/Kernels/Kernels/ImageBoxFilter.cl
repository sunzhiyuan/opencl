//--------------------------------------------------------------------------------------
// File: ImageBoxFilter.cl
// Desc: Performs box filter convolution on an image
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// Name: ImageBoxFilter8x8()
// Desc: Computes an 8x8 box filter on an image
//--------------------------------------------------------------------------------------
__kernel void ImageBoxFilter8x8(__read_only image2d_t source,
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

