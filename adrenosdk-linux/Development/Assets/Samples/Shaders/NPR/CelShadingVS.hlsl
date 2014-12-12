

//--------------------------------------------------------------------------------------
// File: CelShadingVS.hlsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
cbuffer CelConstantBuffer
{
    float4 vLightPos;
    float4 vAmbient;
    float4x4 matModelViewProj;
    float4x4 matNormal;
};

struct VertexShaderInput
{
    float4 vVertex : POSITION;
    float3 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
};

struct PixelShaderInput
{
    float4 vPos : SV_POSITION;
    float3 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float  fDepth : TEXCOORD1;
};


PixelShaderInput main( VertexShaderInput input )
{
    PixelShaderInput vso;
    vso.vPos = mul(matModelViewProj, input.vVertex);
    vso.vNormal = mul((float3x3)matNormal, input.vNormal);
    vso.vTexCoord = input.vTexCoord;
    vso.fDepth = vso.vPos.z / vso.vPos.w;       

    return vso;
}
