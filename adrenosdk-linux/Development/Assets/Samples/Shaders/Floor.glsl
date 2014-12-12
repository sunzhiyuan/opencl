//--------------------------------------------------------------------------------------
// File: Textured.glsl
// Desc: Shaders for a single-textured object
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
[FloorVS]
#include "CommonVS.glsl"

attribute vec4  In_Position;
attribute vec2  In_TexCoord;

uniform   mat4  g_matWorld;
uniform   mat4  g_matView;
uniform   mat4  g_matProj;

varying vec4    Frag_Position;
varying vec2    Frag_TexCoord;

void main()
{
    // Output clip-space position
    Frag_Position = g_matProj * g_matView * g_matWorld * In_Position;
	gl_Position   = Frag_Position;

    // Pass-thru texture coords
    Frag_TexCoord = In_TexCoord;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[FloorFS]
#include "CommonFS.glsl"

uniform sampler2D g_DiffuseSampler;

varying vec4 Frag_Position;
varying vec2 Frag_TexCoord;


//--------------------------------------------------------------------------------------
// Fragment shader entry point
//--------------------------------------------------------------------------------------
void main()
{
	vec4 vPosition = Frag_Position / Frag_Position.w;

    gl_FragColor = texture2D( g_DiffuseSampler, Frag_TexCoord ); 

	gl_FragColor.a = (1.0 - vPosition.y) - 0.5;
}

