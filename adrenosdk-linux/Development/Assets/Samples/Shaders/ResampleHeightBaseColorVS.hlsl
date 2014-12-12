//--------------------------------------------------------------------------------------
// File: ResampleHeightBaseColorVS.hlsl
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

// size of border for textures tiles (>=2)
// this size has to be increased, if mip-mapping for the tiles is applied!!!
#define	TILE_BORDER 4.0

cbuffer ResampleHeightConstantBuffer
{
    float4 terrainBaseColor;
    float4 constraintAttrib;
    float heightMapSize;
};

struct VertexShaderInput
{
    float3 vPosition : POSITION;
    float4 vCoordHM : TEXCOORD0;
    float4 vCoordTile : TEXCOORD1;
    float2 vScale : TEXCOORD2;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float4 texcoord0 : TEXCOORD0;
    float4 texcoord1 : TEXCOORD1;
    float4 texcoord2 : TEXCOORD2;
    float4 texcoord3 : TEXCOORD3;
};


PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

	// place tile in render target
	vso.vPosition = float4( input.vPosition, 1.0 );
	vso.vPosition.xy *= input.vCoordTile.zw;
	vso.vPosition.xy += input.vCoordTile.xy;
	vso.vPosition = vso.vPosition * 2.0 - 1.0;
	vso.vPosition.zw = float2( 1.0, 1.0 );
	
	// adjust texture coordinates from source heightfield
	vso.texcoord0 = float4( input.vPosition, 1.0 );
	
	vso.texcoord0.xy -= 0.5;
	vso.texcoord0.xy *= input.vPosition.z / (input.vPosition.z - 2.0 * TILE_BORDER );
	vso.texcoord0.xy += 0.5;
	
	vso.texcoord0.xy *= input.vCoordHM.zw;
	vso.texcoord0.xy += input.vCoordHM.xy;
	
	// flip y, generated the heightmap with reversed y
    vso.texcoord0.y = 1.0 - vso.texcoord0.y;
	
	// a translation, that bi-cubic interpolation is correctly located
	const float C = -1.5;
	
	// lookups for the 4x4 grid of heightvalues	
	vso.texcoord1.xy = vso.texcoord0.xy + float2(-1+C,+0+C) / heightMapSize;
	vso.texcoord2.xy = vso.texcoord0.xy + float2(-1+C,+1+C) / heightMapSize;
	vso.texcoord3.xy = vso.texcoord0.xy + float2(-1+C,+2+C) / heightMapSize;
	vso.texcoord0.zw = vso.texcoord0.xy + float2(+1+C,-1+C) / heightMapSize;
	vso.texcoord1.zw = vso.texcoord0.xy + float2(+1+C,+0+C) / heightMapSize;
	vso.texcoord2.zw = vso.texcoord0.xy + float2(+1+C,+1+C) / heightMapSize;
	vso.texcoord3.zw = vso.texcoord0.xy + float2(+1+C,+2+C) / heightMapSize;

	vso.texcoord0.xy = vso.texcoord0.xy + float2(-1+C,-1+C) / heightMapSize;

    return vso;
}

