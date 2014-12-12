//--------------------------------------------------------------------------------------
// File: ClothShader.glsl
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
[ClothShaderVS]
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
[ClothShaderFS]
#include "CommonFS.glsl"

varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_View;

uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;
uniform vec4 g_SpecularColor;
uniform vec4 g_AmbientLight;
uniform float g_MinnaertExponent;


void main()
{
    const float PI = 3.1415926535897932384626433832795;

    vec2 UV = g_Tex;
    vec4 Color = texture2D( g_DiffuseTexture, UV * 4.0);
    vec4 N = texture2D( g_BumpTexture, UV ) * 2.0 - 1.0;
    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );

    float NL = max( 0.0, dot( N.xyz, L ) );
    float VN = max( 0.0, dot( N.xyz, V ) );

    // General Minnaert formula:
    // Result = Color * (cos(NL)^k * cos(VN)^(1-k))
    // float A = saturate( pow( NL, g_MinnaertExponent ) );
    // float B = saturate( pow( VN, 1.0 - g_MinnaertExponent ) );
    // float Diffuse = ( A * B );

    // Enhanced Minnaert:
    float Diffuse = NL * pow( max( NL * VN, 0.1 ), 1.0 - g_MinnaertExponent );

    vec3 AmbientColor = Color.rgb * g_AmbientLight.rgb;
    gl_FragColor = vec4( AmbientColor + Color.rgb * Diffuse * Color.w, 1.0 );
}
