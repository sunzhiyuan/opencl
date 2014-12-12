

//--------------------------------------------------------------------------------------
// File: Gaussian.glsl
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
[GaussianVS]
#include "../CommonVS.glsl"

uniform vec2 g_vStepSize;

attribute vec4 g_vVertex;

varying vec2 g_vTexCoordM;
varying vec2 g_vTexCoordB0;
varying vec2 g_vTexCoordF0;
varying vec2 g_vTexCoordB1;
varying vec2 g_vTexCoordF1;
varying vec2 g_vTexCoordB2;
varying vec2 g_vTexCoordF2;

void main()
{
    gl_Position  = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );

    g_vTexCoordM  = vec2( g_vVertex.z, g_vVertex.w );
    g_vTexCoordB0 = g_vTexCoordM - g_vStepSize;
    g_vTexCoordF0 = g_vTexCoordM + g_vStepSize;
    g_vTexCoordB1 = g_vTexCoordM - g_vStepSize * 2.0;
    g_vTexCoordF1 = g_vTexCoordM + g_vStepSize * 2.0;
    g_vTexCoordB2 = g_vTexCoordM - g_vStepSize * 3.0;
    g_vTexCoordF2 = g_vTexCoordM + g_vStepSize * 3.0;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[GaussianFS]
#include "../CommonFS.glsl"

uniform sampler2D g_sImageTexture;

varying vec2 g_vTexCoordM;
varying vec2 g_vTexCoordB0;
varying vec2 g_vTexCoordF0;
varying vec2 g_vTexCoordB1;
varying vec2 g_vTexCoordF1;
varying vec2 g_vTexCoordB2;
varying vec2 g_vTexCoordF2;

void main()
{
    vec4 vSampleM  = texture2D( g_sImageTexture, g_vTexCoordM );
    vec4 vSampleB0 = texture2D( g_sImageTexture, g_vTexCoordB0 );
    vec4 vSampleF0 = texture2D( g_sImageTexture, g_vTexCoordF0 );
    vec4 vSampleB1 = texture2D( g_sImageTexture, g_vTexCoordB1 );
    vec4 vSampleF1 = texture2D( g_sImageTexture, g_vTexCoordF1 );
    vec4 vSampleB2 = texture2D( g_sImageTexture, g_vTexCoordB2 );
    vec4 vSampleF2 = texture2D( g_sImageTexture, g_vTexCoordF2 );

    gl_FragColor = 0.1752 * vSampleM + 
                   0.1658 * ( vSampleB0 + vSampleF0 ) + 
                   0.1403 * ( vSampleB1 + vSampleF1 ) + 
                   0.1063 * ( vSampleB2 + vSampleF2 );
}
