//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The fragment shader
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



uniform sampler2D g_CIETexture;
varying vec2 g_TexCoord;

vec4 ConvertCIEtoRGB( vec4 Value )
{
    vec4 Yxy = Value;

    // Yxy -> XYZ conversion
    vec3 XYZ;
    XYZ.r = Yxy.r * Yxy.g / Yxy.b;                      // X = Y * x / y
    XYZ.g = Yxy.r;                                      // copy luminance Y
    XYZ.b = Yxy.r * ( 1.0 - Yxy.g - Yxy.b ) / Yxy.b;    // Z = Y * ( 1.0 - x - y ) / y

    // XYZ -> RGB conversion
    mat3 XYZ2RGB;
    XYZ2RGB[0] = vec3(  2.5651, -1.1665, -0.3986 );
    XYZ2RGB[1] = vec3( -1.0217,  1.9777,  0.0439 );
    XYZ2RGB[2] = vec3(  0.0753, -0.2543,  1.1892 );
    vec3 RGB = XYZ * XYZ2RGB;

    return vec4( RGB, 1.0 );
}

void main()
{
    vec4 Color = texture2D( g_CIETexture, g_TexCoord );
    gl_FragColor = ConvertCIEtoRGB( Color );
}
