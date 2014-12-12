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
[ComputeNormalsVS]

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

[ComputeNormalsFS]

#version 300 es


#include "CommonFS.glsl"

#define FRAG_COL0	0

uniform sampler2D heightMap;

in vec4 texcoord0;
in vec4 texcoord1;
in vec4 texcoord2;

layout(location = FRAG_COL0) out vec4 NormalOut;

#define BS 255.0
float decodeHeight( vec3 h )
{
    return dot( h, vec3( 1.0f, 1.0f/BS, 1.0f/BS/BS ) );
}

void main()
{
    float height;
    height = decodeHeight( texture( heightMap, texcoord0.xy ).xyz );
    
    // compute and store normal
    
    // edge length of height map grid
    float edgeLength = texcoord1.w;
    vec3 a,b;
    a.x = edgeLength;
    a.y = decodeHeight( texture( heightMap, texcoord1.xy ).xyz ) - height;
    a.z = 0.0;
    b.x = 0.0;
    b.y = decodeHeight( texture( heightMap, texcoord2.xy ).xyz ) - height;
    b.z = edgeLength;
    
    vec3 nrml = cross( b, a );
    nrml = normalize( nrml );
    nrml = vec3( -nrml.z, nrml.y, nrml.x );
    nrml = nrml * 0.5 + 0.5;
    
    // slope
    
    // height scale
    float heightScale = texcoord1.z;
    float maxSlope = max( abs( a.y ), abs( b.y ) ) * heightScale * 0.25;
    
    NormalOut = vec4( nrml.xyz, maxSlope );

}


