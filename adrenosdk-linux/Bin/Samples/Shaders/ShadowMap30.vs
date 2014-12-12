

//--------------------------------------------------------------------------------------
// File: ShadowMap.glsl
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

#version 300 es

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



uniform mat4 g_matModelViewProj;
uniform mat4 g_matShadow;
uniform mat4 g_matScreenCoord;

in vec4 g_vVertex;

out vec4 g_vOutShadowCoord;
out vec2 g_vOutTexCoord;
out vec4 g_vOutScreenCoord;

void main()
{
    g_vOutShadowCoord = g_matShadow * g_vVertex;

    g_vOutScreenCoord = g_matScreenCoord * g_vVertex;

    gl_Position = g_matModelViewProj * g_vVertex;
}


