

//--------------------------------------------------------------------------------------
// File: CelShadingPS.hlsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

Texture2D tCelShadingTexture : register(t0);
SamplerState sCelShadingTexture: register(s0);

Texture2D tTexture : register(t1);
SamplerState sTexture: register(s1);

cbuffer CelConstantBuffer
{
    float4 vLightPos;
    float4 vAmbient;
    float4x4 matModelViewProj;
    float4x4 matNormal;
};

struct PixelShaderInput
{
    float4 vPos : SV_POSITION;
    float3 vNormal : NORMAL0;
    float2 vTexCoord : TEXCOORD0;
    float  fDepth : TEXCOORD1;
};


struct PixelShaderOutput
{
    half4 vColor : SV_TARGET0;
    half4 vOther : SV_TARGET1;
};

PixelShaderOutput main(PixelShaderInput input)
{
    half vAngle            = max( 0.0, dot( normalize( input.vNormal ), normalize( vLightPos.xyz ) ) );
    half4 vCelShadingColor = tCelShadingTexture.Sample(sCelShadingTexture, float2( vAngle, 0.0 ) );
    half4 vTexColor        = tTexture.Sample( sTexture,  input.vTexCoord );
    
    PixelShaderOutput pso;
    
    pso.vColor = vCelShadingColor * ( vTexColor + half4( vAmbient.xyz, 0.0 ) );
    pso.vOther = half4( normalize( input.vNormal ), input.fDepth );
    return pso;
} 
