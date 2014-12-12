
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
// The basic fragment shader
//--------------------------------------------------------------------------------------
[BlendMultiplicativeFS]
#include "CommonFS.glsl"

uniform sampler2D g_sImageTexture1;
uniform sampler2D g_sImageTexture2;

varying vec2 g_vTexCoords;

void main()
{
    vec4 vColor1 = texture2D( g_sImageTexture1, g_vTexCoords );
    vec4 vColor2 = texture2D( g_sImageTexture2, g_vTexCoords );

    gl_FragColor = vColor1 * vColor2;
}

