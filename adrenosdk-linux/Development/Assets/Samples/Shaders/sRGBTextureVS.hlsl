//--------------------------------------------------------------------------------------
// File: sRGBTextureVS.hlsl
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader.
//--------------------------------------------------------------------------------------

struct VertexShaderInput
{
    float4 vPosition: POSITION;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;
    vso.vPosition = float4( input.vPosition.x, input.vPosition.y, 0.0, 1.0 );
    vso.vTexCoord = float2( input.vPosition.z, input.vPosition.w );

    return vso;
}
