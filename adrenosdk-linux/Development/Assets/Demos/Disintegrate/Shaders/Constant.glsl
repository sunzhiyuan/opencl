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


//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[ConstantVS]
#include "CommonVS.glsl"

uniform mat4 g_mModelViewProjectionMatrix;

attribute vec4 g_vVertex;
attribute vec4 g_vColor;

varying vec4 g_vOutColor;

void main()
{
    gl_Position = g_mModelViewProjectionMatrix * g_vVertex;
    g_vOutColor = g_vColor;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[ConstantFS]
#include "CommonFS.glsl"

varying vec4 g_vOutColor;
void main()
{
    gl_FragColor = g_vOutColor;
} 
