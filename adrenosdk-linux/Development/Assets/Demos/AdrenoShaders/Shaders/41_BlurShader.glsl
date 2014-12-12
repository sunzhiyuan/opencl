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
[BlurShader41VS]
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
[BlurShader41FS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
uniform float g_GaussWeight[4];
uniform float g_GaussInvSum;

void main()
{
    // 4 tap gaussian distribution
    vec4 Color = g_GaussWeight[0] * texture2D( g_ColorTexture, vec2( g_TexCoord.x + g_StepSize.x, g_TexCoord.y + g_StepSize.y ) );
    Color += g_GaussWeight[1] * texture2D( g_ColorTexture, vec2( g_TexCoord.x + g_StepSize.x * 0.5, g_TexCoord.y + g_StepSize.y * 0.5 ) );
    Color += g_GaussWeight[2] * texture2D( g_ColorTexture, vec2( g_TexCoord.x - g_StepSize.x * 0.5, g_TexCoord.y - g_StepSize.y * 0.5 ) );
    Color += g_GaussWeight[3] * texture2D( g_ColorTexture, vec2( g_TexCoord.x - g_StepSize.x, g_TexCoord.y - g_StepSize.y ) );

    gl_FragColor = ( Color * g_GaussInvSum );
}
