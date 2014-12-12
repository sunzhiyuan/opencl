

//--------------------------------------------------------------------------------------
// File: ShadowMapVS.hlsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2009 QUALCOMM Incorporated. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------

cbuffer ShadowMapConstantBuffer
{
    float4x4 matModelViewProj;
    float4x4 matShadow;
    float4x4 matScreenCoord;
    float4   vAmbient;
    float    fEpsilon;
};

struct VertexShaderInput
{
    float4 vVertex : POSITION;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float4 vShadowCoord : TEXCOORD0;
    float4 vScreenCoord : TEXCOORD1;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

    vso.vShadowCoord = mul(matShadow, input.vVertex);
    vso.vScreenCoord = mul(matScreenCoord, input.vVertex);
    vso.vPosition = mul(matModelViewProj, input.vVertex);

    return vso;
}

