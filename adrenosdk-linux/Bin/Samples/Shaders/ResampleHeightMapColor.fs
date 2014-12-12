//--------------------------------------------------------------------------------------
// File: Step0_Resample_Height.glsl
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// ResampleHeightMapColor Shader
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



#define FRAG_COL0	0
#define FRAG_COL1	1

uniform sampler2D colorMap;
uniform sampler2D heightTexture;
uniform sampler2D coverageTexture;

in vec4 texcoord0;

layout(location = FRAG_COL0) out vec4 Color;
layout(location = FRAG_COL1) out vec4 HeightAndCoverage;

void main()
{
    Color = texture( colorMap, texcoord0.xy );
    
    HeightAndCoverage.xyz = texture( heightTexture, texcoord0.xy ).xyz;
    HeightAndCoverage.w = texture( coverageTexture, texcoord0.xy ).w;

}

