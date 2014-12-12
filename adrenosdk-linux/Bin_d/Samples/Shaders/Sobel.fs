

//--------------------------------------------------------------------------------------
// File: Sobel.glsl
// Desc: Sobel shader for the ImageEffects sample
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//
// A Sobel filter is a good edge detection filter, though is requires more samples 
// than the Laplace filter.  It computes the first order derivates in X and Y direction.
// These derivates can be used to find edge slope (as is done by this shader), and to 
// find the edge direction if desired.
//
// The Sobel filter is evaluated by these filter kernels:
//
// 	        [1 0 -1]          [ 1  2  1]
//	SobelX: [2 0 -2]  SobelY: [ 0  0  0]
//	        [1 0 -1]          [-1 -2 -1]
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
varying vec2 g_vTexCoords2;
varying vec2 g_vTexCoords3;

void main()
{
    vec4 s0 = texture2D( g_sImageTexture, g_vTexCoords0 );
    vec4 s1 = texture2D( g_sImageTexture, g_vTexCoords1 );
    vec4 s2 = texture2D( g_sImageTexture, g_vTexCoords2 );
    vec4 s3 = texture2D( g_sImageTexture, g_vTexCoords3 );

    vec4 vSobelX = 4.0 * ( ( s0 + s2 ) - ( s1 + s3 ) );
    vec4 vSobelY = 4.0 * ( ( s0 + s1 ) - ( s2 + s3 ) );

    // Compute the vector length
    vec4 vSobelSqr = vSobelX * vSobelX + vSobelY * vSobelY;
    float fSobel   = length(vSobelSqr);
    if (fSobel > 0.2)
    {
        fSobel = 1.0;
    }

    gl_FragColor = vec4( clamp( vec3( 1.0 - fSobel ), 0.0, 1.0 ), 1.0 );
}
