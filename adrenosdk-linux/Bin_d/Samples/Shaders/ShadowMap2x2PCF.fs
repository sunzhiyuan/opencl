

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



uniform sampler2D g_sShadowMap;
uniform sampler2D g_sDiffuseSpecularMap;
uniform vec3 g_vAmbient;
uniform float g_fEpsilon;

varying vec4 g_vOutShadowCoord;
varying vec4 g_vOutScreenCoord;

float lookup( float x, float y )
{
    vec4 vOffset = vec4( x, y, 0.0, 0.0 ) * g_vOutShadowCoord.w * g_fEpsilon;
    float fLightDepth = ( texture2DProj( g_sShadowMap, g_vOutShadowCoord + vOffset ) ).r;
    float fBias = 0.03;
    return (g_vOutShadowCoord.z / g_vOutShadowCoord.w - fBias) > fLightDepth ? 0.0 : 1.0;
}

void main()
{
    float fSum = 0.0;

 //   fSum += lookup( 0.0, 0.0 );

    fSum += lookup( -0.5, -0.5 );
    fSum += lookup( -0.5,  0.5 );
    fSum += lookup(  0.5, -0.5 );
    fSum += lookup(  0.5,  0.5 );

    vec4 vDiffuseAndSpecular = texture2DProj( g_sDiffuseSpecularMap, g_vOutScreenCoord );
    gl_FragColor = vec4( g_vAmbient, 0.0 ) + 0.25 * fSum * vDiffuseAndSpecular;
}

