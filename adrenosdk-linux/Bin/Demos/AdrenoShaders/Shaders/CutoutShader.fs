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
uniform vec4 g_OccluderParams;
uniform vec2 g_ScreenLightPos;
uniform vec3 g_NearQAspect;


void main()
{
    float Aspect = g_NearQAspect.z;
    vec2 ScreenPos = ( g_TexCoord * 2.0 - 1.0 ) * vec2( Aspect, 1.0 );
    float OccluderThreshold = g_OccluderParams.x;
    float OccluderFadeout = g_OccluderParams.y;
    float LightRadius = g_OccluderParams.z;
    float LightPenumbra = g_OccluderParams.w;

    vec2 LightScreenVector = g_ScreenLightPos - ScreenPos;

    vec4 Color = texture2D( g_ColorTexture, g_TexCoord + LightScreenVector.xy );
    vec4 Depth = texture2D( g_DepthTexture, g_TexCoord );

    // Q = Zf / Zf - Zn
    // z = -Zn * Q / Zd - Q
    float Zd = Depth.r;
    float Zn = g_NearQAspect.x;
    float Q = g_NearQAspect.y;
    float WorldDepth = -( Zn * Q ) / ( Zd - Q );

    // find pixels that pass the depth test.  these ones are open spaces, and become
    // the roots of the god rays.
    float DepthVal = smoothstep( OccluderThreshold - OccluderFadeout, OccluderThreshold, WorldDepth );

    // attenuation
    float Attenuation = smoothstep( LightRadius + LightPenumbra, LightRadius, length( LightScreenVector ) );

    gl_FragColor = vec4( DepthVal * Attenuation ) * Color;
}
