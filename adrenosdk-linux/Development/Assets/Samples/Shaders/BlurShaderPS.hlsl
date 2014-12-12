
//--------------------------------------------------------------------------------------
// File: BlurShaderPS.hlsl
// Desc: Shaders for per-pixel lighting
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

cbuffer BlurConstantBuffer
{
    float4 GaussWeight;
    float2 StepSize;    
    float  GaussInvSum;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

Texture2D tColorTexture : register(t0);
SamplerState sColorTexture : register(s0);

half4 main(PixelShaderInput input) : SV_TARGET
{
    // 4 tap gaussian distribution
    half4 Color = GaussWeight[0] * tColorTexture.Sample( sColorTexture, float2( input.TexCoord.x + StepSize.x, input.TexCoord.y + StepSize.y ) );
    Color += GaussWeight[1] * tColorTexture.Sample( sColorTexture, float2( input.TexCoord.x + StepSize.x * 0.5, input.TexCoord.y + StepSize.y * 0.5 ) );
    Color += GaussWeight[2] * tColorTexture.Sample( sColorTexture, float2( input.TexCoord.x - StepSize.x * 0.5, input.TexCoord.y - StepSize.y * 0.5 ) );
    Color += GaussWeight[3] * tColorTexture.Sample( sColorTexture, float2( input.TexCoord.x - StepSize.x, input.TexCoord.y - StepSize.y ) );

    return Color * GaussInvSum;    
}
