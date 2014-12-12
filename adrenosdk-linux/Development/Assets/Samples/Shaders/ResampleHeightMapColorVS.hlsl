//--------------------------------------------------------------------------------------
// File: ResampleHeightMapColorVS.hlsl
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
// ResampleHeightMapColor Shader
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
		
	// adjust texture coordinates from source data
	vso.texcoord0 = float4( input.vPosition, 1.0 );
	vso.texcoord0.xy *= input.vCoordTile.zw;
	vso.texcoord0.xy += input.vCoordTile.xy;
	vso.texcoord0.y = 1.0 - vso.texcoord0.y;

    return vso;
}

