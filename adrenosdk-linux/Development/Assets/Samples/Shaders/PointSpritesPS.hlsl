

//--------------------------------------------------------------------------------------
// File: PointSpritesPS.hlsl
// Desc: Shaders for the PointSprites sample
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
    float4 vOutColor : COLOR;
};


float4 main(PixelShaderInput input) : SV_TARGET
{
    float fIntensity = tTexture.Sample( sTexture, input.vTexCoord ).r;
    
    return fIntensity * input.vOutColor;
}



