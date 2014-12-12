

//--------------------------------------------------------------------------------------
// File: Wood.glsl
// Desc: Wood shader
//
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[WoodVS]
#include "../CommonVS.glsl"

uniform mat4  g_matModelViewProj;
uniform mat4  g_matModelView;
uniform mat3  g_matNormal;
uniform float g_fScale;

attribute vec4 g_vVertex;
attribute vec3 g_vNormalES;

varying vec3 g_vOutScaledPosition;
varying vec3 g_vOutNormalES;
varying vec3 g_vOutViewVec;

void main()
{
    gl_Position = g_matModelViewProj * g_vVertex;

    g_vOutScaledPosition = g_fScale * g_vVertex.xyz;
    g_vOutNormalES       = g_matNormal * g_vNormalES;
    g_vOutViewVec        = -vec3( g_matModelView * g_vVertex );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[WoodFS]
#include "../CommonFS.glsl"

#extension GL_OES_texture3D : enable
uniform sampler3D g_sNoise;

uniform float g_Kd;
uniform float g_Ks;
uniform vec4  g_vDarkWoodColor;
uniform vec4  g_vLightWoodColor;
uniform float g_fFrequency;
uniform float g_fNoiseScale;
uniform float g_fRingScale;
uniform vec4  g_vLightDir;

varying vec3 g_vOutScaledPosition;
varying vec3 g_vOutNormalES;
varying vec3 g_vOutViewVec;

void main()
{
    // Signed noise
    float fSNoise = texture3D( g_sNoise, g_vOutScaledPosition ).x;

    // Rings will go along z-axis, perturbed with some noise
    float fRing = fract( g_fFrequency * g_vOutScaledPosition.z + g_fNoiseScale * fSNoise );
    // Map [0, 1] to 0->1->0 with 4(x-x^2)
    fRing *= 4.0 * ( 1.0 - fRing );

    // Adjust ring smoothness and shape, and add some noise
    float fLrp = pow( fRing, g_fRingScale ) + fSNoise;
    vec4 fBase = mix( g_vDarkWoodColor, g_vLightWoodColor, fLrp );

    vec3 vNormal = normalize( g_vOutNormalES );
    // Soft diffuse
    float fDiffuse = 0.5 + 0.5 * dot( g_vLightDir.xyz, vNormal );
    // Standard specular
    float fSpecular = pow( clamp( dot( reflect( -normalize( g_vOutViewVec ), vNormal ), g_vLightDir.xyz ), 0.0, 1.0 ), 12.0 );

    gl_FragColor = g_Kd * fDiffuse * fBase + g_Ks * fSpecular;
}


