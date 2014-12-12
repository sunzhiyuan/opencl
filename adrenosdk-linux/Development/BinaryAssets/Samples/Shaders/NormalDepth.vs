

//--------------------------------------------------------------------------------------
// File: NormalDepth.glsl
// Desc: Shader for exporting Normals and Depth values
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

// This shader is used to output normals to r, g, b and to output depth to the alpha

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// File: CommonFS.glsl
// Desc: Useful common shader code for fragment shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

// default to medium precision
precision mediump float;

// OpenGL ES require that precision is defined for a fragment shader
// usage example: varying NEED_HIGHP vec2 vLargeTexCoord;
#ifdef GL_FRAGMENT_PRECISION_HIGH
   #define NEED_HIGHP highp
#else
   #define NEED_HIGHP mediump
#endif

// Enable supported extensions
#extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



uniform mat3 g_matNormal;
uniform mat4 g_matModelViewProj;

attribute vec4 g_vVertex;
attribute vec3 g_vNormal;

varying float g_fOutDepth;
varying vec3  g_vOutNormal;

void main()
{
    gl_Position  = g_matModelViewProj * g_vVertex;
    g_fOutDepth  = gl_Position.z / gl_Position.w;

    g_vOutNormal = g_matNormal * g_vNormal;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
