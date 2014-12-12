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



uniform mat4 g_matModelView;
uniform mat4 g_matModelViewProj;
uniform mat3 g_matNormal;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;

// outputs
varying vec3 g_Normal;
varying float g_OutDepth;

void main()
{
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;
    gl_Position  = vPositionCS;
    g_OutDepth  = gl_Position.z / gl_Position.w;

    // transform object-space normals to eye-space
    g_Normal = g_matNormal * g_vNormalOS;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
