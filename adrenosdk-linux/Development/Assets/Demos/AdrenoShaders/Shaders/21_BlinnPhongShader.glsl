//--------------------------------------------------------------------------------------
// File: BlinnPhongShader.glsl
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
[BlinnPhongShader21VS]
#include "CommonVS.glsl"

attribute vec4 g_PositionIn;
attribute vec3 g_NormalIn;
attribute vec2 g_TexCoordIn;

uniform mat3 g_MatNormal;
uniform mat4 g_MatModel;
uniform mat4 g_MatModelViewProj;

varying vec3 g_NormalOut;
varying vec3 g_ViewOut;
varying vec2 g_TexOut;


void main()
{
    vec4 WorldPosition = g_MatModel * g_PositionIn;
    gl_Position        = g_MatModelViewProj * g_PositionIn;

    // Transform object-space normals to world space
    vec3 WorldNormal = g_MatNormal * g_NormalIn;

    // Pass everything off to the fragment shader
    g_NormalOut  = WorldNormal.xyz;
    g_ViewOut = vec3( 0.0, 0.0, 0.0 ) - WorldPosition.xyz;
    g_TexOut = g_TexCoordIn;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[BlinnPhongShader21FS]
#include "CommonFS.glsl"

varying vec3 g_ViewOut;
varying vec3 g_NormalOut;
varying vec2 g_TexOut;

uniform sampler2D g_DiffuseTexture;
uniform vec4 g_DiffuseColor;
uniform vec4 g_SpecularColor;
uniform vec4 g_AmbientLight;
uniform vec3 g_LightDir;


void main()
{
    // Normalize per-pixel vectors	
    vec3 vNormal = normalize( g_NormalOut );
    vec3 vLight  = normalize( g_LightDir );
    vec3 vView   = normalize( g_ViewOut );
    vec3 vHalf   = normalize( vLight + vView );
    vec4 C = texture2D( g_DiffuseTexture, g_TexOut.xy );

    // Compute the lighting in world space
    float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );
    float fSpecular = pow( max( 0.0, dot( vNormal, vHalf ) ), g_SpecularColor.a );
	float SelfShadow = 4.0 * fDiffuse;

    // Combine lighting with the material properties
    gl_FragColor.rgba  = g_AmbientLight;
    gl_FragColor.rgba += g_DiffuseColor.rgba * fDiffuse * C;
    gl_FragColor.rgb  += SelfShadow * vec3(0.15, 0.15, 0.15) * g_SpecularColor.rgb * fSpecular;
}
