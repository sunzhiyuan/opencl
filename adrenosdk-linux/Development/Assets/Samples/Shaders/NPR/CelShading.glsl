

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
[CelShadingVS]
#include "../CommonVS.glsl"

uniform mat3 g_matNormal;
uniform mat4 g_matModelViewProj;

attribute vec4 g_vVertex;
attribute vec3 g_vNormal;
attribute vec2 g_vTexCoord;

varying vec3 g_vOutNormal;
varying vec2 g_vOutTexCoord;

void main()
{
    gl_Position  = g_matModelViewProj * g_vVertex;
    g_vOutNormal = g_matNormal * g_vNormal;
    g_vOutTexCoord = g_vTexCoord;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[CelShadingFS]
#include "../CommonFS.glsl"

uniform sampler2D g_sCelShadingTexture;
uniform sampler2D g_sTexture;
uniform vec3 g_vLightPos;
uniform vec3 g_vAmbient;

varying vec3 g_vOutNormal;
varying vec2 g_vOutTexCoord;

void main()
{
    float vAngle          = max( 0.0, dot( normalize( g_vOutNormal ), normalize( g_vLightPos ) ) );
    vec4 vCelShadingColor = texture2D( g_sCelShadingTexture, vec2( vAngle, 0.0 ) );
    vec4 vTexColor        = texture2D( g_sTexture, g_vOutTexCoord );

    gl_FragColor = vCelShadingColor * ( vTexColor + vec4( g_vAmbient, 0.0 ) );
} 
