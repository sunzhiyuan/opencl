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
[PointSpritesVS]
#include "CommonVS.glsl"

// Uniform shader constants 
uniform   mat4  g_matModelViewProj;
uniform   float g_fSize;

// Input per-vertex attributes
attribute vec4  g_vPosition;
attribute vec2  g_vUV;
attribute float g_PointSize;

varying vec2    g_vTexCoord;
varying vec2    g_vScreenSpace;

void main()
{
    gl_Position = g_matModelViewProj * g_vPosition;
    // gl_Position = g_vPosition;
    
    gl_PointSize = g_PointSize;
    
    // Map [-1, 1] to [0, 1]
    //g_vTexCoord = (g_vPosition.xy + vec2(1.0, 1.0)) / 2.0;
    //g_vScreenSpace = vec2(g_fSize / 800.0, g_fSize / 480.0 );
    
    g_vTexCoord = g_vUV;
    
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[PointSpritesFS]
#include "CommonFS.glsl"

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

