//--------------------------------------------------------------------------------------
// File: Blend.glsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
//
// Simple 2D Texture Blending
//
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



uniform sampler2D g_sImageTexture1;
uniform sampler2D g_sImageTexture2;

in vec2 g_vTexCoords0;
in vec2 g_vTexCoords1;
in vec2 g_vTexCoords2;
in vec2 g_vTexCoords3;

in vec2 g_vTexCoords;

#define FRAG_COL0		0
layout(location = FRAG_COL0) out vec4 Color;   

void main()
{
    vec4 vColor1 = texture(g_sImageTexture1, g_vTexCoords); 
    
    vec4 s0 = texture(g_sImageTexture2, g_vTexCoords0);
    vec4 s1 = texture(g_sImageTexture2, g_vTexCoords1);
    vec4 s2 = texture(g_sImageTexture2, g_vTexCoords2);
    vec4 s3 = texture(g_sImageTexture2, g_vTexCoords3);

    vec4 vSobelX = 4.0 * ((s0 + s2) - (s1 + s3));
    vec4 vSobelY = 4.0 * ((s0 + s1) - (s2 + s3));

    // Compute the vector length
    vec4 vSobelSqr = vSobelX * vSobelX + vSobelY * vSobelY;
    float fSobel   = length(vSobelSqr);
    if (fSobel > 0.2)
    {
        fSobel = 1.0;
    }
    
    Color = vColor1 * clamp(vec4(1.0) - vec4(fSobel, fSobel, fSobel, 1.0 - fSobel), vec4(0.0), vec4(1.0));
}

