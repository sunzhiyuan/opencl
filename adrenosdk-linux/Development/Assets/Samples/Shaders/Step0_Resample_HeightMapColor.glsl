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
[ResampleHeightMapColorVS]

#version 300 es

#include "CommonVS.glsl"

#define ATTR_POSITION	1
#define ATTR_TEXCOORD0	7
#define ATTR_TEXCOORD1	8
#define ATTR_TEXCOORD2	9

// size of border for textures tiles (>=2)
// this size has to be increased, if mip-mapping for the tiles is applied!!!
#define	TILE_BORDER 4.0

layout(location = ATTR_POSITION)	in vec3 g_PositionIn;
layout(location = ATTR_TEXCOORD0) 	in vec4 g_CoordHMIn;
layout(location = ATTR_TEXCOORD1) 	in vec4 g_CoordTileIn;
layout(location = ATTR_TEXCOORD2) 	in vec2 g_ScaleIn;

uniform float heightMapSize;

out vec4 texcoord0;

void main()
{
    // place tile in render target
    gl_Position = vec4( g_PositionIn, 1.0 );
    gl_Position.xy *= g_CoordTileIn.zw;
    gl_Position.xy += g_CoordTileIn.xy;
    gl_Position = gl_Position * 2.0 - 1.0;
    gl_Position.zw = vec2( 1.0 );
    
    // adjust texture coordinates from source data
    texcoord0 = vec4( g_PositionIn, 1.0 );
    texcoord0.xy *= g_CoordTileIn.zw;
    texcoord0.xy += g_CoordTileIn.xy;
    //texcoord0.y = 1.0 - texcoord0.y;
}

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[ResampleHeightMapColorFS]

#version 300 es

#include "CommonFS.glsl"

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

