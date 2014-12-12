

//--------------------------------------------------------------------------------------
// File: Lens.glsl
// Desc: Lens shader for the ImageEffects sample
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



uniform vec2 g_vPosition;

attribute vec4 g_vVertex;

varying vec2 g_vTexCoords;
varying vec3 g_vMidCoords;

void main()
{
    gl_Position  = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    g_vTexCoords = vec2( g_vVertex.z, g_vVertex.w );
    g_vMidCoords = vec3( 1.8 * ( 2.0 * vec2( g_vVertex.z, g_vVertex.w ) - 1.0 + g_vPosition ), 0.5 );

}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
