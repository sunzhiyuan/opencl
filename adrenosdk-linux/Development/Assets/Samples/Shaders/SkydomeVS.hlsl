

//--------------------------------------------------------------------------------------
// File: SkydomeVS.hlsl
// Desc: Vertex Shader for Skydome in the BumpedReflectionMapping sample
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
cbuffer SkydomeConstantBuffer
{
    float4x4 MatModelViewProj;
    float4x4 MatNormal;
};


struct VertexShaderInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 CubeTexCoord : TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

    float4 Position  = float4( input.Position, 1.0 );
    
	vso.Position = mul( MatModelViewProj,  Position );
    vso.CubeTexCoord = mul( (float3x3)MatNormal, input.Normal );
        
    return vso;	
}


