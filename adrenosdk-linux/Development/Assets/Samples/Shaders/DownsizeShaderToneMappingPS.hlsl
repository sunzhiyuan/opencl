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
cbuffer DownsizeShaderConstantBuffer
{
    float2 g_StepSize;
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

    float2 StepSize = g_StepSize;
    
    half4 fSum = half4( 0.0, 0.0, 0.0, 0.0 );

	fSum  = tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 0.5 * StepSize.x, 0.5 * StepSize.y));
    fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-0.5 * StepSize.x, 0.5 * StepSize.y));
    fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-0.5 * StepSize.x,-0.5 * StepSize.y));
    fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 0.5 * StepSize.x,-0.5 * StepSize.y));
	
    // Average
	fSum *= (1.0 / (4.0));

    return fSum;
}
