

//--------------------------------------------------------------------------------------
// File: ShadowMap.glsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

[Depth30VS]

#version 300 es

//--------------------------------------------------------------------------------------
// The vertex shader for rendering to the shadow map
//--------------------------------------------------------------------------------------

#include "CommonVS.glsl"

uniform   mat4 g_matModelViewProj;

in vec4 g_vVertex;

void main()
{
    gl_Position = g_matModelViewProj * g_vVertex;
}


[Depth30FS]

#version 300 es

//--------------------------------------------------------------------------------------
// The fragment shader for rendering to the shadow map
//--------------------------------------------------------------------------------------

#include "CommonFS.glsl"

void main()
{
}
