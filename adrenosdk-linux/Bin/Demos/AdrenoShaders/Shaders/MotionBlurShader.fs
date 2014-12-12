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
