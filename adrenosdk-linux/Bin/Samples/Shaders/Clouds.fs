

//--------------------------------------------------------------------------------------
// File: Clouds.glsl
// Desc: Clouds shader
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

uniform float g_fIntensity;
uniform vec3 g_vColor1;
uniform vec3 g_vColor2;

varying vec3 g_vOutScaledPosition;

void main()
{
    // Unisgned turbulence
    float fUSTurb = 0.5 * ( 1.0 + texture3D( g_sTurb, g_vOutScaledPosition ).x );
    fUSTurb *= g_fIntensity;

    vec3 vResultColor = mix( g_vColor1, g_vColor2, fUSTurb );

    gl_FragColor = vec4( vResultColor, 1.0 );
}


