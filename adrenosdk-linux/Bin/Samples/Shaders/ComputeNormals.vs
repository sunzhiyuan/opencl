//--------------------------------------------------------------------------------------
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//
// ComputeNormals
//

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------

#version 300 es

//--------------------------------------------------------------------------------------
// File: CommonVS.glsl
// Desc: Useful common shader code for vertex shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



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

uniform vec4 rtSize;

out vec4 texcoord0;
out vec4 texcoord1;
out vec4 texcoord2;

void main()
{
    // place tile in render target
    gl_Position = vec4( g_PositionIn, 1.0 );
    gl_Position.xy *= g_CoordTileIn.zw;
    gl_Position.xy += g_CoordTileIn.xy;
    gl_Position = gl_Position * 2.0 - 1.0;
    gl_Position.zw = vec2( 1.0 );
    
    // 1 over texture resolution
    float delta = rtSize.y;
    
    // adjust source rectable in texture
    texcoord0 = vec4( g_PositionIn, 1.0 );
    
    texcoord0.xy *= g_CoordTileIn.zw;
    texcoord0.xy += g_CoordTileIn.xy;
    
    texcoord0.x -= 0.5 * rtSize.y;
    texcoord0.y -= 0.5 * rtSize.y;
    
    // neighbor samples
    texcoord1 = texcoord0 + vec4( delta, 0.0, 0.0, 0.0 );
    texcoord2 = texcoord0 + vec4( 0.0, delta, 0.0, 0.0 );
    
    texcoord1.zw = g_ScaleIn.xy;
}

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

