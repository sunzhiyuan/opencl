

//--------------------------------------------------------------------------------------
// File: Gaussian.glsl
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
#extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



uniform sampler2D g_sImageTexture;

varying vec2 g_vTexCoordM;
varying vec2 g_vTexCoordB0;
varying vec2 g_vTexCoordF0;
varying vec2 g_vTexCoordB1;
varying vec2 g_vTexCoordF1;
varying vec2 g_vTexCoordB2;
varying vec2 g_vTexCoordF2;

void main()
{
    vec4 vSampleM  = texture2D( g_sImageTexture, g_vTexCoordM );
    vec4 vSampleB0 = texture2D( g_sImageTexture, g_vTexCoordB0 );
    vec4 vSampleF0 = texture2D( g_sImageTexture, g_vTexCoordF0 );
    vec4 vSampleB1 = texture2D( g_sImageTexture, g_vTexCoordB1 );
    vec4 vSampleF1 = texture2D( g_sImageTexture, g_vTexCoordF1 );
    vec4 vSampleB2 = texture2D( g_sImageTexture, g_vTexCoordB2 );
    vec4 vSampleF2 = texture2D( g_sImageTexture, g_vTexCoordF2 );

    gl_FragColor = 0.1752 * vSampleM + 
                   0.1658 * ( vSampleB0 + vSampleF0 ) + 
                   0.1403 * ( vSampleB1 + vSampleF1 ) + 
                   0.1063 * ( vSampleB2 + vSampleF2 );
}
