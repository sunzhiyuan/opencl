

//--------------------------------------------------------------------------------------
// File: Sharpen.glsl
// Desc: Sharpen shader for the ImageEffects sample
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

varying vec2 g_vTexCoords11;
varying vec2 g_vTexCoords00;
varying vec2 g_vTexCoords02;
varying vec2 g_vTexCoords20;
varying vec2 g_vTexCoords22;

void main()
{
    vec4 vSharp11 = texture2D( g_sImageTexture, g_vTexCoords11 );
    vec4 vSharp00 = texture2D( g_sImageTexture, g_vTexCoords00 );
    vec4 vSharp02 = texture2D( g_sImageTexture, g_vTexCoords02 );
    vec4 vSharp20 = texture2D( g_sImageTexture, g_vTexCoords20 );
    vec4 vSharp22 = texture2D( g_sImageTexture, g_vTexCoords22 );

    // Sharpen by increasing local contrast.
    // This is basically the image, plus the four differences between the pixel and it's neighbors.
    vec4 vSharp = 5.0 * vSharp11 - ( vSharp00 + vSharp02 + vSharp20 + vSharp22 );

    gl_FragColor = vSharp;
}
