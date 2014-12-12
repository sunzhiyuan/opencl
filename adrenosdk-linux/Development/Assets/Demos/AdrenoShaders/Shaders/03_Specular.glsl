//--------------------------------------------------------------------------------------
// File: Specular.glsl
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
[SpecularVS]
#include "CommonVS.glsl"

uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;

varying   vec3 g_vNormalES;
varying   vec3 g_vViewVecES;


void main()
{
    vec4 vPositionES = g_matModelView     * g_vPositionOS;
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;

    // Transform object-space normals to eye-space
    vec3 vNormalES = g_matNormal * g_vNormalOS;

    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    g_vNormalES  = vNormalES.xyz;
    g_vViewVecES = vec3(0.0,0.0,0.0) - vPositionES.xyz;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[SpecularFS]
#include "CommonFS.glsl"

struct MATERIAL
{
    vec4  vAmbient;
    vec4  vSpecular;
};

uniform MATERIAL g_Material;
uniform vec3     g_vLightPos;

varying vec3     g_vViewVecES;
varying vec3     g_vNormalES;


void main()
{
    // Normalize per-pixel vectors	
    vec3 vNormal = normalize( g_vNormalES );
    vec3 vLight  = normalize( g_vLightPos );
    vec3 vView   = normalize( g_vViewVecES );
    vec3 vHalf   = normalize( vLight + vView );

    // Compute the lighting in eye-space
    float fSpecular = pow( max( 0.0, dot( vNormal, vHalf ) ), 45.0 );

    gl_FragColor.rgba  = g_Material.vAmbient.rgba * 0.15 + g_Material.vSpecular.rgba * fSpecular;
}
