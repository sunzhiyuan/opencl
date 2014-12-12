//--------------------------------------------------------------------------------------
// File: Texture.glsl
// Desc: Texture shader for the ImageEffects sample
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//
// Simple 2D Texture
//
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

varying vec2 g_vTexCoords;

void main()
{
    gl_FragColor = texture2D( g_sImageTexture, g_vTexCoords );
}

//--------------------------------------------------------------------------------------
// A fragment shader that takes an alpha parameter
//--------------------------------------------------------------------------------------
//[TextureAlphaFS]
//#include "CommonFS.glsl"
//
//uniform sampler2D g_sImageTexture;
//uniform float     g_fAlpha;
//
//varying vec2 g_vTexCoords;
//
//void main()
//{
//    vec4 vColor = texture2D( g_sImageTexture, g_vTexCoords );
//    vColor.a = g_fAlpha;
//    gl_FragColor = vColor;
//}
