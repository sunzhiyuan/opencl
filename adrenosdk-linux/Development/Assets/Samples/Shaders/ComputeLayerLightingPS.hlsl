
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



// ComputeLayerLighting
//--------------------------------------------------------------------------------------

Texture2D tNormalMap : register(t0);
SamplerState sNormalMap : register(s0);

Texture2D tShadowTexture : register(t1);
SamplerState sShadowTexture : register(s1);

Texture2D tOcclusionTexture : register(t2);
SamplerState sOcclusionTexture : register(s2);

Texture2D tColorMap : register(t3);
SamplerState sColorMap : register(s3);

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float4 texcoord0 : TEXCOORD0;
    float4 texcoord1 : TEXCOORD1;
    float4 texcoord2 : TEXCOORD2;
    float4 texcoord3 : TEXCOORD3;
};

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


half3 computeLighting( half4 color, half4 normal, PixelShaderInput input )
{
    // Same used in RenderTerrain Shader
    half4 sunDirection = half4( -0.438, 0.509, 0.741, 0.0 );

    // Sun color
    half3 light = half3( 3.52, 3.42, 2.99 );
	
    light *= max( 0, dot( normal.xyz, sunDirection.zyx ) );

    // shadow information and ambient occlusion
    half	shadow = tShadowTexture.Sample( sShadowTexture, input.texcoord2.xy ).x;
    half	ambientOcclusion = tOcclusionTexture.Sample( sOcclusionTexture, input.texcoord2.xy ).x * 2.0 - 0.7;
	
    // combine light * shadow + ambient light
    return	color.xyz * ( light * shadow + ambientOcclusion * half3( 0.39, 0.47, 0.75 ) );
}


half4 main(PixelShaderInput input) : SV_TARGET
{
    half4 normal = tNormalMap.Sample( sNormalMap, input.texcoord0.xy ) * 2.0 - 1.0;
    half4 surfaceColor = tColorMap.Sample( sColorMap, input.texcoord0.xy );

    surfaceColor.xyz = computeLighting( surfaceColor, normal, input );
	
    // allow higher dynamic range by using alpha channel	
    half maxV = max( surfaceColor.x, max( surfaceColor.y, surfaceColor.z ) );
    maxV = max( 1.0f, maxV );
    surfaceColor.w = maxV / 4.0f;
    surfaceColor.xyz /= maxV;

    return surfaceColor;
}

