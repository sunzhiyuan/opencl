//--------------------------------------------------------------------------------------
// File: Assemble.glsl
// Desc: Shaders for the Disintegrate sample
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
[AssembleVS]
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
[AssembleFS]
#include "CommonFS.glsl"

varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_View;

uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;
uniform sampler2D g_NoiseTexture;
uniform vec4 g_SpecularColor;
uniform vec4 g_AmbientLight;
uniform float g_fAssembleValue;


void main()
{
    vec4 C = texture2D( g_DiffuseTexture, g_Tex);
    vec4 N = texture2D( g_BumpTexture, g_Tex ) * 2.0 - 1.0;
    vec4 NoiseVal = texture2D( g_NoiseTexture, g_Tex );
    if(NoiseVal.x > g_fAssembleValue)
        discard;
    
    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );
    vec3 H = normalize( L + V );
    
    float Diffuse   = saturate( dot( L, N.xyz ) );

    vec3 vNormal    = normalize(N.xyz);
    vec3 vHalf      = normalize( L + V );
    
    float ambient   = 0.2;
    float diffuse   = saturate( dot( L, vNormal ) );
    float specular  = pow( saturate( dot( vNormal, H ) ), g_SpecularColor.w );

    vec3 AmbientColor = C.rgb * g_AmbientLight.rgb;
    gl_FragColor    = (ambient + diffuse) * C + specular * 0.7;

    // Fix-up the alpha value
    gl_FragColor.w = 1.0;
}
