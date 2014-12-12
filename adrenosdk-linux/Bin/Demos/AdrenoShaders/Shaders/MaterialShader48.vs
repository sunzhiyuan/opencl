//--------------------------------------------------------------------------------------
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



uniform mat4  g_MatModelViewProj;

attribute vec4 g_PositionIn;
attribute vec2 g_TexCoordIn;

varying vec2 g_TexCoord;

void main()
{
    vec4 Position  = g_PositionIn;
    vec2 TexCoord  = g_TexCoordIn;

    gl_Position = g_MatModelViewProj * Position;
    g_TexCoord = TexCoord;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------