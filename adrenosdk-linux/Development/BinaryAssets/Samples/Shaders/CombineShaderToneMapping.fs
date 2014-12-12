
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// File: CommonFS.glsl
// Desc: Useful common shader code for fragment shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

// default to medium precision
precision mediump float;

// OpenGL ES require that precision is defined for a fragment shader
// usage example: varying NEED_HIGHP vec2 vLargeTexCoord;
#ifdef GL_FRAGMENT_PRECISION_HIGH
   #define NEED_HIGHP highp
#else
   #define NEED_HIGHP mediump
#endif

// Enable supported extensions
#extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



varying vec2 g_TexCoord;

uniform sampler2D g_SharpTexture;
uniform sampler2D g_CurrentLuminance;
uniform sampler2D g_BloomTexture;

uniform float MiddleGray;
uniform float White;


void main()
{
    vec4 ScreenPixel = texture2D( g_SharpTexture, g_TexCoord );
    vec4 Bloom = texture2D( g_BloomTexture, g_TexCoord );
    
	// Current adaption level of luminance (1x1 texture)
	float AdaptedLum = clamp( texture2D( g_CurrentLuminance, vec2( 0.0, 0.0 ) ).r, 0.1, 0.7 );
    
    // add the bloom render target
    vec4 sample_bloom = clamp( ScreenPixel + Bloom, 0.0, 5.0 );

	// RGB -> XYZ conversion
	// http://www.w3.org/Graphics/Color/sRGB
	// The official sRGB to XYZ conversion matrix is (following ITU-R BT.709)
	// 0.4125 0.3576 0.1805
	// 0.2126 0.7152 0.0722
	// 0.0193 0.1192 0.9505	
	const mat3 RGB2XYZ = mat3( 0.5141364, 0.3238786,  0.16036376,
							   0.265068,  0.67023428, 0.06409157,
							   0.0241188, 0.1228178,  0.84442666);				            
	// Make sure none of XYZ can be zero
	// This prevents temp from ever being zero, or Yxy from ever being zero        
	vec3 XYZ = ( RGB2XYZ * sample_bloom.rgb ) + vec3( 0.000001, 0.000001, 0.000001 );
	
	// XYZ -> Yxy conversion
	vec3 Yxy;
	Yxy.r = XYZ.g;									// copy luminance Y

	float temp = dot( vec3( 1.0, 1.0, 1.0 ), XYZ.rgb );
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
	const mat3 XYZ2RGB  = mat3( 2.5651, -1.1665, -0.3986,
							   -1.0217,  1.9777,  0.0439, 
							    0.0753, -0.2543,  1.1892);
#if 0
	// Night Tone mapper: for scotopic images
	// reduces brightness and contrast, desaturates the image, adds a blue shift and reduces visual acuity 
	// 
	// Pseudo: Y * (1.33 * (1+((Y+Z)/X)-1.68)
	float V = XYZ.g *(1.33*(1.0+((XYZ.g+XYZ.b) / XYZ.r)-1.68));
	vec3 NightColor = V * BlueShiftColor.rgb;
	sample_bloom.rgb = lerp(NightColor, mul(XYZ2RGB, XYZ), saturate(Yxy.r * BlueShiftParams.r + (1.0 - AdaptedLum.x) * BlueShiftParams.b + BlueShiftParams.g));
#else    
	sample_bloom.rgb = (XYZ2RGB * XYZ);
	// cheap gamma correction with an artistic curve	
	sample_bloom.rgb = pow((XYZ2RGB * XYZ), vec3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));	
#endif
	
	/*
	float c = 0.0031308;
	float cf = 12.92;
	float a = 0.055;
	float e = 0.41666;
	
	vec3 finalColor;
	finalColor.r = (sample.r<c)? cf*sample.r : (1.0+a)*pow(sample.r, e)-a;
	finalColor.g = (sample.g<c)? cf*sample.g : (1.0+a)*pow(sample.g, e)-a;
	finalColor.b = (sample.b<c)? cf*sample.b : (1.0+a)*pow(sample.b, e)-a;
	
	gl_FragColor = clamp(vec4(finalColor.r, finalColor.g, finalColor.b, 1.0), 0.0, 1.0);
	*/
	
	gl_FragColor = clamp(vec4(sample_bloom.r, sample_bloom.g, sample_bloom.b, 1.0), 0.0, 1.0);
}
