//--------------------------------------------------------------------------------------
// File: StraussShader.glsl
// Desc: Shaders for per-pixel lighting
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



attribute vec4 g_PositionIn;
attribute vec3 g_NormalIn;
attribute vec2 g_TexCoordIn;

uniform mat4 g_MatModel;
uniform mat4 g_MatModelViewProj;
uniform mat3 g_MatNormal;

varying vec3 g_Normal;
varying vec4 g_WorldPos;
varying vec2 g_TexCoord;


void main()
{
    gl_Position = g_MatModelViewProj * g_PositionIn;
    g_WorldPos  = g_MatModel * g_PositionIn;
    g_Normal    = g_MatNormal * g_NormalIn;
    g_TexCoord  = g_TexCoordIn;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
