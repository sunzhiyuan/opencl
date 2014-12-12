

//--------------------------------------------------------------------------------------
// File: SkydomePS.hlsl
// Desc: Pixel Shader for Skydome in the BumpedReflectionMapping sample
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

TextureCube tCube : register(t0);
SamplerState sCube: register(s0);

cbuffer SkydomeConstantBuffer
{
    float4x4 MatModelViewProj;
    float4x4 MatNormal;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 CubeTexCoord : TEXCOORD0;
};

half4 main(PixelShaderInput input) : SV_TARGET
{    
    // Sample cube map
    half3 vEnvColor = tCube.Sample( sCube, input.CubeTexCoord ).rgb; 
    return half4( vEnvColor, 1.0);
}
