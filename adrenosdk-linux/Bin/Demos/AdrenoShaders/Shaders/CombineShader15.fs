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



// input from the vs
varying vec2 g_TexCoords0;
varying vec2 g_TexCoords1;
varying vec2 g_TexCoords2;
varying vec2 g_TexCoords3;

// ps variables
uniform sampler2D g_NormalTexture;
uniform sampler2D g_HatchTexture;
uniform float g_SlopeBias;

void main()
{
    vec4 HatchPixel = texture2D( g_HatchTexture, g_TexCoords0 );

    // sample the target pixel and some neighbors
    vec4 normal0 = texture2D( g_NormalTexture, g_TexCoords0 );
    vec4 normal1 = texture2D( g_NormalTexture, g_TexCoords1 );
    vec4 normal2 = texture2D( g_NormalTexture, g_TexCoords2 );
    vec4 normal3 = texture2D( g_NormalTexture, g_TexCoords3 );

    // calculate horizontal and vertical changes
    vec4 slope1 = ( ( normal0 + normal1 ) - ( normal2 + normal3 ) ) * g_SlopeBias;
    vec4 slope2 = ( ( normal0 + normal2 ) - ( normal1 + normal3 ) ) * g_SlopeBias;

    // threshold function
    vec4 x = slope1 * slope1 + slope2 * slope2;
    float y = 1.0 - length( x );
    if( y < 0.8 )
    {
        y = 0.6;
    }
    gl_FragColor = vec4( HatchPixel.rgb * y, 1.0 );
}
