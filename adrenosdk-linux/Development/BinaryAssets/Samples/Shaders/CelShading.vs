

//--------------------------------------------------------------------------------------
// File: CelShading.glsl
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



uniform mat3 g_matNormal;
uniform mat4 g_matModelViewProj;

attribute vec4 g_vVertex;
attribute vec3 g_vNormal;
attribute vec2 g_vTexCoord;

varying vec3 g_vOutNormal;
varying vec2 g_vOutTexCoord;

void main()
{
    gl_Position  = g_matModelViewProj * g_vVertex;
    g_vOutNormal = g_matNormal * g_vNormal;
    g_vOutTexCoord = g_vTexCoord;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
