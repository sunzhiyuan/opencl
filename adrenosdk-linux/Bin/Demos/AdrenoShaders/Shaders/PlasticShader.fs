//--------------------------------------------------------------------------------------
// File: PlasticShader.glsl
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

uniform samplerCube g_Cubemap;
uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;
uniform vec4 g_DiffuseColor;
uniform vec4 g_SpecularColor;
uniform vec4 g_AmbientLight;
uniform float g_ReflectionStrength;


void main()
{
    vec4 Color = texture2D( g_DiffuseTexture, g_Tex.xy ) * g_DiffuseColor;
    vec4 N = texture2D( g_BumpTexture, g_Tex.xy ) * 2.0 - 1.0;
    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );
    vec3 Half = normalize( L + V );
    float NL = max( 0.0, dot( N.xyz, L ) );
    float VN = max( 0.0, dot( N.xyz, V ) );

    // specular shine
    float Specular = pow( max( 0.0, dot( N.xyz, Half ) ), g_SpecularColor.a );
	float SelfShadow = 4.0 * NL;

    // cube mapping
    vec3 CubeTexcoord = reflect( -V, N.xyz );
    vec4 CubeColor = textureCube( g_Cubemap, CubeTexcoord );

 	gl_FragColor = g_AmbientLight;
    gl_FragColor += Color * vec4( NL * Color.w );
    gl_FragColor.rgb += (CubeColor.rgb * g_ReflectionStrength);
    gl_FragColor.rgb += SelfShadow * vec3(0.15, 0.15, 0.15) * g_SpecularColor.rgb * Specular;
}
