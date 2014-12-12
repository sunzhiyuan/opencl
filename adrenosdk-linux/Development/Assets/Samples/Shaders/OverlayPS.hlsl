

//--------------------------------------------------------------------------------------
// File: OverlayPS.hlsl
// Desc: Pixel Shader for drawing a textured overlay
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


Texture2D overlayTexture : register(t0);
SamplerState overlaySampler: register(s0);

struct PixelShaderInput
{
    float4 vVertexPos : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
 // For looking at shadow maps:
 //   return float4(overlayTexture.Sample(overlaySampler, input.vTexCoord).rrr, 1.0);
    return overlayTexture.Sample(overlaySampler, input.vTexCoord);
}