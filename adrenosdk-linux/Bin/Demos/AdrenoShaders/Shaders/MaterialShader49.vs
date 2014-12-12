//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
// 
// //--------------------------------------------------------------------------------------
// // The vertex shader
// //--------------------------------------------------------------------------------------
// [MaterialShaderVS]
// #include "CommonVS.glsl"
// 
// uniform mat4  g_MatModelViewProj;
// uniform mat4  g_MatModel;
// uniform mat4  g_MatNormal;
// uniform vec3  g_LightPos;
// uniform vec3  g_EyePos;
// 
// attribute vec4 g_PositionIn;
// attribute vec2 g_TexCoordIn;
// attribute vec3 g_NormalIn;
// 
// varying vec2 g_TexCoord;
// varying vec3 g_LightVec;
// varying vec3 g_ViewVec;
// varying vec3 g_Normal;
// 
// void main()
// {
//     vec4 Position  = g_PositionIn;
//     vec2 TexCoord  = g_TexCoordIn;
//     vec3 Normal    = g_NormalIn;
// 
//     gl_Position = g_MatModelViewProj * Position;
//     g_Normal    = ( g_MatNormal * vec4( Normal, 1.0 ) ).xyz;
//     g_TexCoord  = TexCoord;
// 
//     vec4 WorldPos = g_MatModel * Position;
//     g_LightVec = g_LightPos - WorldPos.xyz;
//     g_ViewVec  = g_EyePos   - WorldPos.xyz;
// }



//--------------------------------------------------------------------------------------
// The vertex shader.
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
varying vec2 g_TexCoord;

void main()
{
    gl_Position  = vec4( g_PositionIn.x, g_PositionIn.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_PositionIn.z, g_PositionIn.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
