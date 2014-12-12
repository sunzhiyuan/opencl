

//--------------------------------------------------------------------------------------
// File: MaterialShaderPS.hlsl
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
cbuffer MaterialConstantBuffer
{
    float4x4 MatModelViewProj;
    float4x4 MatModel;
    float4   LightPos;
    float4   EyePos;
    float4   AmbientColor;
};

Texture2D tDiffuseTexture : register(t0);
SamplerState sDiffuseTexture : register(s0);

Texture2D tBumpTexture : register(t1);
SamplerState sBumpTexture : register(s1);

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 LightVec : TEXCOORD1;
    float3 ViewVec : TEXCOORD2;
    float3 Normal : TEXCOORD3;
    float  Depth : TEXCOORD4;
};

struct PixelShaderOutput
{
    half4 vColor : SV_TARGET0;
    float4 vDepth : SV_TARGET1;
};


PixelShaderOutput main(PixelShaderInput input)
{
    half4 vBaseColor = tDiffuseTexture.Sample( sDiffuseTexture, input.TexCoord );
        
    half3 vNormal;
    vNormal.xyz = tBumpTexture.Sample( sBumpTexture, input.TexCoord).xyz * 2.0 - 1.0;

    // Standard Phong lighting
    half3 vHalf     = normalize( input.LightVec.xyz + input.ViewVec );
    half fDiffuse  = saturate( dot( input.LightVec.xyz, vNormal ) );
    
    half fSpecular = pow( saturate( dot( vHalf, vNormal ) ), 20.0 );

    PixelShaderOutput pso;
    pso.vColor = saturate(half4(AmbientColor.x * vBaseColor.x, AmbientColor.x * vBaseColor.x, AmbientColor.x * vBaseColor.x, 0.0) + 
                          half4(fDiffuse * vBaseColor.x + fSpecular, fDiffuse * vBaseColor.y + fSpecular, fDiffuse * vBaseColor.z + fSpecular, 0.0) * input.LightVec.w);
    // Write fragment depth to second MRT
    pso.vDepth = float4(input.Depth, 0.0, 0.0, 1.0);
    return pso;
}
