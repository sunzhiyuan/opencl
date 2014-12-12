

//--------------------------------------------------------------------------------------
// File: Warp.glsl
// Desc: Warp shader for the ImageEffects sample
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
[WarpVS]
#include "../CommonVS.glsl"

uniform float  g_fTime;

attribute vec4 g_vVertex;

varying vec2   g_vTexCoords;
varying vec2   g_vOffsetTexCoords;
varying vec3   g_vNoiseTexCoords;


void main()
{
    gl_Position        = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    g_vTexCoords       = vec2( g_vVertex.z, g_vVertex.w );
    g_vOffsetTexCoords = g_vTexCoords - vec2( 0.5, 0.5 );
    g_vNoiseTexCoords  = vec3( g_vTexCoords, g_fTime );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[WarpFS]
#include "../CommonFS.glsl"

uniform sampler2D g_sImageTexture;
uniform sampler3D g_sNoiseTexture;
uniform float     g_fWarpScale;

varying vec2      g_vTexCoords;
varying vec2      g_vOffsetTexCoords;
varying vec3      g_vNoiseTexCoords;


void main()
{
    vec2  vTexCoords = g_vTexCoords;
    float fStrength  = 0.25 - dot( g_vOffsetTexCoords, g_vOffsetTexCoords );

    if( fStrength > 0.0 )
    {
        vTexCoords += g_fWarpScale * fStrength * (2.0 * texture3D( g_sNoiseTexture, g_vNoiseTexCoords ).xw - 1.0);
    }

    gl_FragColor = texture2D( g_sImageTexture, vTexCoords );
}
