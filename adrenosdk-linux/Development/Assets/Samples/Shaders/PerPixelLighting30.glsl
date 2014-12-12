

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
[PerPixelLighting30VS]

#version 300 es

#include "CommonVS.glsl"

uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;

in vec4 g_vPositionOS;
in vec3 g_vNormalOS;

out   vec3 g_vNormalES;
out   vec3 g_vViewVecES;


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
[PerPixelLighting30FS]
#version 300 es

#include "CommonFS.glsl"

struct MATERIAL
{
    vec4  vAmbient;
    vec4  vDiffuse;
    vec4  vSpecular;
};

uniform MATERIAL g_Material;
uniform vec3     g_vLightPos;

in vec3     g_vViewVecES;
in vec3     g_vNormalES;
out vec4 g_FragColor;

void main()
{
    // Normalize per-pixel vectors	
    vec3 vNormal = normalize( g_vNormalES );
    vec3 vLight  = normalize( g_vLightPos );
    vec3 vView   = normalize( g_vViewVecES );
    vec3 vHalf   = normalize( vLight + vView );

    // Compute the lighting in eye-space
    float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );
    float fSpecular = pow( max( 0.0, dot( vNormal, vHalf ) ), g_Material.vSpecular.a );

    // Combine lighting with the material properties
    g_FragColor.rgba  = g_Material.vAmbient.rgba;
    g_FragColor.rgba += g_Material.vDiffuse.rgba * fDiffuse;
    g_FragColor.rgb  += g_Material.vSpecular.rgb * fSpecular;
}
