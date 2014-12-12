//--------------------------------------------------------------------------------------
// File: PerPixelLighting.glsl
// Desc: Shaders for per-pixel lighting
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
[AmbientVS]
#include "CommonVS.glsl"

uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;

attribute vec4 g_vPositionOS;

void main()
{
    vec4 vPositionES = g_matModelView     * g_vPositionOS;
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;
    
    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[AmbientFS]
#include "CommonFS.glsl"

struct MATERIAL
{
    vec4  vAmbient;
};

uniform MATERIAL g_Material;


void main()
{
    // Ambient lighting
    gl_FragColor.rgba = g_Material.vAmbient.rgba * 0.15;
}
