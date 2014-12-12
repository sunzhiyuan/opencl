//--------------------------------------------------------------------------------------
// Desc: Shaders for the Robot demo
//
// Adreno SDK
//
// Copyright (c) 2013 QUALCOMM Technologies, Inc.
// All Rights Reserved. 
// QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

[RoomVS]
#include "CommonVS.glsl"
//--------------------------------------------------------------------------------------
// External constants
//--------------------------------------------------------------------------------------
uniform vec3 g_vLightDir;
uniform vec4 g_vMaterialDiffuse;
uniform vec4 g_vMaterialSpecular;
uniform vec3 g_vMaterialAmbient;
uniform vec3 g_vMaterialEmissive;

// Matrix palette
uniform mat4 g_matWorld;
uniform mat4 g_matView;
uniform mat4 g_matProj;


//--------------------------------------------------------------------------------------
// Vertex structures
//--------------------------------------------------------------------------------------
attribute vec4  In_Pos;
attribute vec2  In_Tex0;

varying   vec2  Out_Tex0;


//--------------------------------------------------------------------------------------
// Vertex shader
//--------------------------------------------------------------------------------------
void main()
{
    // Transform position into clip space
    vec4 vPositionOS = In_Pos.xyzw;
    vec4 vPositionWS = g_matWorld * vPositionOS;
    vec4 vPositionES = g_matView * vPositionWS;
    vec4 vPositionCS = g_matProj * vPositionES;
    gl_Position = vPositionCS;

    // Copy the input texture coordinate through
    Out_Tex0.x  = In_Tex0.x;
    Out_Tex0.y  = 1.0 - In_Tex0.y;
}


[RoomFS]
#include "CommonFS.glsl"
//--------------------------------------------------------------------------------------
// Fragment shader
//--------------------------------------------------------------------------------------
uniform vec4 g_vMaterialDiffuse;
uniform vec4 g_vMaterialSpecular;
uniform vec3 g_vMaterialAmbient;
uniform vec3 g_vMaterialEmissive;

uniform sampler2D BaseTexture;

varying vec2  Out_Tex0;


void main()
{
    gl_FragColor.rgba = g_vMaterialDiffuse * texture2D( BaseTexture, Out_Tex0 ).rgba;
}
