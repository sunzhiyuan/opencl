//--------------------------------------------------------------------------------------
// File: BumpedReflectionMappingVS.hlsl
// Desc: Vertex Shader for the BumpedReflectionMapping sample
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
    float3 LightVec : TEXCOORD1;
    float3 ViewVec : TEXCOORD2;
    float3 CubeTexCoord : TEXCOORD3;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

    float4 Position  = input.Position;
    float2 TexCoord  = input.TexCoord;
    float3 Tangent   = mul( (float3x3)MatNormal, input.Tangent );
    float3 Binormal  = mul( (float3x3)MatNormal, input.Binormal );
    float3 Normal    = mul( (float3x3)MatNormal, input.Normal );

	vso.Position = mul(MatModelViewProj,  Position);
    vso.TexCoord = TexCoord;
    
    float4 WorldPos = mul(MatModel, Position);
    float3 lightVec = LightPos.xyz - WorldPos.xyz;
    float3 viewVec  = EyePos.xyz   - WorldPos.xyz;
    
    // Transform vectors into tangent space
    vso.LightVec.x = dot( lightVec, Tangent);
    vso.LightVec.y = dot( lightVec, Binormal);
    vso.LightVec.z = dot( lightVec, Normal);
    vso.LightVec.xyz = normalize(vso.LightVec.xyz);

    vso.ViewVec.x  = dot( viewVec, Tangent );
    vso.ViewVec.y  = dot( viewVec, Binormal );
    vso.ViewVec.z  = dot( viewVec, Normal );
    vso.ViewVec = normalize(vso.ViewVec);
        
    // Compute reflection vector in eye space
    float4 vPositionES = mul( MatModelView, input.Position );
    vso.CubeTexCoord = reflect( normalize( vPositionES.xyz/vPositionES.w ), Normal );

    return vso;	
}


