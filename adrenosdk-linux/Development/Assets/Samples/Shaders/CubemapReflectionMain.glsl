//--------------------------------------------------------------------------------------
// File: CubemapReflection.glsl
// Desc: Reflection shaders for the CubemapReflection sample
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
[CubemapReflectionMainVS]
#include "CommonVS.glsl"

uniform   mat4 g_matModelViewProj;
uniform   mat4 g_matModelView;
uniform   mat3 g_matNormal;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;
attribute vec2 g_vTexcoordIN;

varying   vec3 g_vNormalES;
varying   vec3 g_vViewVecES;
varying   vec3 g_vTexcoord;

void main()
{
    // Compute the eye space position
    vec4 vPositionES = g_matModelView * g_vPositionOS;

    // Compute the clip space position
    gl_Position = g_matModelViewProj * g_vPositionOS;
    
    // Transform object-space normals to eye-space
    g_vNormalES  = g_matNormal * g_vNormalOS;
    
    // Pass the view vector to the fragment shader
    g_vViewVecES = -vPositionES.xyz;
    
    // Pass the texture coordinates to the fragment shader
    g_vTexcoord = reflect( normalize( vPositionES.xyz/vPositionES.w ), g_vNormalES );
}


//--------------------------------------------------------------------------------------
// The main fragment shader
//--------------------------------------------------------------------------------------
[CubemapReflectionMainFS]
#include "CommonFS.glsl"

const vec3  g_vAmbient   = vec3(0.2, 0.2, 0.2);
const vec3  g_vDiffuse   = vec3(1.0, 1.0, 0.1);
const vec3  g_vSpecular  = vec3(1.0, 1.0, 1.0);
const float g_fShininess = 64.0;

uniform samplerCube g_RefMap;
uniform samplerCube g_EnvMap;
uniform vec3 g_vLightPos;

varying vec3 g_vNormalES;
varying vec3 g_vViewVecES;
varying vec3 g_vTexcoord;

void main()
{
    // Normalize per-pixel vectors	
    vec3 vNormal = normalize( g_vNormalES );
    vec3 vLight  = normalize( g_vLightPos );
    vec3 vView   = normalize( g_vViewVecES );
    vec3 vHalf   = normalize( vLight + vView );

    // Compute the lighting in eye-space
    float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );
    float fSpecular = pow( max( 0.0, dot( vNormal, vHalf ) ), g_fShininess );

    // Combine lighting with the material properties
    vec4 vColor;
    vColor.rgb = g_vAmbient + g_vDiffuse*fDiffuse + g_vSpecular*fSpecular;
    vColor.a    = 1.0;

    vec4 vRefColor = textureCube( g_RefMap, g_vTexcoord );
    vec4 vEnvColor = textureCube( g_EnvMap, g_vTexcoord );

    gl_FragColor = vColor * ( vEnvColor * ( 1.0 - vRefColor.a ) + ( vRefColor * vRefColor.a ) );
}
