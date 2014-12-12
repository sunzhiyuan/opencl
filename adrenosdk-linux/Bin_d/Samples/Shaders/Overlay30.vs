

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



uniform   vec2 g_vScreenSize;
in vec4 g_vVertex;
out   vec2 g_vTexCoord;

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
