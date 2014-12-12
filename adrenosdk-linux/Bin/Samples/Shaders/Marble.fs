

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
uniform sampler3D g_sTurb;

uniform float g_Kd;
uniform float g_Ks;
uniform vec4  g_vColor1;
uniform vec4  g_vColor2;
uniform float g_fTurbScale;
uniform vec4  g_vLightDir;

varying vec3 g_vOutPosition;
varying vec3 g_vOutScaledPosition;
varying vec3 g_vOutNormalES;
varying vec3 g_vOutViewVec;

void main()
{
    // Signed turbulence
    float fSTurb = texture3D( g_sTurb, g_vOutScaledPosition ).x;

    float fLrp = sin( g_fTurbScale * fSTurb + g_vOutScaledPosition.x + g_vOutScaledPosition.y + g_vOutScaledPosition.z );
    vec4 fMarbleColor = mix( g_vColor1, g_vColor2, fLrp );

    vec3 vNormal = normalize( g_vOutNormalES );
    // Soft diffuse
    float fDiffuse = 0.5 + 0.5 * dot( g_vLightDir.xyz, vNormal );
    // Standard specular
    float fSpecular = pow( clamp( dot( reflect( -normalize( g_vOutViewVec ), vNormal ), g_vLightDir.xyz ), 0.0, 1.0 ), 12.0 );

    gl_FragColor = g_Kd * fDiffuse * fMarbleColor + g_Ks * fSpecular;
}


