
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

cbuffer AverageLuminanceConstantBuffer
{
    float2 g_StepSize;
};

Texture2D tColorTexture : register(t0);
SamplerState sColorTexture : register(s0);

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    float fSum = 0.0;
    
    float2 StepSize = g_StepSize;

	fSum  = tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 1.5 * StepSize.x, 1.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 0.5 * StepSize.x, 1.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-0.5 * StepSize.x, 1.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-1.5 * StepSize.x, 1.5 * StepSize.y)).r;

	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 1.5 * StepSize.x, 0.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 0.5 * StepSize.x, 0.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-0.5 * StepSize.x, 0.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-1.5 * StepSize.x, 0.5 * StepSize.y)).r;

	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 1.5 * StepSize.x,-0.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 0.5 * StepSize.x,-0.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-0.5 * StepSize.x,-0.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-1.5 * StepSize.x,-0.5 * StepSize.y)).r;

	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 1.5 * StepSize.x,-1.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 0.5 * StepSize.x,-1.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-0.5 * StepSize.x,-1.5 * StepSize.y)).r;
	fSum += tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-1.5 * StepSize.x,-1.5 * StepSize.y)).r;
	
	// Average
	fSum *= (1.0 / (16.0));
	
 	return float4(fSum, fSum, fSum, fSum);
}
