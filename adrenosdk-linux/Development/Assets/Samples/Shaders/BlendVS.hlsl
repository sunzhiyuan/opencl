//--------------------------------------------------------------------------------------
// File: BlendVS.hlsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//
// Simple 2D Texture Blending
//
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
cbuffer BlendConstantBuffer
{
    float fHalfStepSizeX;
    float fHalfStepSizeY;
    float2 Padding;
};

struct VertexShaderInput
{
    float4 vVertex : POSITION;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoords  : TEXCOORD0;
    float2 vTexCoords0 : TEXCOORD1;
    float2 vTexCoords1 : TEXCOORD2;
    float2 vTexCoords2 : TEXCOORD3;
    float2 vTexCoords3 : TEXCOORD4;
};

PixelShaderInput main( VertexShaderInput input )
{
    PixelShaderInput vso;
    vso.vPosition  = float4( input.vVertex.x, input.vVertex.y, 0.5, 1.0 );
    vso.vTexCoords = float2( input.vVertex.z, input.vVertex.w );
    
    vso.vTexCoords0 = vso.vTexCoords + float2( -fHalfStepSizeX, -fHalfStepSizeY );
    vso.vTexCoords1 = vso.vTexCoords + float2(  fHalfStepSizeX, -fHalfStepSizeY );
    vso.vTexCoords2 = vso.vTexCoords + float2( -fHalfStepSizeX,  fHalfStepSizeY );
    vso.vTexCoords3 = vso.vTexCoords + float2(  fHalfStepSizeX,  fHalfStepSizeY );

    return vso;
}
