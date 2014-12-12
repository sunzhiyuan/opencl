

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
// Simple 2D Texture with alpha
//
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// A fragment shader that takes an alpha parameter
//--------------------------------------------------------------------------------------
[TextureAlphaFS]
#include "CommonFS.glsl"

uniform sampler2D g_sImageTexture;
uniform float     g_fAlpha;

varying vec2 g_vTexCoords;

void main()
{
    vec4 vColor = texture2D( g_sImageTexture, g_vTexCoords );
    vColor.a = g_fAlpha;
    gl_FragColor = vColor;
}
