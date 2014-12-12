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
// The fragment shader
//--------------------------------------------------------------------------------------
[BlurDepthShaderFS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
uniform float g_GaussWeight[4];
uniform float g_GaussInvSum;

void main()
{
    // 4 tap gaussian distribution
    float d = g_GaussWeight[0] * texture2D( g_ColorTexture, vec2( g_TexCoord.x + g_StepSize.x, g_TexCoord.y + g_StepSize.y ) ).x;
    d += g_GaussWeight[1] * texture2D( g_ColorTexture, vec2( g_TexCoord.x + g_StepSize.x * 0.5, g_TexCoord.y + g_StepSize.y * 0.5 ) ).x;
    d += g_GaussWeight[2] * texture2D( g_ColorTexture, vec2( g_TexCoord.x - g_StepSize.x * 0.5, g_TexCoord.y - g_StepSize.y * 0.5 ) ).x;
    d += g_GaussWeight[3] * texture2D( g_ColorTexture, vec2( g_TexCoord.x - g_StepSize.x, g_TexCoord.y - g_StepSize.y ) ).x;

    gl_FragDepth = 0.0;//( d * g_GaussInvSum );
}
