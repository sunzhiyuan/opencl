//--------------------------------------------------------------------------------------
// File: Blend.glsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//
// Simple 2D Texture Blending
//
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
#version 300 es
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





uniform float g_fHalfStepSizeX;
uniform float g_fHalfStepSizeY;


// attribute vec4 g_vVertex;
#define ATTR_POSITION	0

layout(location = ATTR_POSITION) in vec4 g_vVertex;

out vec2 g_vTexCoords;

out vec2 g_vTexCoords0;
out vec2 g_vTexCoords1;
out vec2 g_vTexCoords2;
out vec2 g_vTexCoords3;

void main()
{
    gl_Position  = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    g_vTexCoords = vec2( g_vVertex.z, g_vVertex.w );
    
    g_vTexCoords0 = g_vTexCoords + vec2( -g_fHalfStepSizeX, -g_fHalfStepSizeY );
    g_vTexCoords1 = g_vTexCoords + vec2(  g_fHalfStepSizeX, -g_fHalfStepSizeY );
    g_vTexCoords2 = g_vTexCoords + vec2( -g_fHalfStepSizeX,  g_fHalfStepSizeY );
    g_vTexCoords3 = g_vTexCoords + vec2(  g_fHalfStepSizeX,  g_fHalfStepSizeY );
}



//--------------------------------------------------------------------------------------
// The basic fragment shader
//--------------------------------------------------------------------------------------
