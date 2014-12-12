//--------------------------------------------------------------------------------------
// File: Textured.glsl
// Desc: Shaders for a single-textured object
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



attribute vec4  In_Position;
attribute vec2  In_TexCoord;

uniform   mat4  g_matWorld;
uniform   mat4  g_matView;
uniform   mat4  g_matProj;

varying vec4    Frag_Position;
varying vec2    Frag_TexCoord;

void main()
{
    // Output clip-space position
    Frag_Position = g_matProj * g_matView * g_matWorld * In_Position;
	gl_Position   = Frag_Position;

    // Pass-thru texture coords
    Frag_TexCoord = In_TexCoord;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
