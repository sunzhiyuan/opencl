//--------------------------------------------------------------------------------------
// File: PhysicallyBasedLightingVS.hlsl
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
cbuffer PhysicallyBasedLightingConstantBuffer
{
    float4x4 matModelView;
    float4x4 matModelViewProj;
    float4x4 matNormal;
    float4   vLightPos;
    float4   SkyColor;
    float4   GroundColor;
    float4   vMaterialSpecular;
    float4   vMaterialDiffuse;
    float4   vLightColor;
    float    fMaterialGlossiness;
};


struct VertexShaderInput
{
    float3 vPositionOS : POSITION;
    float3 vNormalOS : NORMAL;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float3 NIn: TEXCOORD0;
    float3 LIn: TEXCOORD1;
    float3 VIn: TEXCOORD2;
    float3 vColor : COLOR;
};


PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

	float4 P = mul( matModelView, float4(input.vPositionOS, 1.0) );
	
	vso.NIn = mul( (float3x3)matNormal, input.vNormalOS.xyz );
	
	vso.LIn = vLightPos.xyz - P.xyz;
	
	vso.VIn = float3(0.0,0.0,0.0) - P.xyz;
	
    vso.vPosition = mul( matModelViewProj, float4(input.vPositionOS, 1.0) );
    
    vso. vColor = SkyColor.rgb * dot( vso.NIn, float3( 0., 1., 0.)) + GroundColor.rgb * dot( vso.NIn, float3( 0., -1., 0.));

    return vso;
	
}


