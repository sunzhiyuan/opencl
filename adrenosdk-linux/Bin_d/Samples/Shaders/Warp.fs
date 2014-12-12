

//--------------------------------------------------------------------------------------
// File: Warp.glsl
// Desc: Warp shader for the ImageEffects sample
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



uniform sampler2D g_sImageTexture;
uniform sampler3D g_sNoiseTexture;
uniform float     g_fWarpScale;

varying vec2      g_vTexCoords;
varying vec2      g_vOffsetTexCoords;
varying vec3      g_vNoiseTexCoords;


void main()
{
    vec2  vTexCoords = g_vTexCoords;
    float fStrength  = 0.25 - dot( g_vOffsetTexCoords, g_vOffsetTexCoords );

    if( fStrength > 0.0 )
    {
        vTexCoords += g_fWarpScale * fStrength * (2.0 * texture3D( g_sNoiseTexture, g_vNoiseTexCoords ).xw - 1.0);
    }

    gl_FragColor = texture2D( g_sImageTexture, vTexCoords );
}
