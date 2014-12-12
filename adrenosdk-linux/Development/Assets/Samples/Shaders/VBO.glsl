

//--------------------------------------------------------------------------------------
// File: VBO.glsl
// Desc: Shaders for VBO sample
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
[VBOVS]
#include "CommonVS.glsl"

attribute vec3 g_vVertex;
attribute vec2 g_vTCoord;
varying vec2 g_vTexCoord;


void main()
{    
    gl_Position  = vec4( g_vVertex.x, g_vVertex.y, g_vVertex.z, 1.0 );
    g_vTexCoord = g_vTCoord;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[VBOFS]
#include "CommonFS.glsl"

uniform sampler2D g_sImageTexture;
varying vec2      g_vTexCoord;

void main()
{
    gl_FragColor = texture2D( g_sImageTexture, g_vTexCoord );
}
