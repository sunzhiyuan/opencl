

//--------------------------------------------------------------------------------------
// File: PerPixelLightingVS.hlsl
// Desc: Shaders for per-pixel lighting
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
cbuffer PerPixelLightingConstantBuffer
{
    float4x4 matModelView;
    float4x4 matModelViewProj;
    float4x4 matNormal;
    float4   vAmbient;
    float4   vDiffuse;
    float4   vSpecular;
    float4   vLightPos;
};

struct VertexShaderInput
{
    float4 vPositionOS : POSITION;
    float3 vNormalOS : NORMAL;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float3 vNormalES : TEXCOORD0;
    float3 vViewVecES : TEXCOORD1;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

    float4 vPositionES = mul(matModelView, input.vPositionOS);
    float4 vPositionCS = mul(matModelViewProj, input.vPositionOS);

    // Transform object-space normals to eye-space
    float3 vNormalES = mul((float3x3)matNormal, input.vNormalOS);

    // Pass everything off to the fragment shader
    vso.vPosition  = vPositionCS;
    vso.vNormalES  = vNormalES.xyz;
    vso.vViewVecES = float3(0.0,0.0,0.0) - vPositionES.xyz;

    return vso;
}

