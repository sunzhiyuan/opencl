

//--------------------------------------------------------------------------------------
// File: Skinning2PS.hlsl
// Desc: Pixel shader for Skinning2
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#define MAX_BONES 80

cbuffer SkinningConstantBuffer
{
    float4   g_matWorldArray[MAX_BONES * 3];
    float4x4 g_matView;
    float4x4 g_matProj;
    float4   g_lightPos;
    float4   g_cameraPos;
    float4   g_vAmbient;
    float4   g_vDiffuse;
    float4   g_vSpecular;
    float    g_fShininess;
};


Texture2D tDiffuse : register(t0);
SamplerState sDiffuse : register(s0);

Texture2D tNormal : register(t1);
SamplerState sNormal : register(s1);

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 Tex0 : TEXCOORD0;
    float3 Light : TEXCOORD1;
    float3 EyeNormal : TEXCOORD2;
};


//--------------------------------------------------------------------------------------
// Fragment shader entry point
//--------------------------------------------------------------------------------------
half4 main(PixelShaderInput input) : SV_TARGET
{
	// Get texture color and normal map
	half4 vDiffuse = tDiffuse.Sample( sDiffuse, input.Tex0 );
	half3 vNormalMap = tNormal.Sample( sNormal, input.Tex0 ).rgb * 2.0 - 1.0;
	
	// Apply ambient color
    half4 outColor;
    outColor.rgb = g_vAmbient.rgb;
    outColor.a = 0.0;
    
    // Get light and eye normal
    half3 vLight = normalize( input.Light );
    half3 vEyeNormal = normalize( input.EyeNormal );
	
	// Apply diffuse color
    half fDiffuse = max( 0.0, dot( vNormalMap, vLight ) );
    outColor.rgba += vDiffuse * g_vDiffuse * fDiffuse;
    
    // Apply specular color
    half3 vHalfVector = normalize( vLight + vEyeNormal );
    half fSpecular = pow( max( 0.0, dot( vNormalMap, vHalfVector ) ), g_fShininess );
    outColor.rgba += g_vSpecular * fSpecular;

    return outColor;
}

