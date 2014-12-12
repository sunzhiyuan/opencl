

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



uniform sampler2D g_sShadowMap;
uniform sampler2D g_sDiffuseSpecularMap;
uniform vec3 g_vAmbient;

varying vec4 g_vOutShadowCoord;
varying vec4 g_vOutScreenCoord;

void main()
{
   float fLightDepth = ( texture2DProj( g_sShadowMap, g_vOutShadowCoord ) ).r;

   vec4 vShadowColor;
   
   float fBias = 0.03;
   
   if( (g_vOutShadowCoord.z / g_vOutShadowCoord.w) - fBias > fLightDepth )
   {
       vShadowColor = vec4( vec3( 0.0 ), 1.0 );
   }
   else
   {
       vShadowColor = vec4( vec3( 1.0 ), 1.0 );
   }
   
   vec4 vDiffuseAndSpecular = texture2DProj( g_sDiffuseSpecularMap, g_vOutScreenCoord );

   gl_FragColor = vec4( g_vAmbient, 0.0 ) + vShadowColor * vDiffuseAndSpecular;
}
