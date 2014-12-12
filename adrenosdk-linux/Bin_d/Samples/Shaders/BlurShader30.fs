//--------------------------------------------------------------------------------------
// File: BlurShader.glsl
// Desc: Shaders for per-pixel lighting
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



in vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
uniform float g_GaussWeight[4];
uniform float g_GaussInvSum;

out vec4 g_FragColor;

void main()
{
    // 4 tap gaussian distribution
    vec4 Color = g_GaussWeight[0] * texture( g_ColorTexture, vec2( g_TexCoord.x + g_StepSize.x, g_TexCoord.y + g_StepSize.y ) );
    Color += g_GaussWeight[1] * texture( g_ColorTexture, vec2( g_TexCoord.x + g_StepSize.x * 0.5, g_TexCoord.y + g_StepSize.y * 0.5 ) );
    Color += g_GaussWeight[2] * texture( g_ColorTexture, vec2( g_TexCoord.x - g_StepSize.x * 0.5, g_TexCoord.y - g_StepSize.y * 0.5 ) );
    Color += g_GaussWeight[3] * texture( g_ColorTexture, vec2( g_TexCoord.x - g_StepSize.x, g_TexCoord.y - g_StepSize.y ) );

    g_FragColor = ( Color * g_GaussInvSum );
}
