//--------------------------------------------------------------------------------------
// File: FishShader.glsl
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
[FishShaderVS]
#include "CommonVS.glsl"

attribute vec4 g_PositionIn;
attribute vec2 g_TexCoordIn;
attribute vec3 g_NormalIn;
attribute vec3 g_TangentIn;

uniform mat4 g_MatModel;
uniform mat4 g_MatModelViewProj;
uniform mat4 g_MatNormal;
uniform vec3 g_LightPos;
uniform vec3 g_LightPos2;
uniform vec3 g_EyePos;

varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_Light2;
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

    // second light
    Light = normalize( g_LightPos2 - WorldPos.xyz );
    g_Light2 = Light * WorldToTangentSpace;

    // put the view vector into tangent space as well
    vec3 Viewer = normalize( g_EyePos - WorldPos.xyz );
    g_View = Viewer * WorldToTangentSpace;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[FishShaderFS]
#include "CommonFS.glsl"

varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_Light2;
varying vec3 g_View;

uniform samplerCube g_Cubemap;
uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;
uniform vec4 g_DiffuseColor;
uniform vec4 g_SpecularColor;
uniform vec4 g_SpecularColor2;
uniform vec4 g_AmbientColor;
uniform vec2 g_Tiling;
uniform float g_ReflectionStrength;


void main()
{
    vec2 UV = g_Tex * g_Tiling;
    vec4 Color = texture2D( g_DiffuseTexture, UV ) * g_DiffuseColor;
    vec4 N = texture2D( g_BumpTexture, UV ) * 2.0 - 1.0;
    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );
    vec3 Half = normalize( L + V );
    float NL = dot( N.xyz, L );
    float VN = max( 0.0, dot( N.xyz, V ) );
    float SelfShadow = smoothstep( -0.5, 0.0, dot( vec3( 0.0, 0.0, 1.0 ), L ) );

    // Specular
    float Specular = pow( max( 0.0, dot( N.xyz, Half ) ), g_SpecularColor.a );

    // Specular 2
    vec3 Half2 = normalize( normalize( g_Light2 ) + V );
    float Specular2 = pow( max( 0.0, dot( N.xyz, Half2 ) ), g_SpecularColor2.a );

    // Cube mapping
    vec3 CubeTexcoord = reflect( -V, N.xyz );
    vec4 CubeColor = textureCube( g_Cubemap, CubeTexcoord );

 	gl_FragColor = g_AmbientColor;
    gl_FragColor += Color * vec4( NL * SelfShadow );
    gl_FragColor.rgb += CubeColor.rgb * g_ReflectionStrength;
    gl_FragColor.rgb += SelfShadow * g_SpecularColor.rgb * Specular * Color.w;
    gl_FragColor.rgb += SelfShadow * g_SpecularColor2.rgb * Specular2 * Color.w;
}
