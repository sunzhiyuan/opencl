

//--------------------------------------------------------------------------------------
// File: OverlayVS.hlsl
// Desc: Vertex shader for drawing a textured overlay
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
cbuffer OverlayConstantBuffer
{
    float4 vScreenSize;
};

struct VertexShaderInput
{
    float4 vVertexPos : POSITION;
};

struct PixelShaderInput
{
    float4 vVertexPos : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

PixelShaderInput main( VertexShaderInput input )
{
    PixelShaderInput vso;
    vso.vVertexPos.x = +( 2.0 * ( input.vVertexPos.x / vScreenSize.x ) - 1.0 );
    vso.vVertexPos.y = -( 2.0 * ( input.vVertexPos.y / vScreenSize.y ) - 1.0 );
    vso.vVertexPos.z = 0.0;
    vso.vVertexPos.w = 1.0;

    vso.vTexCoord.x = input.vVertexPos.z;
    vso.vTexCoord.y = input.vVertexPos.w;
    
    return vso;
}
