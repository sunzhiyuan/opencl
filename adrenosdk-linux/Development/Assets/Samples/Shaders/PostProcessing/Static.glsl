

//--------------------------------------------------------------------------------------
// File: Static.glsl
// Desc: Static shader for the ImageEffects sample
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
[StaticVS]
#include "../CommonVS.glsl"

uniform float  g_fFrequency;
uniform float  g_fTime;

attribute vec4 g_vVertex;

varying vec2   g_vTexCoords;
varying vec3   g_vFreqTexCoords;


void main()
{
    gl_Position      = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    g_vTexCoords     = vec2( g_vVertex.z, g_vVertex.w );
    g_vFreqTexCoords = vec3( g_vTexCoords * g_fFrequency, g_fTime );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[StaticFS]
#include "../CommonFS.glsl"

uniform sampler2D g_sImageTexture;
uniform sampler3D g_sRandomTexture;
uniform float     g_fStaticScale;

varying vec2      g_vTexCoords;
varying vec3      g_vFreqTexCoords;


void main()
{
    vec4 vImageColor  = texture2D( g_sImageTexture, g_vTexCoords );
    vec4 vStaticColor = texture3D( g_sRandomTexture, g_vFreqTexCoords );
    gl_FragColor = vImageColor + g_fStaticScale * vStaticColor;
}
