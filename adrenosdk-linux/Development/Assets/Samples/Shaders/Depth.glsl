

//--------------------------------------------------------------------------------------
// File: ShadowMap.glsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader for rendering to the shadow map
//--------------------------------------------------------------------------------------
[DepthVS]
#include "CommonVS.glsl"

uniform   mat4 g_matModelViewProj;

attribute vec4 g_vVertex;

void main()
{
    gl_Position = g_matModelViewProj * g_vVertex;
}


//--------------------------------------------------------------------------------------
// The fragment shader for rendering to the shadow map
//--------------------------------------------------------------------------------------
[DepthFS]
#include "CommonFS.glsl"

void main()
{
}

