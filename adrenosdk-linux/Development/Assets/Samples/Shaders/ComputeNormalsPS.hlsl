
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//
// ComputeNormals
//

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float4 texcoord0 : TEXCOORD0;
    float4 texcoord1 : TEXCOORD1;
    float4 texcoord2 : TEXCOORD2;
};

Texture2D tHeightMap : register(t0);
SamplerState sHeightMap : register(s0);

#define BS 255.0f
half decodeHeight( half3 h )
{
	return dot( h, half3( 1.0f, 1.0f/BS, 1.0f/BS/BS ) );
}

half4 main(PixelShaderInput input) : SV_TARGET
{
	half height;
	height = decodeHeight( tHeightMap.Sample( sHeightMap, input.texcoord0.xy ).xyz );
	
	// compute and store normal
	
	// edge length of height map grid
	half edgeLength = input.texcoord1.w;
	half3 a,b;
	a.x = edgeLength;
	a.y = decodeHeight( tHeightMap.Sample( sHeightMap, input.texcoord1.xy ).xyz ) - height;
	a.z = 0.0;
	b.x = 0.0;
	b.y = decodeHeight( tHeightMap.Sample( sHeightMap, input.texcoord2.xy ).xyz ) - height;
	b.z = edgeLength;
	
	half3 nrml = cross( b, a );
	nrml = normalize( nrml );
	nrml = half3( -nrml.z, nrml.y, nrml.x );
	nrml = nrml * 0.5 + 0.5;
	
	// slope
	
	// height scale
	half heightScale = input.texcoord1.z;
	half maxSlope = max( abs( a.y ), abs( b.y ) ) * heightScale * 0.25;
	
    return half4( nrml.xyz, maxSlope );
}


