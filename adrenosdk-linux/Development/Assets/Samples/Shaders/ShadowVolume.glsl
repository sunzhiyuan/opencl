

//--------------------------------------------------------------------------------------
// File: StencilVolume.glsl
// Desc: Simple constant color shader
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
[ShadowVolumeVS]
#include "CommonVS.glsl"

uniform vec4 g_vLightDirection;
uniform mat3 g_matNormal;
uniform mat4 g_matModelViewProj;
uniform mat4 g_matViewProj;

attribute vec4 g_vVertex;
attribute vec3 g_vNormal;

#define INFINITY 10000.0

void main()
{
    vec3 vNormal = g_matNormal * g_vNormal;

    if( dot( g_vLightDirection.xyz, vNormal ) >= 0.0 )
    {
        gl_Position = g_matViewProj * vec4( g_vLightDirection.xyz, 0.0 ) * INFINITY;
    }
    else
    {
        gl_Position = g_matModelViewProj * g_vVertex;
    }
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[ShadowVolumeFS]
#include "CommonFS.glsl"

void main()
{
    gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
}
