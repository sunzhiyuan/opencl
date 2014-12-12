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

uniform sampler2D g_RayTexture;
uniform vec2 g_ScreenLightPos;
uniform vec3 g_RayParams;


void main()
{
    float Aspect = g_RayParams.x;
    float RayStart = g_RayParams.y;
    float RayLength = g_RayParams.z;
    vec2 ScreenPos = ( g_TexCoord * 2.0 - 1.0 ) * vec2( Aspect, 1.0 );
    vec2 RayVector = ( g_ScreenLightPos - ScreenPos );
    vec2 RayOffset = RayVector * RayStart;
    RayVector *= RayLength;

    vec4 BaseColor = texture2D( g_RayTexture, g_TexCoord );
    vec4 Color = BaseColor;
    Color += max( BaseColor, texture2D( g_RayTexture, g_TexCoord + RayOffset + RayVector * 0.33 ) );
    Color += max( BaseColor, texture2D( g_RayTexture, g_TexCoord + RayOffset + RayVector * 0.66 ) );
    Color += max( BaseColor, texture2D( g_RayTexture, g_TexCoord + RayOffset + RayVector * 1.0 ) );

    Color *= 0.25;

    gl_FragColor = Color;
}
