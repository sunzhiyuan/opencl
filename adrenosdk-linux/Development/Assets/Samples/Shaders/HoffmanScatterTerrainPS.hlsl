
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
// The fragment shader for terrain (uses SAME vert shader as the sky) 
//--------------------------------------------------------------------------------------

cbuffer HoffmanScatterConstantBuffer
{
    float4x4 matModelView;
    float4x4 matModelViewProj;

    float4 EyePos;
    float4 SunDir;
    float4 BetaRPlusBetaM;
    float4 HGg;
    float4 BetaDashR;
    float4 BetaDashM;
    float4 OneOverBetaRPlusBetaM;
    float4 Multipliers;
    float4 SunColorAndIntensity;    
};

struct PixelShaderInput
{
    float4 vVertexPos : SV_POSITION;
    float3 vVSTexCoord : TEXCOORD0;
    float3 LightVec : TEXCOORD1;
    float3 Lin : TEXCOORD2;
    float3 Fex : TEXCOORD3;
};


Texture2D tBaseTexture : register(t0);
SamplerState sBaseTexture: register(s0);

Texture2D tNormTexture : register(t1);
SamplerState sNormTexture: register(s1);

half4 main( PixelShaderInput input ) : SV_TARGET
{
	half3 normal = 2.0 * (tNormTexture.Sample( sNormTexture, input.vVSTexCoord.xy ).xyz - 0.5);
	normal = half4(mul(matModelView, half4( normal, 0.0 ) ) ).xyz;
	
    half nDotL = saturate( dot( normal, input.LightVec ) );
	half3 light  = half3( nDotL, nDotL, nDotL );
	half3 albedo = tBaseTexture.Sample( sBaseTexture, input.vVSTexCoord.xy ).xyz; 
 
    return half4(input.Lin + (input.Fex * light * albedo), 1.);
} 
