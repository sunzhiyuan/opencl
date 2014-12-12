

//--------------------------------------------------------------------------------------
// File: ShadowMap.glsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


[ShadowMap30VS]

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------

#version 300 es

#include "CommonVS.glsl"

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


[ShadowMap30FS]

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

#version 300 es

#include "CommonFS.glsl"

uniform sampler2DShadow g_sShadowMap;
uniform sampler2D g_sDiffuseSpecularMap;
uniform vec3 g_vAmbient;

in vec4 g_vOutShadowCoord;
in vec2 g_vOutTexCoord;
in vec4 g_vOutScreenCoord;

out vec4 g_FragColor;

void main()
{
	float shadowCompResult = textureProj( g_sShadowMap, g_vOutShadowCoord );
	vec4 vShadowColor = vec4( vec3( shadowCompResult ), 1.0 );

    vec4 vDiffuseAndSpecular = textureProj( g_sDiffuseSpecularMap, g_vOutScreenCoord );
	g_FragColor = vec4( g_vAmbient, 0.0 ) + vShadowColor * vDiffuseAndSpecular;
}

