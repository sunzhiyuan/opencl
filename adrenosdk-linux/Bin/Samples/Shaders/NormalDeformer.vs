

//--------------------------------------------------------------------------------------
// File: Constant.glsl
// Desc: Simple constant color shader
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



uniform mat4 g_mModelViewProjectionMatrix;
uniform vec4 g_vColor;
uniform float g_fSize;

attribute vec4 g_vVertex;
attribute vec3 g_vNormal;

varying vec4 g_vOutColor;

void main()
{
    vec4 vVertex = g_vVertex + vec4( normalize( g_vNormal ) * g_fSize, 0.0 );
    gl_Position = g_mModelViewProjectionMatrix * vVertex;
    g_vOutColor = g_vColor;
}


