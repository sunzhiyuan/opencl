
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
// The fragment shader
//--------------------------------------------------------------------------------------
cbuffer AddLayerConstantBuffer
{
    float4 noisePanning;
    float4 constraintAttrib;
    float4 constraintSlope;
    float4 constraintAltitude;
    float4 materialColor;
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


Texture2D tNormalMap : register(t0);
SamplerState sNormalMap : register(s0);

Texture2D tNoiseMap : register(t1);
SamplerState sNoiseMap : register(s1);

Texture2D tColorMap : register(t2);
SamplerState sColorMap : register(s2);

Texture2D tHeightCoverageTexture : register(t3);
SamplerState sHeightCoverageTexture : register(s3);

#define BS 255.0f
half decodeHeight( half3 h )
{
	return dot( h, half3( 1.0f, 1.0f/BS, 1.0f/BS/BS ) );
}

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

    half   coverage    = tHeightCoverageTexture.Sample( sHeightCoverageTexture, input.texcoord0.xy ).w;
    half3  parentColor = tColorMap.Sample( sColorMap, input.texcoord0.xy ).xyz;
	
    half	altitude = decodeHeight( tHeightCoverageTexture.Sample( sHeightCoverageTexture, input.texcoord0.xy ).xyz );
    half	slope    = tNormalMap.Sample( sNormalMap, input.texcoord0.xy ).w * 4.0f; 
	
    // get nice per pixel noise
    half	noiseValue;
    noiseValue  = tNoiseMap.Sample( sNoiseMap, input.texcoord1.xy ).x;
    noiseValue *= tNoiseMap.Sample( sNoiseMap, input.texcoord3.xy ).x;
    noiseValue -= 0.1f;
    noiseValue *= 2.0f;
	
    // per pixel noise, according to surface parameters
    half	perPixelNoise = ( noiseValue ) * 2.0f * constraintAttrib.y + ( constraintAttrib.x - 0.5f ) * 4.0f;

    // evaluate constraints	
    half	hatFuncA, hatFuncS;
	
	// altitude right = height * m(right) + n(right)
	half	alt_right = altitude * constraintAltitude.x + constraintAltitude.y;
	// altitude left  = height * m(left) + n(left)
	half	alt_left  = altitude * constraintAltitude.z + constraintAltitude.w;
	hatFuncA = saturate( min( alt_left, alt_right ) );

	// slope right = slope * m(right) + n(right)
	half	slo_right = slope * constraintSlope.x + constraintSlope.y;
	// slope left  = slope * m(left) + n(left)
	half	slo_left  = slope * constraintSlope.z + constraintSlope.w;
	hatFuncS = saturate( min( slo_left, slo_right ) );
		
    // resulting coverage of new texture layer
    half coverageAlone = saturate( hatFuncS * hatFuncA * perPixelNoise );
	
    // new joined coverage
    coverage = saturate( coverage * coverageAlone );

    // new surface color
    //vec4 surfaceColor = vec4( lerp( parentColor, materialColor.xyz, coverage ), 0.0f );
    //vec4 surfaceColor = vec4( lerp( materialColor.xyz, parentColor, coverage ), 1.0f );
    half4 surfaceColor = half4( materialColor.xyz*coverage+parentColor*(1.0-coverage), 0.0f );
	
    // output #0: surfaceColor
    //Color = vec4( texture( heightCoverageTexture, texcoord0.xy ).xyz, 1); //surfaceColor;
    pso.Color = surfaceColor;
	
    // displacement
    altitude += 0.003125f * coverage * tNoiseMap.Sample( sNoiseMap, input.texcoord1.xy ).x * constraintAttrib.z;

    // output #1: new height and coverage
    pso.HeightAndCoverage = half4( encodeHeight( altitude ), coverage );

    return pso;
}
