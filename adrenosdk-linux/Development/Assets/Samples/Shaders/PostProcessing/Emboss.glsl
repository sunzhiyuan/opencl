

//--------------------------------------------------------------------------------------
// File: Emboss.glsl
// Desc: Emboss shader for the ImageEffects sample
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//
//    An emboss filter. The filter kernel looks like this:
//
//    [1  1  0]
//    [1  0 -1]
//    [0 -1 -1]
//
//    By using linear filtering and sampling exactly at the upper left and lower right
//    corner of the pixel we can evaluate the entire filter with just two samples by 
//    subtracting the samples and multiplying by four.
//
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[EmbossVS]
#include "../CommonVS.glsl"

uniform float g_fHalfStepSizeX;
uniform float g_fHalfStepSizeY;

attribute vec4 g_vVertex;

varying vec2 g_vTexCoords0;
varying vec2 g_vTexCoords1;

void main()
{
    gl_Position  = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    vec2 vTexCoords = vec2( g_vVertex.z, g_vVertex.w ) + 2.0 * vec2( g_fHalfStepSizeX, g_fHalfStepSizeY );

    g_vTexCoords0 = vTexCoords - vec2( g_fHalfStepSizeX, g_fHalfStepSizeY );
    g_vTexCoords1 = vTexCoords + vec2( g_fHalfStepSizeX, g_fHalfStepSizeY );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[EmbossFS]
#include "../CommonFS.glsl"

uniform sampler2D g_sImageTexture;

varying vec2 g_vTexCoords0;
varying vec2 g_vTexCoords1;

void main()
{
    vec4 vEmboss = 4.0 * ( texture2D( g_sImageTexture, g_vTexCoords0 ) - texture2D( g_sImageTexture, g_vTexCoords1 ) );

    // Offset with 0.5 to show both positive and negative response
    gl_FragColor = vEmboss + 0.5;
}
