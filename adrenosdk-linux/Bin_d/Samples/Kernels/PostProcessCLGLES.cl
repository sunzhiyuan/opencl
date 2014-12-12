//--------------------------------------------------------------------------------------
// File: PostProcessCLGLES.cl
// Desc: Postprocess OpenGL ES rendered FBO and store result in texture
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Name: SobelFilter()
// Desc: Computes a sobel edge detection filter
//--------------------------------------------------------------------------------------
__kernel void SobelFilter(__read_only image2d_t source,
                          __write_only image2d_t dest,
                          int imgWidth,
                          int imgHeight)
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE |
                              CLK_ADDRESS_CLAMP_TO_EDGE   |
                              CLK_FILTER_NEAREST;

    int2 coord = (int2) ( get_global_id(0), get_global_id(1) );


    // The kernel weights are as follows, but we can get better performance
    // by avoiding computing the values with 0.0 weights.
    //
    // const float weightsX[9] = { -1.0f, 0.0f, +1.0f,
    //                            -2.0f, 0.0f, +2.0f,
    //                            -1.0f, 0.0f, +1.0f };
    // const float weightsY[9] = { +1.0f, +2.0f, +1.0f,
    //                             0.0f,  0.0f,  0.0f,
    //                            -1.0f, -2.0f, -1.0f };

    // Fetch all of the neighboring values
    float grayValues[9];
    for( int y = 0; y <= 2; y++ )
    {
        for( int x = 0; x <= 2; x++ )
        {
            float3 rgbCol = read_imagef( source, sampler, coord + (int2) ( x - 1, y - 1) ).xyz;

            // Convert RGB to grayscale value
            float gray = ( 0.299f * rgbCol.x + 0.587f * rgbCol.y + 0.114f * rgbCol.z );
            
            grayValues[ y * 3 + x ] = gray;
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

    // Write the gradient to the output image
    write_imagef( dest, coord, gradient );
}
