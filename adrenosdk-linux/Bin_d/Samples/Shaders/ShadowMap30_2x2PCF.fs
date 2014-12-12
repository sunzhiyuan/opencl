

//--------------------------------------------------------------------------------------
// File: ShadowMap.glsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The fragment shader with a 2x2 PCF filter
//--------------------------------------------------------------------------------------

#version 300 es

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



uniform sampler2DShadow g_sShadowMap;
uniform sampler2D g_sDiffuseSpecularMap;
uniform vec3 g_vAmbient;
uniform float g_fEpsilon;

in vec4 g_vOutShadowCoord;
in vec4 g_vOutScreenCoord;

out vec4 g_FragColor;

float lookup( float x, float y )
{
    vec4 vOffset = vec4( x, y, 0.0, 0.0 ) * g_vOutShadowCoord.w * g_fEpsilon;
    return textureProj( g_sShadowMap, g_vOutShadowCoord + vOffset );
}

void main()
{
    float fSum = 0.0;

    fSum += lookup( -0.5, -0.5 );
    fSum += lookup( -0.5,  0.5 );
    fSum += lookup(  0.5, -0.5 );
    fSum += lookup(  0.5,  0.5 );

    vec4 vDiffuseAndSpecular = textureProj( g_sDiffuseSpecularMap, g_vOutScreenCoord );
    g_FragColor = vec4( g_vAmbient, 0.0 ) + 0.25 * fSum * vDiffuseAndSpecular;
}

