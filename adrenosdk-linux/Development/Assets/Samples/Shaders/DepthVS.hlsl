//--------------------------------------------------------------------------------------
// File: DepthVS.hlsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader for rendering to the shadow map
//--------------------------------------------------------------------------------------
cbuffer DepthConstantBuffer
{
    float4x4 matModelViewProj;
};

struct VertexShaderInput
{
    float4 vVertexPos : POSITION;
};

struct PixelShaderInput
{
    float4 vVertexPos : SV_POSITION;
    float4 vClipPos : TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;
    vso.vVertexPos = mul(matModelViewProj, float4(input.vVertexPos.xyz, 1.0));
    vso.vClipPos = vso.vVertexPos;    
    return vso;    
}