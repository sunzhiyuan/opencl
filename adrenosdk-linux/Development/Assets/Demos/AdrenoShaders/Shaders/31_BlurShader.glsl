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
[BlurShader31VS]
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
[BlurShader31FS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
uniform float g_GaussWeight[4];
uniform float g_GaussInvSum;

void main()
{
    vec2 UV = g_TexCoord;

    // 4 tap gaussian distribution
    vec4 Color = g_GaussWeight[0] * texture2D( g_ColorTexture, UV + g_StepSize );
    Color += g_GaussWeight[1] * texture2D( g_ColorTexture, UV + g_StepSize * 0.5 );
    Color += g_GaussWeight[2] * texture2D( g_ColorTexture, UV - g_StepSize * 0.5 );
    Color += g_GaussWeight[3] * texture2D( g_ColorTexture, UV - g_StepSize );

    gl_FragColor = ( Color * g_GaussInvSum );
    //gl_FragColor = vec4( Color.r * g_GaussInvSum, 0.0, texture2D( g_ColorTexture, UV ).b, 1.0 );
}
