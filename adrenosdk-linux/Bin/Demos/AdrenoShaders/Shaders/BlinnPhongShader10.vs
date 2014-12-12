//--------------------------------------------------------------------------------------
// File: BlinnPhongShader.glsl
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

uniform mat3 g_MatNormal;
uniform mat4 g_MatModel;
uniform mat4 g_MatModelViewProj;

varying vec3 g_NormalOut;
varying vec3 g_ViewOut;
varying vec2 g_TexOut;


void main()
{
    vec4 WorldPosition = g_MatModel * g_PositionIn;
    gl_Position        = g_MatModelViewProj * g_PositionIn;

    // Transform object-space normals to world space
    vec3 WorldNormal = g_MatNormal * g_NormalIn;

    // Pass everything off to the fragment shader
    g_NormalOut  = WorldNormal.xyz;
    g_ViewOut = vec3( 0.0, 0.0, 0.0 ) - WorldPosition.xyz;
    g_TexOut = g_TexCoordIn;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
