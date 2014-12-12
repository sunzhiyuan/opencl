//--------------------------------------------------------------------------------------
// File: StraussShader.glsl
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



varying vec3 g_Normal;
varying vec4 g_WorldPos;
varying vec2 g_TexCoord;

uniform vec3 g_LightDir;
uniform vec3 g_CameraPos;
uniform float g_Smoothness;
uniform float g_Metalness;
uniform float g_Transparency;
uniform sampler2D g_DiffuseTexture;
uniform vec4 g_DiffuseColor;


void main()
{
    const float k = 0.1;
    const float kf = 1.12;
    const float ks = 1.01;

    // Make sure the interpolated inputs and constant parameters are normalized
    vec4 Color = texture2D( g_DiffuseTexture, g_TexCoord ) * g_DiffuseColor;
    vec3 N = normalize( g_Normal );
    vec3 L = normalize( g_LightDir );
    vec3 V = normalize( g_CameraPos - g_WorldPos.xyz );
    vec3 H = reflect( L, N );

    // Declare any aliases:
    float NL = max( 0.0, dot( N, L ) );
    float NV = max( 0.0, dot( N, V ) );
    float HV = max( 0.0, dot( H, V ) );

    float FresnelA = ( 1.0 / ( kf * kf ) );
    float fNL = ( 1.0 / ( ( NL - kf ) * ( NL - kf ) ) - FresnelA ) / ( 1.0 / ( ( 1.0 - kf ) * ( 1.0 - kf ) ) - FresnelA );
    float s_cubed = g_Smoothness * g_Smoothness * g_Smoothness;
 
    // Evaluate the diffuse term
    float D = ( 1.0 - g_Metalness * g_Smoothness );
    float RD = ( 1.0 - s_cubed ) * ( 1.0 - g_Transparency );
    vec3 Diffuse = NL * D * RD * Color.rgb;
 
    // Compute the inputs into the specular term
    float R = ( 1.0 - g_Transparency ) - RD;
 
    // Brown's distribution function
    float ShadowA = 1.0 / ( ( 1.0 - ks ) * ( 1.0 - ks ) );
    float ShadowNL = ( ShadowA - 1.0 / ( ( NL - ks ) * ( NL - ks ) ) ) / ( ShadowA - 1.0 / ( ks * ks ) );
    float ShadowNV = ( ShadowA - 1.0 / ( ( NV - ks ) * ( NV - ks ) ) ) / ( ShadowA - 1.0 / ( ks * ks ) );
    float J = fNL * ShadowNL * ShadowNV;

    // 'k' is used to provide small off-specular peak for very rough surfaces.
    // Can be changed to suit desired results...
    float SelfShadow = min( 1.0, R + J * ( R + k ) );

    // Evaluate the specular term
    vec3 C1 = vec3( 1.0 );
    vec3 CS = C1 + g_Metalness * ( 1.0 - fNL ) * ( Diffuse - C1 );
    vec3 Specular = CS * SelfShadow;
    Specular *= pow( HV, 3.0 / ( 1.0 - g_Smoothness ) );
 
    // Composite the final result, ensuring the values are >= 0.0f yields better
    // results. Some combinations of inputs generate negative values which looks
    // wrong when rendered...
    Diffuse  = max( vec3( 0.0 ), Diffuse );
    Specular = max( vec3( 0.0 ), Specular );
    gl_FragColor = vec4( Diffuse + Specular, 1.0 );
}
