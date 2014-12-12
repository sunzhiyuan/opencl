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

//
// ResampleHeightBaseColor, ResampleHeightMapColor
//

//--------------------------------------------------------------------------------------
// ResampleHeightBaseColor Shader
//--------------------------------------------------------------------------------------

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

uniform float heightMapSize;

out vec4 texcoord0;
out vec4 texcoord1;
out vec4 texcoord2;
out vec4 texcoord3;
out vec4 texcoord4;
out vec4 texcoord5;
out vec4 texcoord6;
out vec4 texcoord7;

void main()
{
    // place tile in render target
    gl_Position = vec4( g_PositionIn, 1.0 );
    gl_Position.xy *= g_CoordTileIn.zw;
    gl_Position.xy += g_CoordTileIn.xy;
    gl_Position = gl_Position * 2.0 - 1.0;
    gl_Position.zw = vec2( 1.0 );
    
    // adjust texture coordinates from source heightfield
    texcoord0 = vec4( g_PositionIn, 1.0 );
    
    texcoord0.xy -= 0.5;
    texcoord0.xy *= g_PositionIn.z / (g_PositionIn.z - 2.0 * TILE_BORDER );
    texcoord0.xy += 0.5;
    
    texcoord0.xy *= g_CoordHMIn.zw;
    texcoord0.xy += g_CoordHMIn.xy;
    
    // flip y, generated the heightmap with reversed y
    texcoord0.y = 1.0 - texcoord0.y;
    
    // a translation, that bi-cubic interpolation is correctly located
    const float C = -1.5;
    
    // lookups for the 4x4 grid of heightvalues	
    texcoord1 = texcoord0 + vec4(-1.0+C,+0.0+C,0,0) / heightMapSize;
    texcoord2 = texcoord0 + vec4(-1.0+C,+1.0+C,0,0) / heightMapSize;
    texcoord3 = texcoord0 + vec4(-1.0+C,+2.0+C,0,0) / heightMapSize;
    texcoord4 = texcoord0 + vec4(+1.0+C,-1.0+C,0,0) / heightMapSize;
    texcoord5 = texcoord0 + vec4(+1.0+C,+0.0+C,0,0) / heightMapSize;
    texcoord6 = texcoord0 + vec4(+1.0+C,+1.0+C,0,0) / heightMapSize;
    texcoord7 = texcoord0 + vec4(+1.0+C,+2.0+C,0,0) / heightMapSize;

    texcoord0 = texcoord0 + vec4(-1.0+C,-1.0+C,0,0) / heightMapSize;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

