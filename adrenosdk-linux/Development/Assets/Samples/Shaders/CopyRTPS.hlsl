
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
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
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

Texture2D tColorTexture : register(t0);
SamplerState sColorTexture : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
	float onePixel = tColorTexture.Sample( sColorTexture, float2( 0.0, 0.0)).r;
 	return float4(onePixel, onePixel, onePixel, onePixel);
}
