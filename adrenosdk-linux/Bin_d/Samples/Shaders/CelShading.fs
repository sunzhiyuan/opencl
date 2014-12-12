

//--------------------------------------------------------------------------------------
// File: CelShading.glsl
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



uniform sampler2D g_sCelShadingTexture;
uniform sampler2D g_sTexture;
uniform vec3 g_vLightPos;
uniform vec3 g_vAmbient;

varying vec3 g_vOutNormal;
varying vec2 g_vOutTexCoord;

void main()
{
    float vAngle          = max( 0.0, dot( normalize( g_vOutNormal ), normalize( g_vLightPos ) ) );
    vec4 vCelShadingColor = texture2D( g_sCelShadingTexture, vec2( vAngle, 0.0 ) );
    vec4 vTexColor        = texture2D( g_sTexture, g_vOutTexCoord );

    gl_FragColor = vCelShadingColor * ( vTexColor + vec4( g_vAmbient, 0.0 ) );
} 
