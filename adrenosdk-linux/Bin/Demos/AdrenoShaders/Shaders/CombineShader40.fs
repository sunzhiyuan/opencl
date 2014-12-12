//--------------------------------------------------------------------------------------
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
// This no longer supported on Droid.
// #extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



// Enable supported extensions
#extension GL_OES_texture_3D : enable


varying vec2 g_TexCoord;
varying vec3   g_vFreqTexCoords;

uniform sampler2D g_SharpTexture;
uniform sampler2D g_BlurredTexture;
uniform sampler3D g_Rand3DTexture;

uniform float g_BloomIntensity;
uniform float g_Contrast;
uniform float g_Saturation;
uniform vec3 g_ColorCorrect;
uniform vec3 g_ColorAdd;


void main()
{
    vec4 FocusPixel = texture2D( g_SharpTexture, g_TexCoord );
    vec4 BlurPixel = texture2D( g_BlurredTexture, g_TexCoord );
    
    vec4 Color = clamp(FocusPixel + (BlurPixel * g_BloomIntensity), 0.0, 1.0);
 //   gl_FragColor =  Color;

//	float rand = texture3D(g_Rand3DTexture, g_vFreqTexCoords).r; 
	
    vec3 IntensityConverter = vec3(0.299, 0.587, 0.114); 
	float Y = dot(IntensityConverter, Color.xyz); 

	vec4 sepiaConvert = vec4(0.191, -0.054,-0.221,0.0); 
    
 //   gl_FragColor = vec4(rand, rand, rand, rand) * (Y + sepiaConvert);
    gl_FragColor =  (Y + sepiaConvert);
}
