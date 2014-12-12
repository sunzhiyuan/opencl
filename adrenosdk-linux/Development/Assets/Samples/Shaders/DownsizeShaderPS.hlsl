//--------------------------------------------------------------------------------------
// File: DownsizeShaderPS.hlsl
// Desc: Shaders for per-pixel lighting
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

Texture2D tColorTexture : register(t0);
SamplerState sColorTexture : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    return tColorTexture.Sample( sColorTexture, input.TexCoord );
}
