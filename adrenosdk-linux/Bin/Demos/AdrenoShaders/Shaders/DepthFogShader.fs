//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// File: CommonFS.glsl
// Desc: Useful common shader code for fragment shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


// default to medium precision
precision mediump float;

// OpenGL ES require that precision is defined for a fragment shader
// usage example: varying NEED_HIGHP vec2 vLargeTexCoord;
#ifdef GL_FRAGMENT_PRECISION_HIGH
   #define NEED_HIGHP highp
#else
   #define NEED_HIGHP mediump
#endif


// Enable supported extensions
// This no longer supported on Droid.
// #extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform sampler2D g_DepthTexture;
uniform vec2 g_NearQ;
uniform vec4 g_FogColor;
uniform vec3 g_FogParams;


void main()
{
    vec4 Color = texture2D( g_ColorTexture, g_TexCoord );
    vec4 Depth = texture2D( g_DepthTexture, g_TexCoord );

    float FogStart = g_FogParams.x;
    float FogEnd = g_FogParams.x + g_FogParams.y;
    float FogDensity = g_FogParams.z;

    // Q = Zf / Zf - Zn
    // z = -Zn * Q / Zd - Q
    float Zd = Depth.r;
    float Zn = g_NearQ.x;
    float Q = g_NearQ.y;
    float WorldDepth = -( Zn * Q ) / ( Zd - Q );

    // calculate fog amount based on pixel depth
    float Lerpval = smoothstep( FogStart, FogEnd, WorldDepth ) * FogDensity;
    gl_FragColor = mix( Color, g_FogColor, Lerpval );
}
