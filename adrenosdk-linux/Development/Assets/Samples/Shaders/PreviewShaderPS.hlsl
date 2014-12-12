

//--------------------------------------------------------------------------------------
// File: PreviewShaderPS.glsl
// Desc: Preview shader
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

  
//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

struct PixelShaderInput
{
    float4 vVertexPos : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
};

Texture2D colorTexture : register(t0);
SamplerState colorSampler: register(s0);

float4 main(PixelShaderInput input ) : SV_TARGET
{
    return colorTexture.Sample(colorSampler, input.vTexCoord );     
}
