

//--------------------------------------------------------------------------------------
// File: CelShading.glsl
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



uniform mat3 g_matNormal;
uniform mat4 g_matModelViewProj;

#define ATTR_POSITION	0
#define ATTR_NORMAL		3
#define ATTR_TEXCOORD	6

layout(location = ATTR_POSITION) in vec4 g_vVertex;
layout(location = ATTR_NORMAL) in vec3 g_vNormal;
layout(location = ATTR_TEXCOORD) in vec2 g_vTexCoord;

out vec3 g_vOutNormal;
out vec2 g_vOutTexCoord;
out float g_fOutDepth;

void main()
{
    gl_Position  = g_matModelViewProj * g_vVertex;
    g_vOutNormal = g_matNormal * g_vNormal;
    g_vOutTexCoord = g_vTexCoord;
    
    g_fOutDepth  = gl_Position.z / gl_Position.w;
    
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
