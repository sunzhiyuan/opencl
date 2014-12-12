
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
cbuffer GenerateLuminanceConstantBuffer
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

half4 main(PixelShaderInput input) : SV_TARGET
{
    // used to prevent black pixels
    const half Delta = 0.0001;  
    
	// luminance values            
	// amplify luminance in this scene
	const half3 LUMINANCE = half3(0.2125, 0.7154, 0.0721)  * 20.0;      
    
    half fLogLumSum = 0.0;
    float2 StepSize = g_StepSize;

	fLogLumSum = log(dot(tColorTexture.Sample( sColorTexture, input.TexCoord + float2(StepSize.x,  StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(tColorTexture.Sample( sColorTexture, input.TexCoord + float2(StepSize.x,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(tColorTexture.Sample( sColorTexture, input.TexCoord + float2(StepSize.x, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 0, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 0,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(tColorTexture.Sample( sColorTexture, input.TexCoord + float2( 0,  StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-StepSize.x, -StepSize.y)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-StepSize.x,  0)).rgb, LUMINANCE) + Delta);
	fLogLumSum += log(dot(tColorTexture.Sample( sColorTexture, input.TexCoord + float2(-StepSize.x,  StepSize.y)).rgb, LUMINANCE) + Delta);

	// Average
	fLogLumSum *= (1.0 / (9.0));
	
 	return half4(fLogLumSum, fLogLumSum, fLogLumSum, fLogLumSum);
}
