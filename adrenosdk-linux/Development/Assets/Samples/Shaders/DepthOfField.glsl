//--------------------------------------------------------------------------------------
// File: Depth Of Field.glsl
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
[DOFVS]
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
[DOFFS]
#include "CommonFS.glsl"

uniform sampler2D g_sSharpImage;
uniform sampler2D g_sBlurredImage;

uniform float g_fRange;
uniform float g_fFocus;

varying vec2 g_vTexCoords;

void main()
{
    vec4 vSharpColor = texture2D( g_sSharpImage,   g_vTexCoords );
    vec4 vBlurColor  = texture2D( g_sBlurredImage, g_vTexCoords );
    gl_FragColor = mix( vSharpColor, vBlurColor, clamp( g_fRange * abs( g_fFocus - vSharpColor.a ), 0.0, 1.0 ) );
}

