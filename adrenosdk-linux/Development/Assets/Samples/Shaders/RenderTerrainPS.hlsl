//--------------------------------------------------------------------------------------
// File: RenderTerrainPS.hlsl
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//
// RenderTerrainVS, RenderTerrainPS
// renders the terrain using the texture atlas. atmospheric effects are added.
//

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

Texture2D tTerrainTexture : register(t0);
SamplerState sTerrainTexture : register(s0);

cbuffer RenderTerrainConstantBuffer
{
    float4x4 MatModelViewProj;
    float4 rtSize;
    float4 texCoordBiasScale;
    float4 cameraPosition;
    float gammaControl;
    float exposureControl;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float2 texcoord0 : TEXCOORD0;
    float4 texcoord1 : TEXCOORD1;
    float4 texcoord2 : TEXCOORD2;
};

half4 main(PixelShaderInput input) : SV_TARGET
{
	half4 surfaceColor = tTerrainTexture.Sample( sTerrainTexture, input.texcoord0.xy );
	
	// decode higher dynamic range by using alpha
	surfaceColor.xyz = surfaceColor.xyz * surfaceColor.w * 4.0;
	
	// apply atmosphereic effects
	surfaceColor = surfaceColor * input.texcoord2 + input.texcoord1;
	
	// opt 1: exposure
	//surfaceColor = 1.0 - exp( -3.0 * surfaceColor );
	
	// opt 2: exposure + tone mapping
	surfaceColor = 1.0 - exp( exposureControl * surfaceColor );
	surfaceColor.xyz = pow( surfaceColor.xyz, half3(gammaControl, gammaControl, gammaControl) );

    return surfaceColor;
}

