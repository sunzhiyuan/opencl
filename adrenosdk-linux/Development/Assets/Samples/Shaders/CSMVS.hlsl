//--------------------------------------------------------------------------------------
// File: CSMVS.hlsl
// Desc: Vertex shader for CascadedShadowMaps
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

/******************************************************************************\
*
* Vertex shader
*
\******************************************************************************/
cbuffer CSMConstantBuffer
{
    float4x4 matModelViewProj;
    float4x4 matWorld;
    float4x4 modelviewprojections[4];
    float4x4 matNormal;
    float4   offsets[4];
    float4   shadowSpheres[4];
    float4   light;    
    float    fBias;
};


struct VertexShaderInput
{
    float3 vPositionOS : POSITION;
    float3 vNormalOS : NORMAL;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float4 vTexCoord[6] : TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input) 
{
    PixelShaderInput vso;
	float4 vertex = float4(input.vPositionOS, 1.0);
	
	vertex = mul(matWorld, vertex);
	
	float3 normal = mul((float3x3)matNormal, input.vNormalOS);
	float4 color = float4(0.75,0.75,1.0,1.0);
	
	float3 lightNormalized = normalize( light.xyz );
	
	
	vso.vPosition = mul(matModelViewProj, float4(input.vPositionOS, 1.0));
	
    vso.vTexCoord[0] = color * (dot(normal,lightNormalized) * 0.5 + 0.5);
	vso.vTexCoord[1] = mul(modelviewprojections[0], mul(matWorld, float4(input.vPositionOS, 1.0)));
    vso.vTexCoord[2] = mul(modelviewprojections[1], mul(matWorld, float4(input.vPositionOS, 1.0)));
    vso.vTexCoord[3] = mul(modelviewprojections[2], mul(matWorld, float4(input.vPositionOS, 1.0)));
    vso.vTexCoord[4] = mul(modelviewprojections[3], mul(matWorld, float4(input.vPositionOS, 1.0)));
	
    vso.vTexCoord[5].x = dot((offsets[0].xyz - vertex.xyz), lightNormalized) * 0.00502765209;//this should be scalled 1/(shadow_range)
	vso.vTexCoord[5].y = dot((offsets[1].xyz - vertex.xyz), lightNormalized) * 0.00502765209;
	vso.vTexCoord[5].z = dot((offsets[2].xyz - vertex.xyz), lightNormalized) * 0.00502765209;
	vso.vTexCoord[5].w = dot((offsets[3].xyz - vertex.xyz), lightNormalized) * 0.00502765209;

    return vso;
}
