//--------------------------------------------------------------------------------------
// File: DownsizeShader.glsl
// Desc: Shaders for per-pixel lighting
//
// Author:                 QUALCOMM, Adreno SDK
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

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;

void main()
{
    vec2 StepSize = g_StepSize;
    
    vec4 fSum = vec4( 0.0 );

	fSum  = texture2D( g_ColorTexture, g_TexCoord + vec2( 0.5 * StepSize.x, 0.5 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-0.5 * StepSize.x, 0.5 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-0.5 * StepSize.x,-0.5 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 0.5 * StepSize.x,-0.5 * StepSize.y));
/*
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 2.0 * StepSize.x, 1.0 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 1.0 * StepSize.x, 1.0 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-1.0 * StepSize.x, 1.0 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-2.0 * StepSize.x, 1.0 * StepSize.y));

	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 2.0 * StepSize.x,-1.0 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 1.0 * StepSize.x,-1.0 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-1.0 * StepSize.x,-1.0 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-2.0 * StepSize.x,-1.0 * StepSize.y));

	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 2.0 * StepSize.x,-2.0 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 1.0 * StepSize.x,-2.0 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-1.0 * StepSize.x,-2.0 * StepSize.y));
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-2.0 * StepSize.x,-2.0 * StepSize.y));
*/
	// Average
	fSum *= (1.0 / (4.0));
	
 	gl_FragColor = fSum;
}
