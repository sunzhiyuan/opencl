//--------------------------------------------------------------------------------------
// File: EyeShader.glsl
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
[EyeShaderVS]
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
varying vec3 g_CubeTexcoord;


void main()
{
    vec3 Tangent = normalize( g_TangentIn );
    vec3 Normal = normalize( g_NormalIn );

    // shells
//    vec4 ModifiedPosition = vec4( g_PositionIn.xyz + ( Normal * g_FurLength ), g_PositionIn.w );

    gl_Position   = g_MatModelViewProj * g_PositionIn;
    vec4 WorldPos = g_MatModel * g_PositionIn;
    g_Tex         = g_TexCoordIn;


    // build a matrix to transform from world space to tangent space
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

    // for reflections
    g_CubeTexcoord = reflect( -normalize( g_View ), vec3( 0.0, 0.0, 1.0 ) );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[EyeShaderFS]
#include "CommonFS.glsl"

varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_View;
varying vec3 g_CubeTexcoord;

uniform samplerCube g_Cubemap;
uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;
uniform vec4 g_SpecColor;
uniform vec4 g_AmbientColor;
uniform float g_ReflectionStrength;
uniform float g_IrisGlow;


void main()
{
    vec2 UV = g_Tex * vec2( 1.0, 1.0 );
    vec4 Color = texture2D( g_DiffuseTexture, UV );
    vec4 N1 = vec4( 0.0, 0.0, 1.0, 1.0 );
    vec4 N2 = texture2D( g_BumpTexture, g_Tex.xy ) * 2.0 - 1.0;
    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );
    vec3 H = normalize( L + V );
    float NL = dot( N1.xyz, L );
    float NH = max( 0.0, dot( N1.xyz, H ) );

    // Color.w = iris cutout
    float Iris = Color.w;

    // Normal.w = wetness
    float Wetness = N2.w * 0.5 + 0.5;

    // Specular
    float Specular = min( 1.0, pow( NH, g_SpecColor.w ) );
    float SelfShadow = saturate( 4.0 * NL );

    // Cube mapping
    vec4 CubeColor = textureCube( g_Cubemap, g_CubeTexcoord ) * Wetness;

    // Iris highlight
    float IrisLight = max( 0.0, dot( N2.xyz, L ) ) * g_IrisGlow * Iris;
    vec4 IrisColor = Color * IrisLight;

    gl_FragColor = Color * g_AmbientColor;
    gl_FragColor.rgb += Color.rgb * max( 0.0, NL ) * 0.8;
    gl_FragColor.rgb += IrisColor.rgb;
    gl_FragColor.rgb += CubeColor.rgb * g_ReflectionStrength;
    gl_FragColor.rgb += SelfShadow * g_SpecColor.rgb * Specular;
}
