//--------------------------------------------------------------------------------------
// File: ClothShader.glsl
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



varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_View;

uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;
uniform vec4 g_SpecularColor;
uniform vec4 g_AmbientLight;
uniform float g_MinnaertExponent;


void main()
{
    const float PI = 3.1415926535897932384626433832795;

    vec2 UV = g_Tex;
    vec4 Color = texture2D( g_DiffuseTexture, UV * 4.0);
    vec4 N = texture2D( g_BumpTexture, UV ) * 2.0 - 1.0;
    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );

    float NL = max( 0.0, dot( N.xyz, L ) );
    float VN = max( 0.0, dot( N.xyz, V ) );

    // General Minnaert formula:
    // Result = Color * (cos(NL)^k * cos(VN)^(1-k))
    // float A = saturate( pow( NL, g_MinnaertExponent ) );
    // float B = saturate( pow( VN, 1.0 - g_MinnaertExponent ) );
    // float Diffuse = ( A * B );

    // Enhanced Minnaert:
    float Diffuse = NL * pow( max( NL * VN, 0.1 ), 1.0 - g_MinnaertExponent );

    vec3 AmbientColor = Color.rgb * g_AmbientLight.rgb;
    gl_FragColor = vec4( AmbientColor + Color.rgb * Diffuse * Color.w, 1.0 );
}
