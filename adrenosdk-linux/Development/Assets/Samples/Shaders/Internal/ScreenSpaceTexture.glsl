

//--------------------------------------------------------------------------------------
// File: ScreenSpaceTexture.glsl
// Desc:
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
[ScreenSpaceTextureVS]
#include "../CommonVS.glsl"

uniform mat4 g_matModelViewProj;
uniform mat4 g_matSSTexture;

attribute vec4 g_vVertex;

varying vec4 g_vTexCoords;

void main()
{
    gl_Position = g_matModelViewProj * g_vVertex;
    g_vTexCoords = g_matSSTexture * g_vVertex;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[ScreenSpaceTextureFS]
#include "../CommonFS.glsl"

uniform sampler2D g_sImageTexture;

varying vec4 g_vTexCoords;

void main()
{
    gl_FragColor = texture2DProj( g_sImageTexture, g_vTexCoords );
}

