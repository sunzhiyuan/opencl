//--------------------------------------------------------------------------------------
// File: PointSprites.glsl
// Desc: Shaders for the PointSprites sample
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

// OpenGL ES requires that precision is defined for a fragment shader
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



uniform sampler2D g_TextureSampler;
uniform sampler2D g_NoiseTexture;

uniform float g_fNoiseValue;

varying vec2    g_vTexCoord;
varying vec2    g_vScreenSpace;

void main()
{
    vec4 TexColor = texture2D( g_TextureSampler, g_vTexCoord );
    vec4 NoiseColor = texture2D( g_NoiseTexture, g_vTexCoord );

    //if(NoiseColor.x > g_fNoiseValue)
    //    discard;
        
        
    gl_FragColor = TexColor;
    
    // JCOOK 
    // gl_FragColor = vec4(gl_PointCoord.y, gl_PointCoord.y, gl_PointCoord.y, 1.0);
    // gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
}

