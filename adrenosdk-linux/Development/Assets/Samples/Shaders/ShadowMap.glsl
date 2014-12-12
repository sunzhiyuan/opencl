

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
[ShadowMapVS]
#include "CommonVS.glsl"

uniform mat4 g_matModelViewProj;
uniform mat4 g_matShadow;
uniform mat4 g_matScreenCoord;

attribute vec4 g_vVertex;

varying vec4 g_vOutShadowCoord;
varying vec2 g_vOutTexCoord;
varying vec4 g_vOutScreenCoord;

void main()
{
    g_vOutShadowCoord = g_matShadow * g_vVertex;

    g_vOutScreenCoord = g_matScreenCoord * g_vVertex;

    gl_Position = g_matModelViewProj * g_vVertex;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[ShadowMapFS]
#include "CommonFS.glsl"

uniform sampler2D g_sShadowMap;
uniform sampler2D g_sDiffuseSpecularMap;
uniform vec3 g_vAmbient;

varying vec4 g_vOutShadowCoord;
varying vec4 g_vOutScreenCoord;

void main()
{
   float fLightDepth = ( texture2DProj( g_sShadowMap, g_vOutShadowCoord ) ).r;

   vec4 vShadowColor;
   
   float fBias = 0.03;
   
   if( (g_vOutShadowCoord.z / g_vOutShadowCoord.w) - fBias > fLightDepth )
   {
       vShadowColor = vec4( vec3( 0.0 ), 1.0 );
   }
   else
   {
       vShadowColor = vec4( vec3( 1.0 ), 1.0 );
   }
   
   vec4 vDiffuseAndSpecular = texture2DProj( g_sDiffuseSpecularMap, g_vOutScreenCoord );

   gl_FragColor = vec4( g_vAmbient, 0.0 ) + vShadowColor * vDiffuseAndSpecular;
}
