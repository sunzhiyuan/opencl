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
uniform mat4 g_MatModelViewProjInv;
uniform vec3 g_CamPos;
uniform vec4 g_FogColor;
uniform vec3 g_FogParams;


void main()
{
    vec4 Color = texture2D( g_ColorTexture, g_TexCoord );
    vec4 Depth = texture2D( g_DepthTexture, g_TexCoord );

    float FogTop = g_FogParams.x;
    float FogBottom = g_FogParams.y;
    float FogDensity = g_FogParams.z;

    // recalculate world position of this pixel
    vec4 ScreenPos = vec4( g_TexCoord.x * 2.0 - 1.0, g_TexCoord.y * 2.0 - 1.0, Depth.r, 1.0 );
    vec4 WorldPos = ScreenPos * g_MatModelViewProjInv;
    WorldPos.xyz /= WorldPos.w;

    // calculate fog amount based on world y and whether the camera is itself in the fog
    float FogAmount = smoothstep( FogTop, FogBottom, WorldPos.y );
    float CameraFog = smoothstep( FogTop, FogBottom, g_CamPos.y );
    gl_FragColor = mix( Color, g_FogColor, max( FogAmount, CameraFog ) * FogDensity );
}
