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
//--------------------------------------------------------------------------------------
// File: CommonVS.glsl
// Desc: Useful common shader code for vertex shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



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
