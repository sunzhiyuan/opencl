

//--------------------------------------------------------------------------------------
// File: Sepia.glsl
// Desc: Sepia shader for the ImageEffects sample
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
[SepiaVS]
#include "../CommonVS.glsl"

attribute vec4 g_vVertex;

varying vec2 g_vTexCoords;


void main()
{
    gl_Position  = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    g_vTexCoords = vec2( g_vVertex.z, g_vVertex.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[SepiaFS]
#include "../CommonFS.glsl"

uniform sampler2D g_sImageTexture;
uniform sampler2D g_sLookupTexture;

varying vec2 g_vTexCoords;


void main()
{
    vec3  vSample    = texture2D( g_sImageTexture, g_vTexCoords ).rgb;
    float fLuminance = dot( vSample, vec3( 0.3, 0.59, 0.11 ) );
    gl_FragColor     = texture2D( g_sLookupTexture, vec2( fLuminance, 0.0 ) );
}
