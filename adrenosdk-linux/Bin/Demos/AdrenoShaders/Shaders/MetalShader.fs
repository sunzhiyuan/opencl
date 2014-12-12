//--------------------------------------------------------------------------------------
// File: MetalShader.glsl
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



varying vec2 g_TexCoord;
varying vec4 g_Light;
varying vec3 g_View;

uniform float g_Roughness;
uniform float g_Reflection;       // Fresnel reflection index
uniform float g_DiffuseIntensity;
uniform float g_SpecularIntensity;
uniform float g_UVPull;
uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;


void main()
{
    // Using Cook-Torrance lighting model

    vec2 UV = g_TexCoord * vec2( 1.0, g_UVPull );
    vec4 Color = texture2D( g_DiffuseTexture, UV );
    vec4 N = texture2D( g_BumpTexture, UV ) * 2.0 - 1.0;


    float m = g_Roughness;
    float RI = g_Reflection;
    const float PI = 3.1415926535;
   
    // Normalize per-pixel vectors	
    vec3 L = normalize( g_Light.xyz );
    vec3 V = normalize( g_View );
    vec3 H = normalize( L + V );

	float NH = max( 0.0, dot( N.xyz, H ) );
    float VH = max( 0.0, dot( V, H ) );
	float NV = max( 0.0, dot( N.xyz, V ) );
	float NL = max( 0.0, dot( L, N.xyz ) );

    // Beckmann's distribution function
    float NH2 = NH * NH;
    float m2 = m * m;
    float D = ( 1.0 / m2 * NH2 * NH2 ) * (exp( -( ( 1.0 - NH2 ) / ( m2 * NH2 ) ) ) );

    // Fresnel
    float F = RI + ( 1.0 - RI ) * saturate( pow( ( 1.0 - NV ), 5.0 ) );

    // geometric term
    float G = min( 1.0, min( ( 2.0 * NH * NL ) / VH, ( 2.0 * NH * NV ) / VH ) );

    float S = ( F * D * G ) / ( PI * NL * NV );

	//------------------------------
	// Original: Ir = Ai * Ac + Ii(N.L)dw * (Si * Sc + Di * Dc)
	// My formula: Ir = Ia * C + N.L * sat(lidw * (Di * Dc * C + Si * Sc))
	//------------------------------
	const float Ai = 0.1;
	const float dw = 3.0;
    float Di = g_DiffuseIntensity;
    float Si = g_SpecularIntensity * Color.w;
    gl_FragColor.rgba  = Ai * Color + ( NL * saturate( dw * ( ( Di * NL * Color ) + ( Si * S ) ) ) ) * g_Light.w;
}
