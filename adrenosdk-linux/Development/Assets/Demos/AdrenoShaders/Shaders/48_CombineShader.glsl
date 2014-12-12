//--------------------------------------------------------------------------------------
// File: CombineShader.glsl
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
[CombineShader48VS]
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
[CombineShader48FS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform sampler2D g_LightTexture;

void main()
{
    vec4 Color = texture2D( g_ColorTexture, g_TexCoord );
    vec4 Light = texture2D( g_LightTexture, g_TexCoord );

    // mix lights
    gl_FragColor = Color * 0.02 + Light;
}
