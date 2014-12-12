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
[DownsizeShader40VS]
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
[DownsizeShader40FS]
#include "CommonFS.glsl"

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
