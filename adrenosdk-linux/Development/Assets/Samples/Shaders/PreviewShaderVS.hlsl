

//--------------------------------------------------------------------------------------
// File: PreviewShaderVS.glsl
// Desc: Preview shader
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
struct VertexShaderInput
{
    float4 vVertexPos : POSITION;
};

struct PixelShaderInput
{
    float4 vVertexPos : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};


PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

    vso.vVertexPos = float4( input.vVertexPos.x, input.vVertexPos.y, 0.0, 1.0 );
    vso.vTexCoord = float2( input.vVertexPos.z, input.vVertexPos.w );

    return vso;
}
