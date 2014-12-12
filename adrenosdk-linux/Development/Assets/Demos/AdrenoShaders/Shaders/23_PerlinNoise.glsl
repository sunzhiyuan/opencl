//--------------------------------------------------------------------------------------
// File: PerlinNoise.glsl
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
[PerlinNoiseVS]
#include "CommonVS.glsl"

attribute vec4 g_PositionIn;
attribute vec2 g_TexCoordIn;

varying vec2 g_TexCoord;


void main()
{
    gl_Position   = g_PositionIn;
    g_TexCoord    = g_TexCoordIn;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[PerlinNoiseFS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_NoiseTexture;
uniform float g_Persistence;


void main()
{
    vec2 UV = g_TexCoord * vec2( 1.003 ); // 1.003 to fix seams
    float Perlin = texture2D( g_NoiseTexture, UV ).x / 2.0;
    Perlin += texture2D( g_NoiseTexture, UV * g_Persistence ).x / 4.0;
    Perlin += texture2D( g_NoiseTexture, UV * g_Persistence * g_Persistence ).x / 8.0;
    Perlin += texture2D( g_NoiseTexture, UV * g_Persistence * g_Persistence * g_Persistence ).x / 16.0;
    Perlin += texture2D( g_NoiseTexture, UV * g_Persistence * g_Persistence * g_Persistence * g_Persistence ).x / 32.0;
    Perlin += texture2D( g_NoiseTexture, UV * g_Persistence * g_Persistence * g_Persistence * g_Persistence * g_Persistence ).x / 32.0;
    gl_FragColor = vec4( Perlin, Perlin, Perlin, 1.0 );
}
