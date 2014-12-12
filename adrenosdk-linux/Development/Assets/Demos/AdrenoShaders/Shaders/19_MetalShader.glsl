//--------------------------------------------------------------------------------------
// File: MetalShader.glsl
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
[MetalShaderVS]
#include "CommonVS.glsl"

attribute vec4 g_PositionIn;
attribute vec2 g_TexCoordIn;
attribute vec3 g_NormalIn;
attribute vec3 g_TangentIn;

uniform mat4 g_MatModel;
uniform mat4 g_MatModelViewProj;
uniform mat4 g_MatNormal;
uniform vec3 g_LightPos;
uniform vec3 g_EyePos;

varying vec2 g_TexCoord;
varying vec4 g_Light;
varying vec3 g_View;


void main()
{
    gl_Position = g_MatModelViewProj * g_PositionIn;
    vec4 WorldPos = g_MatModel * g_PositionIn;
    g_TexCoord = g_TexCoordIn;

    // build a matrix to transform from world space to tangent space
    vec3 Tangent = normalize( g_TangentIn );
    vec3 Normal = normalize( g_NormalIn );
    mat3 WorldToTangentSpace;
    WorldToTangentSpace[0] = normalize( g_MatNormal * vec4( Tangent, 1.0 ) ).xyz;
    WorldToTangentSpace[1] = normalize( g_MatNormal * vec4( cross( Tangent, Normal ), 1.0 ) ).xyz;
    WorldToTangentSpace[2] = normalize( g_MatNormal * vec4( Normal, 1.0 ) ).xyz;

    // put the light vector into tangent space
    vec3 Light = normalize( g_LightPos - WorldPos.xyz );
    g_Light.xyz = Light * WorldToTangentSpace;

    // put the view vector into tangent space as well
    vec3 Viewer = normalize( g_EyePos - WorldPos.xyz );
    g_View = Viewer * WorldToTangentSpace;

    // simple attenuation
    float LightRange = 0.1;
    g_Light.w = saturate( 1.0 - dot( Light * LightRange, Light * LightRange ) );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[MetalShaderFS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;
varying vec4 g_Light;
varying vec3 g_View;

uniform float g_Roughness;
uniform float g_Reflection;       // Fresnel reflection index
uniform float g_DiffuseIntensity;
uniform float g_SpecularIntensity;
uniform float g_UVPull;
uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;


void main()
{
    // Using Cook-Torrance lighting model

    vec2 UV = g_TexCoord * vec2( 1.0, g_UVPull );
    vec4 Color = texture2D( g_DiffuseTexture, UV );
    vec4 N = texture2D( g_BumpTexture, UV ) * 2.0 - 1.0;


    float m = g_Roughness;
    float RI = g_Reflection;
    const float PI = 3.1415926535;
   
    // Normalize per-pixel vectors	
    vec3 L = normalize( g_Light.xyz );
    vec3 V = normalize( g_View );
    vec3 H = normalize( L + V );

	float NH = max( 0.0, dot( N.xyz, H ) );
    float VH = max( 0.0, dot( V, H ) );
	float NV = max( 0.0, dot( N.xyz, V ) );
	float NL = max( 0.0, dot( L, N.xyz ) );

    // Beckmann's distribution function
    float NH2 = NH * NH;
    float m2 = m * m;
    float D = ( 1.0 / m2 * NH2 * NH2 ) * (exp( -( ( 1.0 - NH2 ) / ( m2 * NH2 ) ) ) );

    // Fresnel
    float F = RI + ( 1.0 - RI ) * saturate( pow( ( 1.0 - NV ), 5.0 ) );

    // geometric term
    float G = min( 1.0, min( ( 2.0 * NH * NL ) / VH, ( 2.0 * NH * NV ) / VH ) );

    float S = ( F * D * G ) / ( PI * NL * NV );

	//------------------------------
	// Original: Ir = Ai * Ac + Ii(N.L)dw * (Si * Sc + Di * Dc)
	// My formula: Ir = Ia * C + N.L * sat(lidw * (Di * Dc * C + Si * Sc))
	//------------------------------
	const float Ai = 0.1;
	const float dw = 3.0;
    float Di = g_DiffuseIntensity;
    float Si = g_SpecularIntensity * Color.w;
    gl_FragColor.rgba  = Ai * Color + ( NL * saturate( dw * ( ( Di * NL * Color ) + ( Si * S ) ) ) ) * g_Light.w;
}
