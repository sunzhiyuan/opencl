
//--------------------------------------------------------------------------------------
// File: Constant.glsl
// Desc: Simple constant color shader
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

[Constant2VS]
#include "CommonVS.glsl"

uniform mat4 g_mModelViewProjectionMatrix;
uniform vec4 g_vColor;

attribute vec4 g_vVertex;

varying vec4 g_vOutColor;

void main()
{
    gl_Position = g_mModelViewProjectionMatrix * g_vVertex;
    g_vOutColor = g_vColor;
}
