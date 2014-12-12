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
[ContextVS]
#include "CommonVS.glsl"

attribute vec4 g_vVertex;
attribute vec2 g_vTexcoord;
attribute vec4 g_vNormal;

uniform mat4 g_matModelViewProj;

varying vec2 g_vTexCoords;

void main()
{
    gl_Position = g_matModelViewProj * g_vVertex;
    g_vTexCoords = vec2( g_vTexcoord.x, g_vTexcoord.y );
}


//--------------------------------------------------------------------------------------
// The basic fragment shader
//--------------------------------------------------------------------------------------
[ContextFS]
#include "CommonFS.glsl"

uniform sampler2D g_sImageTexture;

varying vec2 g_vTexCoords;

void main()
{
    gl_FragColor = texture2D( g_sImageTexture, g_vTexCoords );
}

//--------------------------------------------------------------------------------------
// A fragment shader that takes an alpha parameter
//--------------------------------------------------------------------------------------
//[TextureAlphaFS]
//#include "CommonFS.glsl"
//
//uniform sampler2D g_sImageTexture;
//uniform float     g_fAlpha;
//
//varying vec2 g_vTexCoords;
//
//void main()
//{
//    vec4 vColor = texture2D( g_sImageTexture, g_vTexCoords );
//    vColor.a = g_fAlpha;
//    gl_FragColor = vColor;
//}
