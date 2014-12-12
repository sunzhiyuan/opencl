

//--------------------------------------------------------------------------------------
// File: PreviewShader.glsl
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
[PreviewShader30VS]

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
[PreviewShader30FS]

#version 300 es

#include "CommonFS.glsl"

in vec2 g_TexCoord;
out vec4 g_FragColor;

uniform sampler2D g_ColorTexture;

void main()
{
     g_FragColor = texture( g_ColorTexture, g_TexCoord );
}
