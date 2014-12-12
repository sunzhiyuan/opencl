//--------------------------------------------------------------------------------------
// File: EyeShader.glsl
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
varying vec3 g_CubeTexcoord;

uniform samplerCube g_Cubemap;
uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;
uniform vec4 g_SpecColor;
uniform vec4 g_AmbientColor;
uniform float g_ReflectionStrength;
uniform float g_IrisGlow;


void main()
{
    vec2 UV = g_Tex * vec2( 1.0, 1.0 );
    vec4 Color = texture2D( g_DiffuseTexture, UV );
    vec4 N1 = vec4( 0.0, 0.0, 1.0, 1.0 );
    vec4 N2 = texture2D( g_BumpTexture, g_Tex.xy ) * 2.0 - 1.0;
    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );
    vec3 H = normalize( L + V );
    float NL = dot( N1.xyz, L );
    float NH = max( 0.0, dot( N1.xyz, H ) );

    // Color.w = iris cutout
    float Iris = Color.w;

    // Normal.w = wetness
    float Wetness = N2.w * 0.5 + 0.5;

    // Specular
    float Specular = min( 1.0, pow( NH, g_SpecColor.w ) );
    float SelfShadow = saturate( 4.0 * NL );

    // Cube mapping
    vec4 CubeColor = textureCube( g_Cubemap, g_CubeTexcoord ) * Wetness;

    // Iris highlight
    float IrisLight = max( 0.0, dot( N2.xyz, L ) ) * g_IrisGlow * Iris;
    vec4 IrisColor = Color * IrisLight;

    gl_FragColor = Color * g_AmbientColor;
    gl_FragColor.rgb += Color.rgb * max( 0.0, NL ) * 0.8;
    gl_FragColor.rgb += IrisColor.rgb;
    gl_FragColor.rgb += CubeColor.rgb * g_ReflectionStrength;
    gl_FragColor.rgb += SelfShadow * g_SpecColor.rgb * Specular;
}
