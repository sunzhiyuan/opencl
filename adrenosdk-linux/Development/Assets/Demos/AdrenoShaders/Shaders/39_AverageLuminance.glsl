//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[AverageLuminance39VS]
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
[AverageLuminance39FS]
#include "CommonFS.glsl"

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
varying vec2 g_TexCoord;

void main()
{
    float fSum = 0.0;
    
    vec2 StepSize = g_StepSize;

	fSum  = texture2D( g_ColorTexture, g_TexCoord + vec2( 2.0 * StepSize.x, 2.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 1.0 * StepSize.x, 2.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-1.0 * StepSize.x, 2.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-2.0 * StepSize.x, 2.0 * StepSize.y)).r;

	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 2.0 * StepSize.x, 1.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 1.0 * StepSize.x, 1.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-1.0 * StepSize.x, 1.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-2.0 * StepSize.x, 1.0 * StepSize.y)).r;

	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 2.0 * StepSize.x,-1.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 1.0 * StepSize.x,-1.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-1.0 * StepSize.x,-1.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-2.0 * StepSize.x,-1.0 * StepSize.y)).r;

	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 2.0 * StepSize.x,-2.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2( 1.0 * StepSize.x,-2.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-1.0 * StepSize.x,-2.0 * StepSize.y)).r;
	fSum += texture2D( g_ColorTexture, g_TexCoord + vec2(-2.0 * StepSize.x,-2.0 * StepSize.y)).r;
	
	// Average
	fSum *= (1.0 / (16.0));
	
 	gl_FragColor = vec4(fSum, fSum, fSum, fSum);
}
