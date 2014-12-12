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
[DOFTextureVS]
#include "CommonVS.glsl"

uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;

attribute vec4 g_vPositionOS;
attribute vec2 g_vTexCoord;

varying float g_fOutViewLength;
varying vec2  g_vOutTexCoord;


void main()
{
    gl_Position      = g_matModelViewProj * g_vPositionOS;
    vec4 vPositionES = g_matModelView * g_vPositionOS;
    g_fOutViewLength = length( -vPositionES.xyz );
    g_vOutTexCoord   = g_vTexCoord;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[DOFTextureFS]
#include "CommonFS.glsl"

uniform sampler2D g_sTexture;

varying vec2  g_vOutTexCoord;
varying float g_fOutViewLength;

void main()
{
    gl_FragColor   = texture2D( g_sTexture, g_vOutTexCoord );
    gl_FragColor.a = g_fOutViewLength;
}

