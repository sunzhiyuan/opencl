

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
[PreviewShaderVS]
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
[PreviewShaderFS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;

void main()
{
     gl_FragColor = texture2D( g_ColorTexture, g_TexCoord );
}
