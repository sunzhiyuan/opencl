

//--------------------------------------------------------------------------------------
// File: ShadowMapPS.glsl
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
Texture2D tShadowMap : register(t0);
SamplerState sShadowMap : register(s0);

Texture2D tDiffuseSpecularMap : register(t1);
SamplerState sDiffuseSpecularMap : register(s1);

cbuffer ShadowMapConstantBuffer
{
    float4x4 matModelViewProj;
    float4x4 matShadow;
    float4x4 matScreenCoord;
    float4   vAmbient;
    float    fEpsilon;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float4 vShadowCoord : TEXCOORD0;
    float4 vScreenCoord : TEXCOORD1;
};

half4 main(PixelShaderInput input) : SV_TARGET
{
    half fLightDepth = tShadowMap.Sample( sShadowMap, input.vShadowCoord.xy / input.vShadowCoord.w).r;

    // Map stores 1 - (z/w) so invert to get (z/w)
    fLightDepth = 1.0 - fLightDepth;

    // Add a small bias to get rid of shadow acne
    half fBias = 0.01; 
    fLightDepth += fBias;

    half4 vShadowColor;
    if( input.vShadowCoord.z / input.vShadowCoord.w > fLightDepth )
    {
        vShadowColor = half4( 0.0, 0.0, 0.0, 1.0 );
    }
    else
    {
        vShadowColor = half4( 1.0, 1.0, 1.0, 1.0 );
    }

    half4 vDiffuseAndSpecular = tDiffuseSpecularMap.Sample( sDiffuseSpecularMap, input.vScreenCoord.xy / input.vScreenCoord.w );
	half4 outColor = half4( vAmbient.xyz, 0.0 ) + vShadowColor * vDiffuseAndSpecular;
    return outColor;
}

