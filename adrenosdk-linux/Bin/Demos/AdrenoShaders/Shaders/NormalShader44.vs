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



attribute vec4 g_PositionIn;
attribute vec3 g_NormalIn;

uniform mat4 g_matModelViewProj;
uniform mat4 g_matModelView;

// outputs
varying vec4 g_ViewNormal;


void main()
{
    // Calculate screen position
    gl_Position = g_matModelViewProj * g_PositionIn;

    // Transform normals to view space
    g_ViewNormal = g_matModelView * vec4( g_NormalIn, 0.0 );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
