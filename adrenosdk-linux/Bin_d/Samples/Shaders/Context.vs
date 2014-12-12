//--------------------------------------------------------------------------------------
// File: Texture.glsl
// Desc: Texture shader for the ImageEffects sample
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//
// Simple 2D Texture
//
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



attribute vec4 g_vVertex;
attribute vec2 g_vTexcoord;
attribute vec4 g_vNormal;

uniform mat4 g_matModelViewProj;

varying vec2 g_vTexCoords;

void main()
{
    gl_Position = g_matModelViewProj * g_vVertex;
    g_vTexCoords = vec2( g_vTexcoord.x, g_vTexcoord.y );
}


//--------------------------------------------------------------------------------------
// The basic fragment shader
//--------------------------------------------------------------------------------------
