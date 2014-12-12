//--------------------------------------------------------------------------------------
// File: BlinnPhongShader.glsl
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



varying vec3 g_ViewOut;
varying vec3 g_NormalOut;
varying vec2 g_TexOut;

uniform sampler2D g_DiffuseTexture;
uniform vec4 g_DiffuseColor;
uniform vec4 g_SpecularColor;
uniform vec4 g_AmbientLight;
uniform vec3 g_LightDir;


void main()
{
    // Normalize per-pixel vectors	
    vec3 vNormal = normalize( g_NormalOut );
    vec3 vLight  = normalize( g_LightDir );
    vec3 vView   = normalize( g_ViewOut );
    vec3 vHalf   = normalize( vLight + vView );
    vec4 C = texture2D( g_DiffuseTexture, g_TexOut.xy );

    // Compute the lighting in world space
    float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );
    float fSpecular = pow( max( 0.0, dot( vNormal, vHalf ) ), g_SpecularColor.a );
	float SelfShadow = 4.0 * fDiffuse;

    // Combine lighting with the material properties
    gl_FragColor.rgba  = g_AmbientLight;
    gl_FragColor.rgba += g_DiffuseColor.rgba * fDiffuse * C;
    gl_FragColor.rgb  += SelfShadow * vec3(0.15, 0.15, 0.15) * g_SpecularColor.rgb * fSpecular;
}
