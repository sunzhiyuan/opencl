//--------------------------------------------------------------------------------------
// File: FurShader.glsl
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
[FurShaderVS]
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
uniform float g_FurLength;

varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_View;


void main()
{
    vec3 Tangent = normalize( g_TangentIn );
    vec3 Normal = normalize( g_NormalIn );

    // shells
    vec4 ModifiedPosition = vec4( g_PositionIn.xyz + ( Normal * g_FurLength ), g_PositionIn.w );

    gl_Position   = g_MatModelViewProj * ModifiedPosition;
    vec4 WorldPos = g_MatModel * ModifiedPosition;
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
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[FurShaderFS]
#include "CommonFS.glsl"

varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_View;

uniform sampler2D g_DiffuseTexture;
uniform float g_FurThinning;
uniform vec4 g_SpecColor;


void main()
{
    vec2 UV = g_Tex * vec2( 1.0, 1.0 );
    vec4 Color = texture2D( g_DiffuseTexture, UV );
    vec4 N = vec4( 0.0, 0.0, 1.0, 1.0 );
    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );
    vec3 H = normalize( L + V );
    float NL = dot( N.xyz, L );
    float NH = max( 0.0, dot( N.xyz, H ) );

    // Specular
    float SpecOpacity = ( 1.0 - Color.w ); // re-use Color.w to paint in spec between the strands
    float Specular = min( 1.0, pow( NH, g_SpecColor.w ) * SpecOpacity );
    float SelfShadow = saturate( 4.0 * NL );

    gl_FragColor = vec4( Color.rgb * NL, saturate( Color.w - g_FurThinning ) );
    gl_FragColor.rgb += SelfShadow * g_SpecColor.rgb * Specular;
}
