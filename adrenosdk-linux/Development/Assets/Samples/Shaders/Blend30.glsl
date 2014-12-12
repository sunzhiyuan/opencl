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
[Blend30VS]
#version 300 es
#include "CommonVS.glsl"



uniform float g_fHalfStepSizeX;
uniform float g_fHalfStepSizeY;


// attribute vec4 g_vVertex;
#define ATTR_POSITION	0

layout(location = ATTR_POSITION) in vec4 g_vVertex;

out vec2 g_vTexCoords;

out vec2 g_vTexCoords0;
out vec2 g_vTexCoords1;
out vec2 g_vTexCoords2;
out vec2 g_vTexCoords3;

void main()
{
    gl_Position  = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    g_vTexCoords = vec2( g_vVertex.z, g_vVertex.w );
    
    g_vTexCoords0 = g_vTexCoords + vec2( -g_fHalfStepSizeX, -g_fHalfStepSizeY );
    g_vTexCoords1 = g_vTexCoords + vec2(  g_fHalfStepSizeX, -g_fHalfStepSizeY );
    g_vTexCoords2 = g_vTexCoords + vec2( -g_fHalfStepSizeX,  g_fHalfStepSizeY );
    g_vTexCoords3 = g_vTexCoords + vec2(  g_fHalfStepSizeX,  g_fHalfStepSizeY );
}



//--------------------------------------------------------------------------------------
// The basic fragment shader
//--------------------------------------------------------------------------------------
[BlendMultiplicative30FS]
#version 300 es
#include "CommonFS.glsl"

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

