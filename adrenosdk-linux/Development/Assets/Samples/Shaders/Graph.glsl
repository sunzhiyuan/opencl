//--------------------------------------------------------------------------------------
// File: Graph.glsl
// Desc: Shaders for OpenCL InteropCLGLES example.  Draws a 2D graph using VBO produced
// by OpenCL
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
[GraphVS]
#include "CommonVS.glsl"

attribute vec4 g_vPositionOS;

void main()
{
    gl_Position = g_vPositionOS;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[GraphFS]
#include "CommonFS.glsl"

void main()
{
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
