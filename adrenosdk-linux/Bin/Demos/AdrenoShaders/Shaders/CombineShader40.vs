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



attribute vec4 g_Vertex;

varying vec2 g_TexCoord;
varying vec3   g_vFreqTexCoords;

uniform float  g_fFrequency;
uniform float  g_fTime;

void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_Vertex.z, g_Vertex.w );
    g_vFreqTexCoords = vec3( g_TexCoord * g_fFrequency, g_fTime );
   
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
