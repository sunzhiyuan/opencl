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



uniform sampler2D g_Texture;
varying vec2 g_TexCoord;

vec4 ConvertRGBtoHSV( vec4 Value )
{
    const float PI2 = 6.283185307179586476925286766559;

    float RG = Value.r - Value.g;
    float RB = Value.r - Value.b;

    // Hue
    float H = acos( ( RG + RB ) / ( 2.0 * sqrt( RG * RG + RB * ( Value.g - Value.b ) ) ) ) / PI2;
    if( Value.b > Value.g )
    {
        H = 1.0 - H;
    }

    // Intensity
    float I = dot( Value.rgb, vec3( 1.0 ) ) / 3.0;

    // Saturation
    float S = 1.0 - min( min( Value.r, Value.g ), Value.b ) / I;

    return vec4( H, S, I, 0.0 );
}

void main()
{
    vec4 Color = texture2D( g_Texture, g_TexCoord );
    gl_FragColor = ConvertRGBtoHSV( Color );
}

