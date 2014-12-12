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



// input
attribute vec4 g_Vertex;

// vs variables
uniform float g_StepSize;

// output to the fs
varying vec2 g_TexCoords0;
varying vec2 g_TexCoords1;
varying vec2 g_TexCoords2;
varying vec2 g_TexCoords3;

void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );

    g_TexCoords0 = vec2( g_Vertex.z - g_StepSize, g_Vertex.w - g_StepSize );
    g_TexCoords1 = vec2( g_Vertex.z + g_StepSize, g_Vertex.w - g_StepSize );
    g_TexCoords2 = vec2( g_Vertex.z - g_StepSize, g_Vertex.w + g_StepSize );
    g_TexCoords3 = vec2( g_Vertex.z + g_StepSize, g_Vertex.w + g_StepSize );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
