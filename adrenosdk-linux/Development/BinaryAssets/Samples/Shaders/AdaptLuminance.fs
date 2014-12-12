
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



uniform sampler2D g_CurrentLuminance;
uniform sampler2D g_PreviousLuminance;
varying vec2 g_TexCoord;
uniform float ElapsedTime;
uniform float Lambda;


void main()
{
	// fetch current luminance value
	float CurrentLum = texture2D( g_CurrentLuminance, vec2( 0.0, 0.0)).r;

	// fetch previous luminance value
	float PreviousLum = texture2D( g_PreviousLuminance, vec2( 0.0, 0.0)).r;
	
	float Lum = (PreviousLum + (CurrentLum - PreviousLum)  * ( pow( 0.98, Lambda * ElapsedTime) ));

    // set exposure limits
    Lum = clamp( Lum, 0.0, 5.0 );

	gl_FragColor = vec4(Lum, Lum, Lum, Lum);
}
