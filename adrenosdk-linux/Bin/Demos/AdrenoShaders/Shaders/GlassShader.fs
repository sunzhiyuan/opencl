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



uniform sampler2D g_BackgroundTexture;
uniform vec4 g_GlassParams;

varying vec4 g_TexCoord;
varying vec3 g_LightVec;
varying vec3 g_ViewVec;
varying vec3 g_Normal;


void main()
{
    vec2 UV             = g_TexCoord.xy;
    vec2 ScreenUV       = g_TexCoord.zw * 1.0;
    vec3 Normal         = normalize( g_Normal );
    vec3 Light          = normalize( g_LightVec );
    vec3 View           = normalize( g_ViewVec );
    vec3 Half           = normalize( Light + View );
    float NV            = max( 0.0, dot( Normal, View ) );
    vec3 BendVec        = reflect( -View, Normal );
    vec3 GlassColor     = g_GlassParams.xyz;
    float Distort       = g_GlassParams.w;

    vec4 BGColor    = texture2D( g_BackgroundTexture, ScreenUV - BendVec.xy * Distort );
    vec4 EnvColor   = texture2D( g_BackgroundTexture, ScreenUV + BendVec.xy * Distort );

    // Fresnel
    float F = saturate( pow( ( 1.0 - NV ), 2.0 ) );

    // Specular
    float Specular  = pow( saturate( dot( Half, Normal ) ), 100.0 ) * 4.0;

    // refraction + env + spec
    gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
    gl_FragColor.rgb += BGColor.rgb * GlassColor * ( 1.0 - F );
    gl_FragColor.rgb += ( EnvColor.rgb + GlassColor * 0.5 ) * F;
    gl_FragColor.rgb += vec3( Specular * 0.7 );
}
