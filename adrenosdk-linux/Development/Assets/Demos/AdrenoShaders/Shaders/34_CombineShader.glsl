//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[CombineShader34VS]
#include "CommonVS.glsl"

attribute vec4 g_Vertex;

varying vec2 g_TexCoord;


void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_Vertex.z, g_Vertex.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[CombineShader34FS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_SharpTexture;
uniform sampler2D g_CurrentLuminance;
uniform sampler2D g_BloomTexture;

uniform float MiddleGray;
uniform float White;


void main()
{
    vec3 FocusPixel = texture2D( g_SharpTexture, g_TexCoord ).rgb;
    vec3 Bloom = texture2D( g_BloomTexture, g_TexCoord ).rgb;
    
	// Current adaption level of luminance (1x1 texture)
	float AdaptedLum = clamp(texture2D( g_CurrentLuminance, vec2( 0.0, 0.0)).r, 0.1, 1.0);

    // add the bloom render target
    vec3 sample_bloom = clamp(FocusPixel + Bloom, 0.0, 1.0);
    
	// RGB -> XYZ conversion
	// http://www.w3.org/Graphics/Color/sRGB
	// The official sRGB to XYZ conversion matrix is (following ITU-R BT.709)
	// 0.4125 0.3576 0.1805
	// 0.2126 0.7152 0.0722
	// 0.0193 0.1192 0.9505	
	const mat3 RGB2XYZ = mat3(0.5141364, 0.3238786,  0.16036376,
							 0.265068,  0.67023428, 0.06409157,
							 0.0241188, 0.1228178,  0.84442666);				            
	// Make sure none of XYZ can be zero
	// This prevents temp from ever being zero, or Yxy from ever being zero        
	vec3 XYZ = (RGB2XYZ * sample_bloom) + vec3(0.000001, 0.000001, 0.000001);	
	
	// XYZ -> Yxy conversion
	vec3 Yxy;
	Yxy.r = XYZ.g;									// copy luminance Y

	float temp = dot(vec3(1.0,1.0,1.0), XYZ.rgb); 
	//Yxy.g = XYZ.r / temp;  							// x = X / (X + Y + Z)
	//Yxy.b  = XYZ.g / temp; 							// y = X / (X + Y + Z)	
	Yxy.gb = XYZ.rg / temp;
	
	// (Lp) Map average luminance to the middlegrey zone by scaling pixel luminance
	// raise the value range a bit ...
	float Lp = Yxy.r * MiddleGray / (AdaptedLum + 0.001);    

	// (Ld) Scale all luminance within a displayable range of 0 to 1
	Yxy.r = (Lp  * (1.0 + Lp/White))/(1.0 + Lp);	

	// Yxy -> XYZ conversion
	XYZ.r = Yxy.r * Yxy.g / Yxy.b;               // X = Y * x / y
	XYZ.g = Yxy.r;                                // copy luminance Y
	XYZ.b = Yxy.r * (1.0 - Yxy.g - Yxy.b) / Yxy.b;  // Z = Y * (1-x-y) / y

	// XYZ -> RGB conversion
	// The official XYZ to sRGB conversion matrix is (following ITU-R BT.709)
	//  3.2410 -1.5374 -0.4986
	// -0.9692  1.8760  0.0416
	//  0.0556 -0.2040  1.0570		
	const mat3 XYZ2RGB  = mat3(2.5651,-1.1665,-0.3986,
							  -1.0217, 1.9777, 0.0439, 
							   0.0753, -0.2543, 1.1892);
#if 0
	// Night Tone mapper: for scotopic images
	// reduces brightness and contrast, desaturates the image, adds a blue shift and reduces visual acuity 
	// 
	// Pseudo: Y * (1.33 * (1+((Y+Z)/X)-1.68)
	float V = XYZ.g *(1.33*(1.0+((XYZ.g+XYZ.b) / XYZ.r)-1.68));
	vec3 NightColor = V * BlueShiftColor.rgb;
	sample_bloom = lerp(NightColor, mul(XYZ2RGB, XYZ), saturate(Yxy.r * BlueShiftParams.r + (1.0 - AdaptedLum.x) * BlueShiftParams.b + BlueShiftParams.g));
#else    
	sample_bloom = (XYZ2RGB * XYZ);	
#endif
		
	gl_FragColor = clamp(vec4(sample_bloom.r, sample_bloom.g, sample_bloom.b, 1.0), 0.0, 1.0);
}
