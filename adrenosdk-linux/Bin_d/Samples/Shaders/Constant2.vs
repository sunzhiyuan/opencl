
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
// File: CommonVS.glsl
// Desc: Useful common shader code for vertex shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



uniform mat4 g_mModelViewProjectionMatrix;
uniform vec4 g_vColor;

attribute vec4 g_vVertex;

varying vec4 g_vOutColor;

void main()
{
    gl_Position = g_mModelViewProjectionMatrix * g_vVertex;
    g_vOutColor = g_vColor;
}
