//--------------------------------------------------------------------------------------
// File: SkinShader.glsl
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
[SkinShaderVS]
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

varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_View;


void main()
{
    gl_Position   = g_MatModelViewProj * g_PositionIn;
    vec4 WorldPos = g_MatModel * g_PositionIn;
    g_Tex         = g_TexCoordIn;

    // build a matrix to transform from world space to tangent space
    vec3 Tangent = normalize( g_TangentIn );
    vec3 Normal = normalize( g_NormalIn );
    mat3 WorldToTangentSpace;
    WorldToTangentSpace[0] = normalize( g_MatNormal * vec4( Tangent, 1.0 ) ).xyz;
    WorldToTangentSpace[1] = normalize( g_MatNormal * vec4( cross( Tangent, Normal ), 1.0 ) ).xyz;
    WorldToTangentSpace[2] = normalize( g_MatNormal * vec4( Normal, 1.0 ) ).xyz;

    // put the light vector into tangent space
    vec3 Light = normalize( g_LightPos - WorldPos.xyz );
    g_Light = Light * WorldToTangentSpace;

    // put the view vector into tangent space as well
    vec3 Viewer = normalize( g_EyePos - WorldPos.xyz );
    g_View = Viewer * WorldToTangentSpace;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[SkinShaderFS]
#include "CommonFS.glsl"

varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_View;

uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;
uniform vec4 g_AmbientLight;
uniform float g_MinnaertExponent;
uniform float g_RollOff;
uniform vec4 g_SubsurfColor;
uniform vec4 g_SpecColor;


void main()
{
    const float PI = 3.1415926535897932384626433832795;

    vec2 UV = g_Tex * vec2( 6.0, 3.0 );
    vec4 Color = texture2D( g_DiffuseTexture, UV );
    vec4 N = texture2D( g_BumpTexture, UV ) * 2.0 - 1.0;
    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );
    vec3 H = normalize( L + V );
    float NL = dot( N.xyz, L );
    float VN = max( 0.0, dot( N.xyz, V ) );
    float NH = max( 0.0, dot( N.xyz, H ) );

    // Specular
    float Specular = min( 1.0, pow( NH, g_SpecColor.w ) * Color.w );
    float SelfShadow = saturate( 4.0 * NL );

    // Subsurface scattering
    float SubLamb = saturate( smoothstep( -g_RollOff, 1.0, NL ) - smoothstep( 0.0, 1.0, NL ) );
    vec4 SubSurface = SubLamb * g_SubsurfColor;

    // General Minnaert formula:
    // Result = Color * (cos(NL)^k * cos(VN)^(1-k))
    float A = saturate( pow( max( 0.02, NL ), g_MinnaertExponent ) );
    float B = saturate( pow( VN, 1.0 - g_MinnaertExponent ) );

    gl_FragColor = Color * g_AmbientLight;
    gl_FragColor += SubSurface;
    gl_FragColor.rgb += Color.rgb * ( A * B );
    gl_FragColor.rgb += SelfShadow * g_SpecColor.rgb * Specular;
}
