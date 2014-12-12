

//--------------------------------------------------------------------------------------
// File: Warp.glsl
// Desc: Warp shader for the ImageEffects sample
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



uniform float  g_fTime;

attribute vec4 g_vVertex;

varying vec2   g_vTexCoords;
varying vec2   g_vOffsetTexCoords;
varying vec3   g_vNoiseTexCoords;


void main()
{
    gl_Position        = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    g_vTexCoords       = vec2( g_vVertex.z, g_vVertex.w );
    g_vOffsetTexCoords = g_vTexCoords - vec2( 0.5, 0.5 );
    g_vNoiseTexCoords  = vec3( g_vTexCoords, g_fTime );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
