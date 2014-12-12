//--------------------------------------------------------------------------------------
// File: sRGBFinalGammeCorrectPS.hlsl
// Desc: 
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

Texture2D tTexture : register(t0);
SamplerState sTexture : register(s0);

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 Color = tTexture.Sample( sTexture, input.vTexCoord );
    // Perform gamma correction - we are fetching from an sRGB render target
    // that we actually now have in the correct space, so we want to undo
    // its correction
    Color = pow(Color, (1.0 / 2.2));
    return Color;
}


