
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// File: CompressedGeometry8VS.hlsl
// Desc: Simple shader demonstrating geometry compression -an OpenglES3.0 port of 
//  Dean Calver's Demo:"Using Vertex Shaders For Geometry Compression" in Shaders X2
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
cbuffer CompressedGeometryConstantBuffer
{
    float4x4 matModelView;
    float4x4 matCT; // compression transform (actually it's inverse)
    float4x4 matModelViewProj;
    float4x4 matNormal;
    float4   vAmbient;
    float4   vDiffuse;
    float4   vSpecular;
    float4   vLightPos;
};

struct VertexShaderInput
{
    float4 vPositionOS : POSITION;
    float4 vNormalOS : NORMAL;
};

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float3 vNormalES : NORMAL;
    float3 vViewVecES : TEXCOORD0;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

    float4 vPos = input.vPositionOS;
	
	vPos = mul(matCT, vPos);
    float4 vPositionES = mul(matModelView, vPos);
    float4 vPositionCS = mul(matModelViewProj, vPos);

	// Unpack normal
    float3 vNormal = input.vNormalOS.xyz * 2.0 - 1.0;
	
    // Transform object-space normals to eye-space
    float3 vNormalES = mul((float3x3)matNormal, vNormal);

    // Pass everything off to the fragment shader
    vso.vPosition  = vPositionCS;
    vso.vNormalES  = vNormalES;
    vso.vViewVecES = float3(0.0,0.0,0.0) - vPositionES.xyz;

    return vso;
}


