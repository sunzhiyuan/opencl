//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
cbuffer AppConstantBuffer
{
    float4x4 matModelViewProj;    
};


struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 tex : TEXCOORD0;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 normal: NORMAL;
    float2 tex : TEXCOORD0;
};

PixelShaderInput main( VertexShaderInput input )
{
    PixelShaderInput vertexShaderOutput;
 
	vertexShaderOutput.pos = mul(matModelViewProj, float4(input.pos, 1.0f));
    vertexShaderOutput.tex = input.tex;
    vertexShaderOutput.normal = input.normal;

    return vertexShaderOutput;
}