

//--------------------------------------------------------------------------------------
// File: Emboss.glsl
// Desc: Emboss shader for the ImageEffects sample
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//
//    An emboss filter. The filter kernel looks like this:
//
//    [1  1  0]
//    [1  0 -1]
//    [0 -1 -1]
//
//    By using linear filtering and sampling exactly at the upper left and lower right
//    corner of the pixel we can evaluate the entire filter with just two samples by 
//    subtracting the samples and multiplying by four.
//
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

varying vec2 g_vTexCoords0;
varying vec2 g_vTexCoords1;

void main()
{
    vec4 vEmboss = 4.0 * ( texture2D( g_sImageTexture, g_vTexCoords0 ) - texture2D( g_sImageTexture, g_vTexCoords1 ) );

    // Offset with 0.5 to show both positive and negative response
    gl_FragColor = vEmboss + 0.5;
}
