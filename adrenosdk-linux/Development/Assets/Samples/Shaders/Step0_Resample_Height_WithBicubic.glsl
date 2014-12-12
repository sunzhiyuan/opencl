//--------------------------------------------------------------------------------------
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
[ResampleHeightBaseColorBicubicVS]

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
	texcoord1 = texcoord0 + vec4(-1+C,+0+C,0,0) / heightMapSize;
	texcoord2 = texcoord0 + vec4(-1+C,+1+C,0,0) / heightMapSize;
	texcoord3 = texcoord0 + vec4(-1+C,+2+C,0,0) / heightMapSize;
	texcoord4 = texcoord0 + vec4(+1+C,-1+C,0,0) / heightMapSize;
	texcoord5 = texcoord0 + vec4(+1+C,+0+C,0,0) / heightMapSize;
	texcoord6 = texcoord0 + vec4(+1+C,+1+C,0,0) / heightMapSize;
	texcoord7 = texcoord0 + vec4(+1+C,+2+C,0,0) / heightMapSize;

	texcoord0 = texcoord0 + vec4(-1+C,-1+C,0,0) / heightMapSize;
}

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[ResampleHeightBaseColorBicubicFS]

#version 300 es

#include "CommonFS.glsl"

#define FRAG_COL0	0
#define FRAG_COL1	1

uniform vec4 terrainBaseColor;
uniform vec4 constraintAttrib;

uniform float heightMapSize;

uniform sampler2D heightMap;
uniform sampler2D noiseMap;

uniform sampler2D bicubicWeight03;
uniform sampler2D bicubicWeight47;
uniform sampler2D bicubicWeight8B;
uniform sampler2D bicubicWeightCF;

in vec4 texcoord0;
in vec4 texcoord1;
in vec4 texcoord2;
in vec4 texcoord3;
in vec4 texcoord4;
in vec4 texcoord5;
in vec4 texcoord6;
in vec4 texcoord7;

layout(location = FRAG_COL0) out vec4 Color;
layout(location = FRAG_COL1) out vec4 HeightAndCoverage;

// height encoding and decoding (float->3x 8bit and vice versa)
#define BS 255.0f
vec3 encodeHeight( float h )
{
	vec3 r;

	h *= BS;

	r.x = floor( h );
	h  -= r.x;
	r.y = floor( h * BS );
	h  -= r.y / BS;
	r.z = floor( h * BS * BS );

	return r / BS;
}

void main()
{
	// Output #0: terrain base color
	Color = terrainBaseColor;
	
	// Output #1: height and initial coverage
	
	// G16R16 heightmap with 2 stored heightvalues per pixel
	vec4 h04 = vec4( texture( heightMap, texcoord0.xy ).yx, texture( heightMap, texcoord4.xy ).yx )/65535.0;
	vec4 h15 = vec4( texture( heightMap, texcoord1.xy ).yx, texture( heightMap, texcoord5.xy ).yx )/65535.0;
	vec4 h26 = vec4( texture( heightMap, texcoord2.xy ).yx, texture( heightMap, texcoord6.xy ).yx )/65535.0;
	vec4 h37 = vec4( texture( heightMap, texcoord3.xy ).yx, texture( heightMap, texcoord7.xy ).yx )/65535.0;

	
	// fraction of height map texture coordinate
	// seems like there's a bug in some ATI 9700 mobility drivers:
	// using the frac(.) method results in weird texture lookups causing false interpolation in one corner
	// of a heixel. Choosing the second method and WRAP-texture lookups works fine...
	//	float2 tc = frac( fragment.texcoord0.xy * heightMapSize ); // directx
	//	vec2 tc = texcoord0.xy * heightMapSize - 1.0;
	vec2 tc = fract( texcoord0.xy * heightMapSize ); // - 1.0;
	
	// get bicubic interpolation weights
	float h;	
	
	vec4 W0 = texture( bicubicWeight03, tc );
	vec4 W1 = texture( bicubicWeight47, tc );
	vec4 W2 = texture( bicubicWeight8B, tc );
	vec4 W3 = texture( bicubicWeightCF, tc );
	
	// weighted sum to obtain height value
	h  = dot( h04, W0 );
	h += dot( h15, W1 );
	h += dot( h26, W2 );
	h += dot( h37, W3 );

	h += -constraintAttrib.z*(
			(texture( noiseMap, texcoord0.xy*8.0).x-0.5) * 4.0+
			(texture( noiseMap, texcoord0.xy*32.0).x-0.5) * 1.0
			) * 0.025;

			
	HeightAndCoverage.xyz = encodeHeight( h );
	
	// initial coverage
	HeightAndCoverage.w = 1.0;
}




//--------------------------------------------------------------------------------------
// ResampleHeightMapColor Shader
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[ResampleHeightMapColorBicubicVS]
#include "CommonVS.glsl"

#version 300

#define ATTR_POSITION	1
#define ATTR_TEXCOORD0	7
#define ATTR_TEXCOORD1	8
#define ATTR_TEXCOORD2	9

// size of border for textures tiles (>=2)
// this size has to be increased, if mip-mapping for the tiles is applied!!!
#define	TILE_BORDER 4.0

// Tim- shader may be setup to be called with the same Vertex buffers and pointers...
//		This can be adjusted later
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
[ResampleHeightMapColorBicubicFS]
#include "CommonFS.glsl"

#version 300

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

