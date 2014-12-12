//--------------------------------------------------------------------------------------
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

// AddLayer, ComputeLayerLighting
//--------------------------------------------------------------------------------------
// AddLayer
//--------------------------------------------------------------------------------------

[AddLayerVS]
//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------

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

uniform vec4 noisePanning;

out vec4 texcoord0;
out vec4 texcoord1;
out vec4 texcoord2;
out vec4 texcoord3;

void main()
{
    // place tile in render target
    gl_Position = vec4( g_PositionIn, 1.0 );
    gl_Position.xy *= g_CoordTileIn.zw;
    gl_Position.xy += g_CoordTileIn.xy;
    gl_Position = gl_Position * 2.0 - 1.0;
    gl_Position.zw = vec2( 0.0, 1.0 );
    
    texcoord0 = vec4( g_PositionIn, 1.0 );
    texcoord0.xy *= g_CoordTileIn.zw;
    texcoord0.xy += g_CoordTileIn.xy;
    texcoord1.zw = vec2( 0.0, 1.0 );

    // compute noise texture coordinates from "world space"-texcoord (=original heightmap coord)
    texcoord1 = vec4( g_PositionIn, 1.0 );
    
    texcoord1.xy -= 0.5;
    texcoord1.xy *= g_PositionIn.z / (g_PositionIn.z - 2.0 * TILE_BORDER );
    texcoord1.xy += 0.5;
    
    texcoord1.xy *= g_CoordHMIn.zw;
    texcoord1.xy += g_CoordHMIn.xy;
    
    // not flipping, sampling loaded textures, so access is normal
    //texcoord1.y = 1.0 - texcoord1.y;
    
    texcoord1.zw = vec2( 0.0, 1.0 );
    
    // keep these texture coordinates (lighting computtion uses this vertex shader, too)
    texcoord2 = texcoord1;
    
    // more texture cordinates for multiple noise octaves
    texcoord3 = texcoord1 * 8.0 + noisePanning;
    texcoord1 = texcoord1 * 1.0 + noisePanning;
}

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

[AddLayerFS]
#version 300 es


#include "CommonFS.glsl"

#define FRAG_COL0	0
#define FRAG_COL1	1

uniform sampler2D normalMap;
uniform sampler2D noiseMap;
uniform sampler2D colorMap;
uniform sampler2D heightCoverageTexture;

uniform vec4 constraintAttrib;
uniform vec4 constraintSlope;
uniform vec4 constraintAltitude;
uniform vec4 materialColor;

in vec4 texcoord0;
in vec4 texcoord1;
in vec4 texcoord2;
in vec4 texcoord3;

layout(location = FRAG_COL0) out vec4 Color;
layout(location = FRAG_COL1) out vec4 HeightAndCoverage;

#define BS 255.0
float decodeHeight( vec3 h )
{
    return dot( h, vec3( 1.0f, 1.0f/BS, 1.0f/BS/BS ) );
}

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
    float   coverage    = texture( heightCoverageTexture, texcoord0.xy ).w;
    vec3	parentColor = texture( colorMap, texcoord0.xy ).xyz;
    
    float	altitude = decodeHeight( texture( heightCoverageTexture, texcoord0.xy ).xyz );
    float	slope    = texture( normalMap, texcoord0.xy ).w * 4.0f; 
    
    // get nice per pixel noise
    float	noiseValue;
    noiseValue  = texture( noiseMap, texcoord1.xy ).x;
    noiseValue *= texture( noiseMap, texcoord3.xy ).x;
    noiseValue -= 0.1f;
    noiseValue *= 2.0f;
    
    // per pixel noise, according to surface parameters
    float	perPixelNoise = ( noiseValue ) * 2.0f * constraintAttrib.y + ( constraintAttrib.x - 0.5f ) * 4.0f;

    // evaluate constraints	
    float	hatFuncA, hatFuncS;
    
        // altitude right = height * m(right) + n(right)
        float	alt_right = altitude * constraintAltitude.x + constraintAltitude.y;
        // altitude left  = height * m(left) + n(left)
        float	alt_left  = altitude * constraintAltitude.z + constraintAltitude.w;
        hatFuncA = saturate( min( alt_left, alt_right ) );

        // slope right = slope * m(right) + n(right)
        float	slo_right = slope * constraintSlope.x + constraintSlope.y;
        // slope left  = slope * m(left) + n(left)
        float	slo_left  = slope * constraintSlope.z + constraintSlope.w;
        hatFuncS = saturate( min( slo_left, slo_right ) );
        
    // resulting coverage of new texture layer
    float coverageAlone = saturate( hatFuncS * hatFuncA * perPixelNoise );
    
    // new joined coverage
    coverage = saturate( coverage * coverageAlone );

    // new surface color
    //vec4 surfaceColor = vec4( lerp( parentColor, materialColor.xyz, coverage ), 0.0f );
    //vec4 surfaceColor = vec4( lerp( materialColor.xyz, parentColor, coverage ), 1.0f );
    vec4 surfaceColor = vec4( materialColor.xyz*coverage+parentColor*(1.0-coverage), 0.0f );
    
    // output #0: surfaceColor
    //Color = vec4( texture( heightCoverageTexture, texcoord0.xy ).xyz, 1); //surfaceColor;
    Color = surfaceColor;
    
    // displacement
    altitude += 0.003125f * coverage * texture( noiseMap, texcoord1.xy ).x * constraintAttrib.z;

    // output #1: new height and coverage
    HeightAndCoverage = vec4( encodeHeight( altitude ), coverage );

}


