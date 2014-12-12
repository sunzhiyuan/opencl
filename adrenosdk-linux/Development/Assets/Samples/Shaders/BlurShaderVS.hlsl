//--------------------------------------------------------------------------------------
// File: BlurShaderVS.hlsl
// Desc: Shaders for per-pixel lighting
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
struct VertexShaderInput
{
    float4 Vertex : POSITION;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

    vso.Position  = float4( input.Vertex.x, input.Vertex.y, 0.0, 1.0 );
    vso.TexCoord = float2( input.Vertex.z, input.Vertex.w );

    return vso;
}

