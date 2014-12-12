//--------------------------------------------------------------------------------------
// File: Overlay.glsl
// Desc: Shaders for drawing a textured overlay
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
// Adreno SDK
// 
// Copyright (c) 2009 QUALCOMM Incorporated.
// All Rights Reserved. 
// QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



uniform   vec2 g_vScreenSize;
attribute vec4 g_vVertex;
varying   vec2 g_vTexCoord;

void main()
{
    // Transform the position
    gl_Position.x = +( 2.0 * ( g_vVertex.x / g_vScreenSize.x ) - 1.0 );
    gl_Position.y = -( 2.0 * ( g_vVertex.y / g_vScreenSize.y ) - 1.0 );
    gl_Position.z = 0.0;
    gl_Position.w = 1.0;
    
    // Pass through texture coordinates
    g_vTexCoord.x = g_vVertex.z;
    g_vTexCoord.y = g_vVertex.w;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
