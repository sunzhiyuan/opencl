
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

cbuffer AdadptLuminanceConstantBuffer
{
    float ElapsedTime;
    float Lambda;
};

Texture2D tCurrentLuminance : register(t0);
SamplerState sCurrentLuminance : register(s0);

Texture2D tPreviousLuminance : register(t1);
SamplerState sPreviousLuminance : register(s1);

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	// fetch current luminance value
	float CurrentLum = tCurrentLuminance.Sample( sCurrentLuminance, float2( 0.0, 0.0)).r;

	// fetch previous luminance value
	float PreviousLum = tPreviousLuminance.Sample( sPreviousLuminance, float2( 0.0, 0.0)).r;
	
	float Lum = (PreviousLum + (CurrentLum - PreviousLum)  * ( pow( 0.98, Lambda * ElapsedTime) ));

    // set exposure limits
    Lum = clamp( Lum, 0.0, 5.0 );

	return float4(Lum, Lum, Lum, Lum);
}
