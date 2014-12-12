

//--------------------------------------------------------------------------------------
// File: MaterialShaderVS.hlsl
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
cbuffer MaterialConstantBuffer
{
    float4x4 MatModelViewProj;
    float4x4 MatModel;
    float4   LightPos;
    float4   EyePos;
    float4   AmbientColor;
};

struct VertexShaderInput
{
    float4 Position : POSITION;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float3 Normal : NORMAL;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 LightVec : TEXCOORD1;
    float3 ViewVec : TEXCOORD2;
    float3 Normal : TEXCOORD3;
    float  Depth : TEXCOORD4;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

    float4 Position  = input.Position;
    float2 TexCoord  = input.TexCoord;
    float3 Tangent   = input.Tangent;
    float3 Binormal  = input.Binormal;
    float3 Normal    = input.Normal;

    vso.Position = mul(MatModelViewProj,  Position);
    vso.TexCoord = TexCoord;
    vso.Normal   = Normal;

    float4 WorldPos = mul(MatModel, Position);
    float3 lightVec = LightPos.xyz - WorldPos.xyz;
    float3 viewVec  = EyePos.xyz   - WorldPos.xyz;

    // Transform vectors into tangent space
    vso.LightVec.x = dot( lightVec, Tangent);
    vso.LightVec.y = dot( lightVec, Binormal);
    vso.LightVec.z = dot( lightVec, Normal);
    vso.LightVec.w = saturate( 1.0 - 0.03 * dot( vso.LightVec.xyz, vso.LightVec.xyz ) );
    vso.LightVec.xyz = normalize(vso.LightVec.xyz);

    vso.ViewVec.x  = dot( viewVec, Tangent );
    vso.ViewVec.y  = dot( viewVec, Binormal );
    vso.ViewVec.z  = dot( viewVec, Normal );
    vso.ViewVec = normalize(vso.ViewVec);

    // Store depth for fragment shader
    // Depth is Z/W.  This is returned by the pixel shader on MRT1.    
    // Use 1 - z/w to get better precision
    vso.Depth = (1.0 - vso.Position.z / vso.Position.w);    
    return vso;
}

