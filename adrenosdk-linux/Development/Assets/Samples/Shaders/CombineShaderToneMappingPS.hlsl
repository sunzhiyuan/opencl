
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
cbuffer CombineShaderConstantBuffer
{
    float MiddleGray;
    float White;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

Texture2D tSharpTexture : register(t0);
SamplerState sSharpTexture : register(s0);

Texture2D tCurrentLuminance : register(t1);
SamplerState sCurrentLuminance : register(s1);

Texture2D tBloomTexture : register(t2);
SamplerState sBloomTexture : register(s2);

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 ScreenPixel = tSharpTexture.Sample( sSharpTexture, input.TexCoord );
    float4 Bloom = tBloomTexture.Sample( sBloomTexture, input.TexCoord );
    
	// Current adaption level of luminance (1x1 texture)
	float AdaptedLum = clamp( tCurrentLuminance.Sample( sCurrentLuminance, float2( 0.0, 0.0 ) ).r, 0.1, 0.7 );
    
    // add the bloom render target
    float4 sample = clamp( ScreenPixel + Bloom, 0.0, 5.0 );

	// RGB -> XYZ conversion
	// http://www.w3.org/Graphics/Color/sRGB
	// The official sRGB to XYZ conversion matrix is (following ITU-R BT.709)
	// 0.4125 0.3576 0.1805
	// 0.2126 0.7152 0.0722
	// 0.0193 0.1192 0.9505	
	const float3x3 RGB2XYZ = float3x3( 0.5141364, 0.3238786,  0.16036376,
							   0.265068,  0.67023428, 0.06409157,
							   0.0241188, 0.1228178,  0.84442666);				            
	// Make sure none of XYZ can be zero
	// This prevents temp from ever being zero, or Yxy from ever being zero        
	float3 XYZ = ( mul( RGB2XYZ,  sample.rgb ) ) + float3( 0.000001, 0.000001, 0.000001 );
	
	// XYZ -> Yxy conversion
	float3 Yxy;
	Yxy.r = XYZ.g;									// copy luminance Y

	float temp = dot( float3( 1.0, 1.0, 1.0 ), XYZ.rgb );
	//Yxy.g = XYZ.r / temp;  							// x = X / (X + Y + Z)
	//Yxy.b  = XYZ.g / temp; 							// y = X / (X + Y + Z)	
	Yxy.gb = XYZ.rg / temp;
	
	// (Lp) Map average luminance to the middlegrey zone by scaling pixel luminance
	// raise the value range a bit ...
	float Lp = Yxy.r * MiddleGray / ( AdaptedLum + 0.001 );    

	// (Ld) Scale all luminance within a displayable range of 0 to 1
	Yxy.r = ( Lp  * ( (1.0 + Lp) / White ) ) / ( 1.0 + Lp );

	// Yxy -> XYZ conversion
	XYZ.r = Yxy.r * Yxy.g / Yxy.b;               // X = Y * x / y
	XYZ.g = Yxy.r;                                // copy luminance Y
	XYZ.b = Yxy.r * ( 1.0 - Yxy.g - Yxy.b ) / Yxy.b;  // Z = Y * (1-x-y) / y

	// XYZ -> RGB conversion
	// The official XYZ to sRGB conversion matrix is (following ITU-R BT.709)
	//  3.2410 -1.5374 -0.4986
	// -0.9692  1.8760  0.0416
	//  0.0556 -0.2040  1.0570		
	const float3x3 XYZ2RGB  = float3x3( 2.5651, -1.1665, -0.3986,
							   -1.0217,  1.9777,  0.0439, 
							    0.0753, -0.2543,  1.1892);
#if 0
	// Night Tone mapper: for scotopic images
	// reduces brightness and contrast, desaturates the image, adds a blue shift and reduces visual acuity 
	// 
	// Pseudo: Y * (1.33 * (1+((Y+Z)/X)-1.68)
	float V = XYZ.g *(1.33*(1.0+((XYZ.g+XYZ.b) / XYZ.r)-1.68));
	float3 NightColor = V * BlueShiftColor.rgb;
	sample.rgb = lerp(NightColor, mul(XYZ2RGB, XYZ), saturate(Yxy.r * BlueShiftParams.r + (1.0 - AdaptedLum.x) * BlueShiftParams.b + BlueShiftParams.g));
#else    
	sample.rgb = mul( XYZ2RGB,  XYZ );
	// cheap gamma correction with an artistic curve	
	sample.rgb = pow(mul( XYZ2RGB, XYZ), float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));	
#endif
	
	/*
	float c = 0.0031308;
	float cf = 12.92;
	float a = 0.055;
	float e = 0.41666;
	
	float3 finalColor;
	finalColor.r = (sample.r<c)? cf*sample.r : (1.0+a)*pow(sample.r, e)-a;
	finalColor.g = (sample.g<c)? cf*sample.g : (1.0+a)*pow(sample.g, e)-a;
	finalColor.b = (sample.b<c)? cf*sample.b : (1.0+a)*pow(sample.b, e)-a;
	
	gl_FragColor = clamp(float4(finalColor.r, finalColor.g, finalColor.b, 1.0), 0.0, 1.0);
	*/
	
	return clamp(float4(sample.r, sample.g, sample.b, 1.0), 0.0, 1.0);
}
