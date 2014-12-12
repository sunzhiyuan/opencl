//--------------------------------------------------------------------------------------
// File: DepthPS.glsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// The fragment shader for rendering to the shadow map
//--------------------------------------------------------------------------------------

struct PixelShaderInput
{
    float4 vVertexPos : SV_POSITION;
    float4 vClipPos : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    // Depth is Z/W.  This is returned by the pixel shader.    
    float depth = (input.vClipPos.z / input.vClipPos.w);    
    
    // Depth that would be in a depth buffer is in the min/max depth
    // range (defaults to [0,1]) whereas depth above is in [-1,1].
    // Move into [0,1] range
    depth = depth * 0.5 + 0.5;

    // Finally, invert for better precision in the near part of the
    // buffer
    depth = 1.0 - depth;

    // Return the depth in 32-bit color channel
    return float4(depth, 0.0, 0.0, 1.0);
}

