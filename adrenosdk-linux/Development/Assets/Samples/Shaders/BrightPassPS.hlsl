
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
cbuffer BrightPassConstantBuffer
{
    float MiddleGray;
    float BrightPassWhite;
    float BrightPassThreshold;
    float BrightPassOffset;
};

Texture2D tScratchTexture : register(t0);
SamplerState sScratchTexture : register(s0);

Texture2D tCurrentLuminance : register(t1);
SamplerState sCurrentLuminance : register(s1);

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	// fetch scratch render target
	float4 Sample = tScratchTexture.Sample( sScratchTexture, input.TexCoord);
	
    // Current adaption level of luminance (1x1 texture)
	float AdaptedLum = clamp(tCurrentLuminance.Sample( sCurrentLuminance, float2( 0.0, 0.0)).r, 0.15, 1.0);
    
	// RGB -> XYZ conversion
	const float3x3 RGB2XYZ = float3x3(0.5141364, 0.3238786,  0.16036376,
							 0.265068,  0.67023428, 0.06409157,
							 0.0241188, 0.1228178,  0.84442666);	
							 			                    
	float3 XYZ = mul( RGB2XYZ, Sample.rgb );
	
	// XYZ -> Yxy conversion
	float3 Yxy;
	Yxy.r = XYZ.g;                            // copy luminance Y
	
	// prevent division by zero to keep the compiler happy ... strange
	float temp = dot(float3(1.0,1.0,1.0), XYZ.rgb) + 0.000001; 
	//Yxy.g = XYZ.r / temp; // x = X / (X + Y + Z)
	//Yxy.b  = XYZ.g / temp; // y = X / (X + Y + Z)	
	Yxy.gb = XYZ.rg / temp;
	
    // (Lp) Map average luminance to the middlegrey zone by scaling pixel luminance
    // raise the value range a bit ...
	float Lp = Yxy.r * MiddleGray / (AdaptedLum + 0.001);    

	// (Ld) Scale all luminance within a displayable range of 0 to 1
	// new bright pass white value
	Yxy.r = (Lp  * (1.0 + Lp /(BrightPassWhite))) ;	

	Yxy.r -= BrightPassThreshold;
	
	Yxy.r = max(Yxy.r, 0.0);
    Yxy.r /= (BrightPassOffset + Yxy.r);

	// Yxy -> XYZ conversion
	XYZ.r = Yxy.r * Yxy.g / Yxy. b;               // X = Y * x / y
	XYZ.g = Yxy.r;                                // copy luminance Y
	XYZ.b = Yxy.r * (1.0 - Yxy.g - Yxy.b) / Yxy.b;  // Z = Y * (1-x-y) / y

	// XYZ -> RGB conversion
	const float3x3 XYZ2RGB  = float3x3(2.5651,-1.1665,-0.3986,
							  -1.0217, 1.9777, 0.0439, 
							   0.0753, -0.2543, 1.1892);
							   
	Sample.rgb = clamp(mul(XYZ2RGB, XYZ), 0.0, 1.0);	    

	return float4(Sample.x, Sample.y, Sample.z, 1.0);
}
