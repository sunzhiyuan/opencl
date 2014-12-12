
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

#version 300 es

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



uniform sampler2D g_ScratchTexture;
uniform sampler2D g_CurrentLuminance;
in vec2 g_TexCoord;
uniform float MiddleGray;
uniform float BrightPassWhite;
uniform float BrightPassThreshold;
uniform float BrightPassOffset;

out vec4 g_FragColor;

void main()
{
	// fetch scratch render target
	vec4 Sample_texture = texture( g_ScratchTexture, g_TexCoord);
	
    // Current adaption level of luminance (1x1 texture)
	float AdaptedLum = clamp(texture( g_CurrentLuminance, vec2( 0.0, 0.0)).r, 0.15, 1.0);
    
	// RGB -> XYZ conversion
	const mat3 RGB2XYZ = mat3(0.5141364, 0.3238786,  0.16036376,
							 0.265068,  0.67023428, 0.06409157,
							 0.0241188, 0.1228178,  0.84442666);	
							 			                    
	vec3 XYZ = RGB2XYZ * Sample_texture.rgb;	
	
	// XYZ -> Yxy conversion
	vec3 Yxy;
	Yxy.r = XYZ.g;                            // copy luminance Y
	
	// prevent division by zero to keep the compiler happy ... strange
	float temp = dot(vec3(1.0,1.0,1.0), XYZ.rgb) + 0.000001; 
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
	const mat3 XYZ2RGB  = mat3(2.5651,-1.1665,-0.3986,
							  -1.0217, 1.9777, 0.0439, 
							   0.0753, -0.2543, 1.1892);
							   
	Sample_texture.rgb = clamp((XYZ2RGB * XYZ), 0.0, 1.0);	    

	g_FragColor = vec4(Sample_texture.x, Sample_texture.y, Sample_texture.z, 1.0);
}
