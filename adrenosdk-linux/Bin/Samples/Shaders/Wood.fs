

//--------------------------------------------------------------------------------------
// File: Wood.glsl
// Desc: Wood shader
//
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
#extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



#extension GL_OES_texture3D : enable
uniform sampler3D g_sNoise;

uniform float g_Kd;
uniform float g_Ks;
uniform vec4  g_vDarkWoodColor;
uniform vec4  g_vLightWoodColor;
uniform float g_fFrequency;
uniform float g_fNoiseScale;
uniform float g_fRingScale;
uniform vec4  g_vLightDir;

varying vec3 g_vOutScaledPosition;
varying vec3 g_vOutNormalES;
varying vec3 g_vOutViewVec;

void main()
{
    // Signed noise
    float fSNoise = texture3D( g_sNoise, g_vOutScaledPosition ).x;

    // Rings will go along z-axis, perturbed with some noise
    float fRing = fract( g_fFrequency * g_vOutScaledPosition.z + g_fNoiseScale * fSNoise );
    // Map [0, 1] to 0->1->0 with 4(x-x^2)
    fRing *= 4.0 * ( 1.0 - fRing );

    // Adjust ring smoothness and shape, and add some noise
    float fLrp = pow( fRing, g_fRingScale ) + fSNoise;
    vec4 fBase = mix( g_vDarkWoodColor, g_vLightWoodColor, fLrp );

    vec3 vNormal = normalize( g_vOutNormalES );
    // Soft diffuse
    float fDiffuse = 0.5 + 0.5 * dot( g_vLightDir.xyz, vNormal );
    // Standard specular
    float fSpecular = pow( clamp( dot( reflect( -normalize( g_vOutViewVec ), vNormal ), g_vLightDir.xyz ), 0.0, 1.0 ), 12.0 );

    gl_FragColor = g_Kd * fDiffuse * fBase + g_Ks * fSpecular;
}


