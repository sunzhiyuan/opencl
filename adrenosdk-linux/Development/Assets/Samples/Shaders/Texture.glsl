

//--------------------------------------------------------------------------------------
// File: Texture.glsl
// Desc: Texture shader for the ImageEffects sample
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//
// Simple 2D Texture
//
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[TextureVS]
#include "CommonVS.glsl"

attribute vec4 g_vVertex;

varying vec2 g_vTexCoords;

void main()
{
    gl_Position  = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    g_vTexCoords = vec2( g_vVertex.z, g_vVertex.w );
}


//--------------------------------------------------------------------------------------
// The basic fragment shader
//--------------------------------------------------------------------------------------
[TextureFS]
#include "CommonFS.glsl"

uniform sampler2D g_sImageTexture;

varying vec2 g_vTexCoords;

void main()
{
    gl_FragColor = texture2D( g_sImageTexture, g_vTexCoords );
}
