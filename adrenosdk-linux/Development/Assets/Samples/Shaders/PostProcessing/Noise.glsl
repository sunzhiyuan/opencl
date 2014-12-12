

//--------------------------------------------------------------------------------------
// File: Noise.glsl
// Desc: Noise shader for the ImageEffects sample
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
[NoiseVS]
#include "../CommonVS.glsl"

attribute vec4 g_vVertex;

varying   vec2 g_vTexCoords;


void main()
{
    gl_Position  = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    g_vTexCoords = vec2( g_vVertex.z, g_vVertex.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[NoiseFS]
#include "../CommonFS.glsl"

uniform sampler2D g_sImageTexture;
uniform sampler2D g_sNoiseTexture;
uniform float     g_fNoiseScale;

varying vec2      g_vTexCoords;


void main()
{
    vec4 vImageColor = texture2D( g_sImageTexture, g_vTexCoords );
    vec4 vNoiseColor = 2.0 * texture2D( g_sNoiseTexture, g_vTexCoords ) - 1.0;
    gl_FragColor = vImageColor + g_fNoiseScale * vNoiseColor;
}
