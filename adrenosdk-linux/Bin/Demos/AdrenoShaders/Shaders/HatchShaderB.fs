
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



// inputs from the vs
varying vec3 g_vNormalES;
varying vec4 g_vOutTexCoord;

// shader variables
uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_HatchTexture1;
uniform sampler2D g_HatchTexture2;
uniform vec3      g_LightPos;
uniform float     g_Ambient;

void main()
{
    vec3 vNormal = normalize( g_vNormalES );
    vec3 vLight = normalize( g_LightPos );
    vec4 HatchTex1 = texture2D( g_HatchTexture1, g_vOutTexCoord.zw );
    vec4 HatchTex2 = texture2D( g_HatchTexture2, g_vOutTexCoord.zw );
    vec4 DiffuseTex = texture2D( g_DiffuseTexture, g_vOutTexCoord.xy );
    float Diffuse = dot( vLight, vNormal );

    // calculate luminance
    float sum = saturate( saturate( Diffuse * DiffuseTex.r ) + g_Ambient );

    // maps tones to channels in the hatch textures
//     vec4 HatchWeights1 = vec4( 1.0, 0.857, 0.714, 0.571 );
//     vec4 HatchWeights2 = vec4( 0.429, 0.286, 0.143, 0.0 );
    vec4 HatchWeights1 = vec4( 1.000, 0.888, 0.777, 0.555 );
    vec4 HatchWeights2 = vec4( 0.333, 0.222, 0.111, 0.000 );

    // normalize to within 0.125 from each luminance anchor
//     vec4 x = vec4( 1.0 ) - saturate( abs( ( HatchWeights1 - vec4( sum ) ) * vec4( 7.0 ) ) );
//     vec4 y = vec4( 1.0 ) - saturate( abs( ( HatchWeights2 - vec4( sum ) ) * vec4( 7.0 ) ) );
    vec4 x = vec4( 1.0 ) - saturate( abs( ( HatchWeights1 - vec4( sum ) ) * vec4( 9.0 ) ) - vec4( 0.0, 0.0, 0.0, 1.0 ) );
    vec4 y = vec4( 1.0 ) - saturate( abs( ( HatchWeights2 - vec4( sum ) ) * vec4( 9.0 ) ) - vec4( 0.0, 0.0, 0.0, 0.0 ) );

    // sum up all pixel weights
    sum = saturate( dot( HatchTex1, x ) + dot( HatchTex2, y ) );

    // reduce contrast + boost luminance
    sum = sum * (0.6) + 0.35;

    // color tint
    gl_FragColor = vec4( 0.7216, 0.7098, 0.6902, 1.0 ) * vec4( sum, sum, sum, 1.0 );
}
