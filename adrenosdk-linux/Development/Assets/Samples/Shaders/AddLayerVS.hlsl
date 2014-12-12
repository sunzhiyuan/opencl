
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// AddLayer
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------

// size of border for textures tiles (>=2)
// this size has to be increased, if mip-mapping for the tiles is applied!!!
#define	TILE_BORDER 4.0

cbuffer AddLayerConstantBuffer
{
    float4 noisePanning;
    float4 constraintAttrib;
    float4 constraintSlope;
    float4 constraintAltitude;
    float4 materialColor;
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
	
	vso.texcoord0 = float4( input.vPosition, 1.0 );
	vso.texcoord0.xy *= input.vCoordTile.zw;
	vso.texcoord0.xy += input.vCoordTile.xy;
    vso.texcoord1.zw = float2( 0.0, 1.0 );
    vso.texcoord0.y = 1.0 - vso.texcoord0.y;

	// compute noise texture coordinates from "world space"-texcoord (=original heightmap coord)
	vso.texcoord1 = float4( input.vPosition, 1.0 );
	
	vso.texcoord1.xy -= 0.5;
	vso.texcoord1.xy *= input.vPosition.z / (input.vPosition.z - 2.0 * TILE_BORDER );
	vso.texcoord1.xy += 0.5;
	
	vso.texcoord1.xy *= input.vCoordHM.zw;
	vso.texcoord1.xy += input.vCoordHM.xy;
	
	// not flipping, sampling loaded textures, so access is normal
	//vso.texcoord1.y = 1.0 - vso.texcoord1.y;
	
	vso.texcoord1.zw = float2( 0.0, 1.0 );
	
	// keep these texture coordinates (lighting computtion uses this vertex shader, too)
	vso.texcoord2 = vso.texcoord1;
	
	// more texture cordinates for multiple noise octaves
	vso.texcoord3 = vso.texcoord1 * 8.0 + noisePanning;
	vso.texcoord1 = vso.texcoord1 * 1.0 + noisePanning;

    return vso;
}
