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

vec4 ConvertRGBtoCIE( vec4 Value )
{
    // RGB -> XYZ conversion
    mat3 RGB2XYZ;
    RGB2XYZ[0] = vec3( 0.5141364, 0.3238786,  0.16036376 );
    RGB2XYZ[1] = vec3( 0.265068,  0.67023428, 0.06409157 );
    RGB2XYZ[2] = vec3( 0.0241188, 0.1228178,  0.84442666 );
    vec3 XYZ = Value.rgb * RGB2XYZ;

    // XYZ -> Yxy conversion
    vec3 Yxy;
    Yxy.r = XYZ.g;  // copy luminance Y

    // prevent division by zero to keep the compiler happy
    float temp = max( dot( vec3( 1.0, 1.0, 1.0 ), XYZ.rgb ), 0.000001 );
    Yxy.g = XYZ.r / temp;
    Yxy.b = XYZ.g / temp;

    return vec4( Yxy.r, Yxy.g, Yxy.b, 0.0 );
}

void main()
{
    vec4 Color = texture2D( g_Texture, g_TexCoord );
    gl_FragColor = ConvertRGBtoCIE( Color );
}

