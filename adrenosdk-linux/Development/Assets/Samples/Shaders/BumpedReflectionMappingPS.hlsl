//--------------------------------------------------------------------------------------
// File: BumpedReflectionMappingPS.hlsl
// Desc: Pixel Shader for the BumpedReflectionMapping sample
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

Texture2D tDiffuse : register(t0);
SamplerState sDiffuse : register(s0);

Texture2D tNormal : register(t1);
SamplerState sNormal: register(s1);

TextureCube tCube : register(t2);
SamplerState sCube: register(s2);

cbuffer BumpedReflectionConstantBuffer
{
    float4x4 MatModelViewProj;
    float4x4 MatModelView;
    float4x4 MatModel;    
    float4x4 MatNormal;
    float4   LightPos;
    float4   EyePos;
    float    FresnelPower;
    float    SpecularPower;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 LightVec : TEXCOORD1;
    float3 ViewVec : TEXCOORD2;
    float3 CubeTexCoord : TEXCOORD3;
};


half4 main(PixelShaderInput input) : SV_TARGET
{
    // Sample diffuse map
    half4 vBaseColor = tDiffuse.Sample( sDiffuse, input.TexCoord );
    
    // Sample environment map
    half4 vEnvColor = tCube.Sample( sCube, input.CubeTexCoord );
    
    half3 L = input.LightVec;
    half3 V = input.ViewVec;
    half3 H = normalize( L + V );
        
    // Sample normalmap and map from [0,1] -> [-1, 1]        
    half3 N = tNormal.Sample( sNormal, input.TexCoord).xyz * 2.0 - 1.0;

    // Compute Reflection Vector
    half3 R = reflect( -V, N );
    half3 RH = normalize( R + V );
    
    // Compute Fresnel term
    half fFresnel = pow(1.0 + dot(-V, RH), FresnelPower );

    // Attenuate reflection color by fresnel term
    half3 vReflectionColor = fFresnel * vEnvColor.rgb;

    
    // Diffuse and specular
    half dotLN = saturate( dot( L, N ) );        
    half fDiffuse  = dotLN;
    half fSpecular = pow( saturate( dot( H, N ) ), SpecularPower );


    return half4( vReflectionColor + fDiffuse * vBaseColor.rgb + fSpecular, 1.0);
}
