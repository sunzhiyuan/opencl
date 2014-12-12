//--------------------------------------------------------------------------------------
// File: MaterialShader.glsl
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
[MaterialShader39VS]
#include "CommonVS.glsl"

uniform mat4  g_MatModelViewProj;
uniform mat4  g_MatModel;
uniform vec3  g_LightPos;
uniform vec3  g_EyePos;

attribute vec4 g_PositionIn;
attribute vec2 g_TexCoordIn;
attribute vec3 g_TangentIn;
attribute vec3 g_BinormalIn;
attribute vec3 g_NormalIn;

varying vec2 g_TexCoord;
varying vec3 g_LightVec;
varying vec3 g_ViewVec;
varying vec3 g_Normal;

void main()
{
    vec4 Position  = g_PositionIn;
    vec2 TexCoord  = g_TexCoordIn;
    vec3 Tangent   = g_TangentIn;
    vec3 Binormal  = g_BinormalIn;
    vec3 Normal    = g_NormalIn;

    gl_Position = g_MatModelViewProj * Position;
    g_TexCoord = TexCoord;
    g_Normal   = Normal;

    vec4 WorldPos = g_MatModel * Position;
    vec3 lightVec = g_LightPos - WorldPos.xyz;
    vec3 viewVec  = g_EyePos   - WorldPos.xyz;

    // Transform vectors into tangent space
    g_LightVec.x = dot( lightVec, Tangent);
    g_LightVec.y = dot( lightVec, Binormal);
    g_LightVec.z = dot( lightVec, Normal);

    g_ViewVec.x  = dot( viewVec, Tangent );
    g_ViewVec.y  = dot( viewVec, Binormal );
    g_ViewVec.z  = dot( viewVec, Normal );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[MaterialShader39FS]
#include "CommonFS.glsl"

uniform vec4 g_AmbientColor;
uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;

varying vec2 g_TexCoord;
varying vec3 g_LightVec;
varying vec3 g_ViewVec;
varying vec3 g_Normal;

void main()
{
    vec4 vBaseColor = texture2D( g_DiffuseTexture, g_TexCoord );
    
    // Select the normal in the appropriate space
    // Note: the mip level is biased to sharpen the bumpmapping effect
    vec3 vNormal    = texture2D( g_BumpTexture, g_TexCoord, -1.0 ).xyz * 2.0 - 1.0;

    // Standard Phong lighting
    float fAtten    = saturate( 1.0 - 0.05 * dot( g_LightVec, g_LightVec ) );
    vec3  vLight    = normalize( g_LightVec );
    vec3  vView     = normalize( g_ViewVec );
    vec3  vHalf     = normalize( vLight + vView );
    float fDiffuse  = saturate( dot( vLight, vNormal ) );
    float fSpecular = pow( saturate( dot( vHalf, vNormal ) ), 64.0 );
    float fLightIntensity = 2.0;

    gl_FragColor = ( fDiffuse * fAtten * fLightIntensity + g_AmbientColor ) * vBaseColor;
    gl_FragColor.rgb += vec3( ( fSpecular * fAtten ) * 0.7);
}
