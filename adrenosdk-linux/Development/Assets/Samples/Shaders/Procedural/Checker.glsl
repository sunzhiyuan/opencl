

//--------------------------------------------------------------------------------------
// File: Checker.glsl
// Desc: Procedural checker shader
// Author:                 QUALCOMM, Adreno SDK
//
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[CheckerVS]
#include "../CommonVS.glsl"

uniform mat4 g_matModelViewProj;

attribute vec4 g_vVertex;

varying vec3 g_vPosition;

void main()
{
    gl_Position = g_matModelViewProj * g_vVertex;
    g_vPosition = g_vVertex.xyz;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[CheckerFS]
#include "../CommonFS.glsl"

uniform vec4  g_vColor1;
uniform vec4  g_vColor2;
uniform float g_fFrequency;

varying vec3 g_vPosition;

void main()
{
    float fXMod = mod( g_vPosition.x * g_fFrequency, 1.0 );
    float fYMod = mod( g_vPosition.y * g_fFrequency, 1.0 );
    float fZMod = mod( g_vPosition.z * g_fFrequency, 1.0 );

    float fValue = 1.0;
    if( fXMod < 0.5 ) fValue = 1.0 - fValue;
    if( fYMod < 0.5 ) fValue = 1.0 - fValue;
    if( fZMod < 0.5 ) fValue = 1.0 - fValue;

    gl_FragColor = lerp( g_vColor1, g_vColor2, fValue );
}
