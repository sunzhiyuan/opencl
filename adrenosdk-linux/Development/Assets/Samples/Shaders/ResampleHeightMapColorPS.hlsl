//--------------------------------------------------------------------------------------
// File: ResampleHeightMapColorPS.hlsl
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//
// ResampleHeightBaseColor, ResampleHeightMapColor
//


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

cbuffer ResampleHeightConstantBuffer
{
    float4 terrainBaseColor;
    float4 constraintAttrib;
    float heightMapSize;
};
struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float4 texcoord0 : TEXCOORD0;
};

struct PixelShaderOutput
{
    half4 Color : SV_TARGET0;
    half4 HeightAndCoverage : SV_TARGET1;
};

Texture2D tColorMap : register(t0);
SamplerState sColorMap : register(s0);

Texture2D tHeightTexture : register(t1);
SamplerState sHeightTexture : register(s1);

Texture2D tCoverageTexture : register(t2);
SamplerState sCoverageTexture : register(s2);

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput pso;

	pso.Color = tColorMap.Sample( sColorMap, input.texcoord0.xy );
	
	pso.HeightAndCoverage.xyz = tHeightTexture.Sample( sHeightTexture, input.texcoord0.xy ).xyz;
	pso.HeightAndCoverage.w = tCoverageTexture.Sample( sCoverageTexture, input.texcoord0.xy ).w;

    return pso;
}

