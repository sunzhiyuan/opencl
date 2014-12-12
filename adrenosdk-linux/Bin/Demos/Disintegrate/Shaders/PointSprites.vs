//--------------------------------------------------------------------------------------
// File: PointSprites.glsl
// Desc: Shaders for the PointSprites sample
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



// Uniform shader constants 
uniform   mat4  g_matModelViewProj;
uniform   float g_fSize;

// Input per-vertex attributes
attribute vec4  g_vPosition;
attribute vec2  g_vUV;
attribute float g_PointSize;

varying vec2    g_vTexCoord;
varying vec2    g_vScreenSpace;

void main()
{
    gl_Position = g_matModelViewProj * g_vPosition;
    // gl_Position = g_vPosition;
    
    gl_PointSize = g_PointSize;
    
    // Map [-1, 1] to [0, 1]
    //g_vTexCoord = (g_vPosition.xy + vec2(1.0, 1.0)) / 2.0;
    //g_vScreenSpace = vec2(g_fSize / 800.0, g_fSize / 480.0 );
    
    g_vTexCoord = g_vUV;
    
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
