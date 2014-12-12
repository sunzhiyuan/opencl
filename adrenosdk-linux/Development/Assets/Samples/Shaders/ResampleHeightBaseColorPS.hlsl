//--------------------------------------------------------------------------------------
// File: ResampleHeightBaseColorPS.hlsl
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
// The fragment shader
//--------------------------------------------------------------------------------------
#define FRAG_COL0	0
#define FRAG_COL1	1

cbuffer ResampleHeightConstantBuffer
{
    float4 terrainBaseColor;
    float4 constraintAttrib;
    float heightMapSize;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float4 texcoord0 : TEXCOORD0;
    float4 texcoord1 : TEXCOORD1;
    float4 texcoord2 : TEXCOORD2;
    float4 texcoord3 : TEXCOORD3;
};

struct PixelShaderOutput
{
    half4 Color : SV_TARGET0;
    half4 HeightAndCoverage : SV_TARGET1;
};

Texture2D tHeightMap : register(t0);
SamplerState sHeightMap : register(s0);

Texture2D tNoiseMap : register(t1);
SamplerState sNoiseMap : register(s1);

Texture2D tBicubicWeight03 : register(t2);
SamplerState sBicubicWeight03 : register(s2);

Texture2D tBicubicWeight47 : register(t3);
SamplerState sBicubicWeight47 : register(s3);

Texture2D tBicubicWeight8B : register(t4);
SamplerState sBicubicWeight8B : register(s4);

Texture2D tBicubicWeightCF : register(t5);
SamplerState sBicubicWeightCF : register(s5);

// height encoding and decoding (float->3x 8bit and vice versa)
#define BS 255.0f
half3 encodeHeight( half h )
{
	half3 r;

	h *= BS;

	r.x = floor( h );
	h  -= r.x;
	r.y = floor( h * BS );
	h  -= r.y / BS;
	r.z = floor( h * BS * BS );

	return r / BS;
}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput pso;

	// Output #0: terrain base color
	pso.Color = terrainBaseColor;
	
	// Output #1: height and initial coverage
	
	// RG16UI heightmap with 2 stored heightvalues per pixel
	half4 h04 = half4( tHeightMap.Sample( sHeightMap, input.texcoord0.xy ).xy, tHeightMap.Sample( sHeightMap, input.texcoord0.zw ).xy );
	half4 h15 = half4( tHeightMap.Sample( sHeightMap, input.texcoord1.xy ).xy, tHeightMap.Sample( sHeightMap, input.texcoord1.zw ).xy );
	half4 h26 = half4( tHeightMap.Sample( sHeightMap, input.texcoord2.xy ).xy, tHeightMap.Sample( sHeightMap, input.texcoord2.zw ).xy );
	half4 h37 = half4( tHeightMap.Sample( sHeightMap, input.texcoord3.xy ).xy, tHeightMap.Sample( sHeightMap, input.texcoord3.zw ).xy );
	
	// fraction of height map texture coordinate
	// seems like there's a bug in some ATI 9700 mobility drivers:
	// using the frac(.) method results in weird texture lookups causing false interpolation in one corner
	// of a heixel. Choosing the second method and WRAP-texture lookups works fine...
	//	half2 tc = frac( fragment.texcoord0.xy * heightMapSize ); // directx
	//	vec2 tc = texcoord0.xy * heightMapSize - 1.0;
	half2 tc = frac( input.texcoord0.xy * heightMapSize ); // - 1.0;
	
	// get bicubic interpolation weights
	half h;	
	
	half4 W0 = tBicubicWeight03.Sample( sBicubicWeight03, tc );
	half4 W1 = tBicubicWeight47.Sample( sBicubicWeight47, tc );
	half4 W2 = tBicubicWeight8B.Sample( sBicubicWeight8B, tc );
	half4 W3 = tBicubicWeightCF.Sample( sBicubicWeightCF, tc );
	
	// weighted sum to obtain height value
	h  = dot( h04, W0 );
	h += dot( h15, W1 );
	h += dot( h26, W2 );
	h += dot( h37, W3 );

	h += -constraintAttrib.z*(
			(tNoiseMap.Sample( sNoiseMap, input.texcoord0.xy*8.0).x-0.5) * 4.0+
			(tNoiseMap.Sample( sNoiseMap, input.texcoord0.xy*32.0).x-0.5) * 1.0
			) * 0.025;
			
	pso.HeightAndCoverage.xyz = encodeHeight( h );
	
	// initial coverage
	pso.HeightAndCoverage.w = 1.0;

    return pso;
}

