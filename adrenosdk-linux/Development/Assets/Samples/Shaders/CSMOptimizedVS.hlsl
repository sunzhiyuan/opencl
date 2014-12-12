//--------------------------------------------------------------------------------------
// File: CSMOptimizedVS.hlsl
// Desc: Optimized vertex shader for CascadedShadowMaps
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
    float4 vColor : COLOR;
    float4 vWorldSpace : TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input) 
{
    PixelShaderInput vso;
	
	vso.vWorldSpace = mul(matWorld, float4(input.vPositionOS, 1.0));
	
    float3 normal = mul((float3x3)matNormal, input.vNormalOS);
	vso.vColor = float4(0.75,0.75,1.0,1.0);
	
	vso.vPosition = mul(matModelViewProj, float4(input.vPositionOS, 1.0));
	
	vso.vColor = vso.vColor * (dot(normal,light.xyz) * 0.5 + 0.5);	

    return vso;
}
