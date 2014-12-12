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
[DownsizeShader33VS]
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
[DownsizeShader33FS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;


void main()
{
	vec2 Offset = g_StepSize * 0.5;
	
	vec4 RGBA;
	RGBA.rgb = texture2D( g_ColorTexture,  g_TexCoord + vec2(Offset.x, Offset.y)).xyz;
	RGBA.rgb += texture2D( g_ColorTexture, g_TexCoord + vec2(-Offset.x, Offset.y)).xyz;
	RGBA.rgb += texture2D( g_ColorTexture, g_TexCoord + vec2(Offset.x, -Offset.y)).xyz;
	RGBA.rgb += texture2D( g_ColorTexture, g_TexCoord + vec2(-Offset.x, -Offset.y)).xyz;
	RGBA.rgb *= 0.25;
	
	RGBA.a = dot(RGBA.rgb, vec3(0.299, 0.587, 0.114));
    
    gl_FragColor = RGBA * RGBA.a;
}
