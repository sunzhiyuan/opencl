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



// inputs from the vs
varying vec3     g_vViewVecES;
varying vec3     g_vNormalES;

struct MATERIAL
{
    vec4  vAmbient;
    vec4  vDiffuse;
    vec4  vSpecular;
};

uniform MATERIAL  g_Material;
uniform vec3      g_vLightPos;
uniform sampler2D g_ColorRamp;

void main()
{
    vec3 vNormal = normalize( g_vNormalES );
    vec3 vHalf   = normalize( g_vLightPos + g_vViewVecES );

    // Calculate general intensity
    float diffuse = dot( g_vViewVecES, vNormal );
 	float specular = pow( max( 0.0, dot( vNormal, vHalf ) ), g_Material.vSpecular.a );
    float intensity = saturate( ( diffuse + specular ) * 0.75 );

    // Use the color ramp texture to create a banded look
    vec2 colorRampUV = vec2( saturate( intensity ), 0.0);
    gl_FragColor = vec4( texture2D( g_ColorRamp, colorRampUV ).rgb, 1.0);
}
