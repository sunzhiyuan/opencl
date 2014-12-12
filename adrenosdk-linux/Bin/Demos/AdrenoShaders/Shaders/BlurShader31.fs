//--------------------------------------------------------------------------------------
// File: BlurShader.glsl
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



varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
uniform float g_GaussWeight[4];
uniform float g_GaussInvSum;

void main()
{
    vec2 UV = g_TexCoord;

    // 4 tap gaussian distribution
    vec4 Color = g_GaussWeight[0] * texture2D( g_ColorTexture, UV + g_StepSize );
    Color += g_GaussWeight[1] * texture2D( g_ColorTexture, UV + g_StepSize * 0.5 );
    Color += g_GaussWeight[2] * texture2D( g_ColorTexture, UV - g_StepSize * 0.5 );
    Color += g_GaussWeight[3] * texture2D( g_ColorTexture, UV - g_StepSize );

    gl_FragColor = ( Color * g_GaussInvSum );
    //gl_FragColor = vec4( Color.r * g_GaussInvSum, 0.0, texture2D( g_ColorTexture, UV ).b, 1.0 );
}
