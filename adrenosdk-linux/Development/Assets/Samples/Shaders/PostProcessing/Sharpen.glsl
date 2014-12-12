

//--------------------------------------------------------------------------------------
// File: Sharpen.glsl
// Desc: Sharpen shader for the ImageEffects sample
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
[SharpenVS]
#include "../CommonVS.glsl"

uniform float g_fStepSizeX;
uniform float g_fStepSizeY;

attribute vec4 g_vVertex;

varying vec2 g_vTexCoords11;
varying vec2 g_vTexCoords00;
varying vec2 g_vTexCoords02;
varying vec2 g_vTexCoords20;
varying vec2 g_vTexCoords22;

void main()
{
    gl_Position  = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    vec2 vTexCoords = vec2( g_vVertex.z, g_vVertex.w );

    g_vTexCoords11 = vTexCoords;
    g_vTexCoords00 = vTexCoords + vec2( -g_fStepSizeX, -g_fStepSizeY );
    g_vTexCoords02 = vTexCoords + vec2(  g_fStepSizeX, -g_fStepSizeY );
    g_vTexCoords20 = vTexCoords + vec2(  g_fStepSizeX,  g_fStepSizeY );
    g_vTexCoords22 = vTexCoords + vec2( -g_fStepSizeX,  g_fStepSizeY );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[SharpenFS]
#include "../CommonFS.glsl"

uniform sampler2D g_sImageTexture;

varying vec2 g_vTexCoords11;
varying vec2 g_vTexCoords00;
varying vec2 g_vTexCoords02;
varying vec2 g_vTexCoords20;
varying vec2 g_vTexCoords22;

void main()
{
    vec4 vSharp11 = texture2D( g_sImageTexture, g_vTexCoords11 );
    vec4 vSharp00 = texture2D( g_sImageTexture, g_vTexCoords00 );
    vec4 vSharp02 = texture2D( g_sImageTexture, g_vTexCoords02 );
    vec4 vSharp20 = texture2D( g_sImageTexture, g_vTexCoords20 );
    vec4 vSharp22 = texture2D( g_sImageTexture, g_vTexCoords22 );

    // Sharpen by increasing local contrast.
    // This is basically the image, plus the four differences between the pixel and it's neighbors.
    vec4 vSharp = 5.0 * vSharp11 - ( vSharp00 + vSharp02 + vSharp20 + vSharp22 );

    gl_FragColor = vSharp;
}
