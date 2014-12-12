

//--------------------------------------------------------------------------------------
// File: Wood.glsl
// Desc: Wood shader
//
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



uniform mat4  g_matModelViewProj;
uniform mat4  g_matModelView;
uniform mat3  g_matNormal;
uniform float g_fScale;

attribute vec4 g_vVertex;
attribute vec3 g_vNormalES;

varying vec3 g_vOutScaledPosition;
varying vec3 g_vOutNormalES;
varying vec3 g_vOutViewVec;

void main()
{
    gl_Position = g_matModelViewProj * g_vVertex;

    g_vOutScaledPosition = g_fScale * g_vVertex.xyz;
    g_vOutNormalES       = g_matNormal * g_vNormalES;
    g_vOutViewVec        = -vec3( g_matModelView * g_vVertex );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
