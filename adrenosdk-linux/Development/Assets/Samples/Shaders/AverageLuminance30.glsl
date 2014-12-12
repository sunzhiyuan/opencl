
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
[AverageLuminance30VS]

#version 300 es

#include "CommonVS.glsl"

in vec4 g_Vertex;

out vec2 g_TexCoord;


void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_Vertex.z, g_Vertex.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[AverageLuminance30FS]

#version 300 es

#include "CommonFS.glsl"

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
in vec2 g_TexCoord;

out vec4 g_FragColor;

void main()
{
    float fSum = 0.0;
    
    vec2 StepSize = g_StepSize;

	fSum  = texture( g_ColorTexture, g_TexCoord + vec2( 1.5 * StepSize.x, 1.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2( 0.5 * StepSize.x, 1.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2(-0.5 * StepSize.x, 1.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2(-1.5 * StepSize.x, 1.5 * StepSize.y)).r;

	fSum += texture( g_ColorTexture, g_TexCoord + vec2( 1.5 * StepSize.x, 0.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2( 0.5 * StepSize.x, 0.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2(-0.5 * StepSize.x, 0.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2(-1.5 * StepSize.x, 0.5 * StepSize.y)).r;

	fSum += texture( g_ColorTexture, g_TexCoord + vec2( 1.5 * StepSize.x,-0.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2( 0.5 * StepSize.x,-0.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2(-0.5 * StepSize.x,-0.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2(-1.5 * StepSize.x,-0.5 * StepSize.y)).r;

	fSum += texture( g_ColorTexture, g_TexCoord + vec2( 1.5 * StepSize.x,-1.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2( 0.5 * StepSize.x,-1.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2(-0.5 * StepSize.x,-1.5 * StepSize.y)).r;
	fSum += texture( g_ColorTexture, g_TexCoord + vec2(-1.5 * StepSize.x,-1.5 * StepSize.y)).r;
	
	// Average
	fSum *= (1.0 / (16.0));
	
 	g_FragColor = vec4(fSum, fSum, fSum, fSum);
}
