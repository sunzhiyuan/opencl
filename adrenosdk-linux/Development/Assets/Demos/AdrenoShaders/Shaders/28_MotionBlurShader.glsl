//--------------------------------------------------------------------------------------
// File: MotionBlurShader.glsl
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
[MotionBlurShaderVS]
#include "CommonVS.glsl"

uniform mat4  g_MatModelView;
uniform mat4  g_MatModelViewProj;
uniform mat4  g_LastMatModelView;
uniform mat4  g_LastMatModelViewProj;
uniform float g_StretchAmount;

attribute vec4 g_PositionIn;
attribute vec2 g_TexCoordIn;
attribute vec3 g_TangentIn;
attribute vec3 g_BinormalIn;
attribute vec3 g_NormalIn;

varying vec2 g_TexCoord;
varying vec3 g_Normal;


void main()
{
    vec4 Position  = g_PositionIn;
    vec3 Tangent   = g_TangentIn;
    vec3 Binormal  = g_BinormalIn;
    vec3 Normal    = g_NormalIn;
    g_TexCoord     = g_TexCoordIn;

    vec4 N = g_MatModelView * vec4( Normal, 0.0 );

    // get current and last position
    vec4 WorldPos     = g_MatModelView * Position;
    vec4 LastWorldPos = g_LastMatModelView * Position;

    // calculate a motion vector in eye space
    vec3 MotionVector = WorldPos.xyz - LastWorldPos.xyz;

    // get clip space positions
    vec4 ClipPos     = g_MatModelViewProj * Position;
    vec4 LastClipPos = g_LastMatModelViewProj * Position;

    // stretch verts that are facing backwards
    float Stretch = dot( MotionVector, N.xyz );
    // gl_Position = Stretch > 0.0 ? ClipPos : LastClipPos; // may be faster, but less smooth
    gl_Position = mix( LastClipPos, ClipPos, saturate( Stretch * g_StretchAmount + 1.0 ) );

    g_Normal = N.xyz;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[MotionBlurShaderFS]
#include "CommonFS.glsl"

uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;
uniform vec3  g_LightPos;
uniform vec3  g_EyePos;

varying vec2 g_TexCoord;
varying vec3 g_Normal;


void main()
{
    vec3 N = normalize( g_Normal );
    vec3 L = normalize( g_LightPos );

    vec4 Color = texture2D( g_DiffuseTexture, g_TexCoord );
    float NL = dot( N, L );

    gl_FragColor = Color * 0.5;             // ambient term
    gl_FragColor += ( Color * NL ) * 0.5;   // diffuse term
}
