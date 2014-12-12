//--------------------------------------------------------------------------------------
// File: CubemapReflection.glsl
// Desc: Reflection shaders for the CubemapReflection sample
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
#extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



const vec3  g_vAmbient   = vec3(0.2, 0.2, 0.2);
const vec3  g_vDiffuse   = vec3(1.0, 1.0, 0.1);
const vec3  g_vSpecular  = vec3(1.0, 1.0, 1.0);
const float g_fShininess = 64.0;

uniform samplerCube g_RefMap;
uniform samplerCube g_EnvMap;
uniform vec3 g_vLightPos;

varying vec3 g_vNormalES;
varying vec3 g_vViewVecES;
varying vec3 g_vTexcoord;

void main()
{
    // Normalize per-pixel vectors	
    vec3 vNormal = normalize( g_vNormalES );
    vec3 vLight  = normalize( g_vLightPos );
    vec3 vView   = normalize( g_vViewVecES );
    vec3 vHalf   = normalize( vLight + vView );

    // Compute the lighting in eye-space
    float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );
    float fSpecular = pow( max( 0.0, dot( vNormal, vHalf ) ), g_fShininess );

    // Combine lighting with the material properties
    vec4 vColor;
    vColor.rgb = g_vAmbient + g_vDiffuse*fDiffuse + g_vSpecular*fSpecular;
    vColor.a    = 1.0;

    vec4 vRefColor = textureCube( g_RefMap, g_vTexcoord );
    vec4 vEnvColor = textureCube( g_EnvMap, g_vTexcoord );

    gl_FragColor = vColor * ( vEnvColor * ( 1.0 - vRefColor.a ) + ( vRefColor * vRefColor.a ) );
}
