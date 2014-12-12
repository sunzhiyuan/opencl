//--------------------------------------------------------------------------------------
// File: BlurShader.glsl
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
[BlurShader30VS]

#version 300 es

#include "CommonVS.glsl"

in vec4 g_Vertex;

out vec2 g_TexCoord;


void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 1.0, 1.0 );
    g_TexCoord = vec2( g_Vertex.z, g_Vertex.w );
}



[BlurShader30FS]

#version 300 es

#include "CommonFS.glsl"

in vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
uniform float g_GaussWeight[4];
uniform float g_GaussInvSum;

out vec4 g_FragColor;

void main()
{
    // 4 tap gaussian distribution
    vec4 Color = g_GaussWeight[0] * texture( g_ColorTexture, vec2( g_TexCoord.x + g_StepSize.x, g_TexCoord.y + g_StepSize.y ) );
    Color += g_GaussWeight[1] * texture( g_ColorTexture, vec2( g_TexCoord.x + g_StepSize.x * 0.5, g_TexCoord.y + g_StepSize.y * 0.5 ) );
    Color += g_GaussWeight[2] * texture( g_ColorTexture, vec2( g_TexCoord.x - g_StepSize.x * 0.5, g_TexCoord.y - g_StepSize.y * 0.5 ) );
    Color += g_GaussWeight[3] * texture( g_ColorTexture, vec2( g_TexCoord.x - g_StepSize.x, g_TexCoord.y - g_StepSize.y ) );

    g_FragColor = ( Color * g_GaussInvSum );
}
