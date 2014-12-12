

//--------------------------------------------------------------------------------------
// File: NormalDepth.glsl
// Desc: Shader for exporting Normals and Depth values
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

// This shader is used to output normals to r, g, b and to output depth to the alpha

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[NormalDepthVS]
#include "CommonFS.glsl"

uniform mat3 g_matNormal;
uniform mat4 g_matModelViewProj;

attribute vec4 g_vVertex;
attribute vec3 g_vNormal;

varying float g_fOutDepth;
varying vec3  g_vOutNormal;

void main()
{
    gl_Position  = g_matModelViewProj * g_vVertex;
    g_fOutDepth  = gl_Position.z / gl_Position.w;

    g_vOutNormal = g_matNormal * g_vNormal;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[NormalDepthFS]
#include "CommonFS.glsl"

varying float g_fOutDepth;
varying vec3  g_vOutNormal;

void main()
{
    gl_FragColor = vec4( normalize( g_vOutNormal ), g_fOutDepth );
}
