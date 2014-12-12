

//--------------------------------------------------------------------------------------
// File: Skinning2VS.hlsl
// Desc: Skinning2 vertex shader
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


struct VertexShaderInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;   
    uint4 BoneIndex : BLENDINDICES;
    float3 BoneWeights : BLENDWEIGHT;
    float2 TexCoord : TEXCOORD0;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 Tex0 : TEXCOORD0;
    float3 Light : TEXCOORD1;
    float3 EyeNormal : TEXCOORD2;
};

//--------------------------------------------------------------------------------------
// Helper functions for transforming a float3/float4 by a bone
//--------------------------------------------------------------------------------------
float3 MulBone3( float3 vInputPos, uint nBone, float fBlendWeight )
{
	float3 vResult;
	
	uint nMatrix = 3 * nBone;
	
    vResult.x = dot( vInputPos, g_matWorldArray[nMatrix + 0].xyz );
    vResult.y = dot( vInputPos, g_matWorldArray[nMatrix + 1].xyz );
    vResult.z = dot( vInputPos, g_matWorldArray[nMatrix + 2].xyz );
    
    return vResult * fBlendWeight;
}

float3 MulBone4( float4 vInputPos, uint nBone, float fBlendWeight )
{
    float3 vResult;
    
    uint nMatrix = 3 * nBone;
    
    vResult.x = dot( vInputPos, g_matWorldArray[nMatrix + 0].xyzw );
    vResult.y = dot( vInputPos, g_matWorldArray[nMatrix + 1].xyzw );
    vResult.z = dot( vInputPos, g_matWorldArray[nMatrix + 2].xyzw );
    
    return vResult * fBlendWeight;
}

//--------------------------------------------------------------------------------------
// Vertex shader entry point
//--------------------------------------------------------------------------------------
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput vso;

	// Get bone indicse
    uint3 BoneIndices;
    BoneIndices.x = input.BoneIndex.x;
#if defined( USE_THREE_BONES ) || defined( USE_TWO_BONES )
    BoneIndices.y = input.BoneIndex.y;  
#elif defined( USE_THREE_BONES )
    BoneIndices.z = input.BoneIndex.z;
#endif
    
	// Transform position
    float3 vPosition = MulBone4( float4( input.Position, 1.0 ), BoneIndices.x, input.BoneWeights.x );
#if defined( USE_TWO_BONES ) || defined( USE_THREE_BONES )
    vPosition += MulBone4( float4( input.Position, 1.0 ), BoneIndices.y, input.BoneWeights.y );
#endif
#if defined( USE_THREE_BONES )
    vPosition += MulBone4( float4( input.Position, 1.0 ), BoneIndices.z, input.BoneWeights.z );
#endif
    
    // Transform normal
    float3 vNormal = MulBone3( input.Normal, BoneIndices.x, input.BoneWeights.x );
#if defined( USE_TWO_BONES ) || defined( USE_THREE_BONES )
    vNormal += MulBone3( input.Normal, BoneIndices.y, input.BoneWeights.y );
#endif
#if defined( USE_THREE_BONES )
    vNormal += MulBone3( input.Normal, BoneIndices.z, input.BoneWeights.z );
#endif
	vNormal = normalize( vNormal );
	
	// Transform tangent
	float3 vTangent = MulBone3( input.Tangent, BoneIndices.x, input.BoneWeights.x );
#if defined( USE_TWO_BONES ) || defined( USE_THREE_BONES )
    vTangent += MulBone3( input.Tangent, BoneIndices.y, input.BoneWeights.y );
#endif
#if defined( USE_THREE_BONES )
    vTangent += MulBone3( input.Tangent, BoneIndices.z, input.BoneWeights.z );
#endif
	vTangent = normalize( vTangent );
	
	// Transform binormal
	float3 vBinormal = MulBone3( input.Binormal, BoneIndices.x, input.BoneWeights.x );
#if defined( USE_TWO_BONES ) || defined( USE_THREE_BONES )
    vBinormal += MulBone3( input.Binormal, BoneIndices.y, input.BoneWeights.y );
#endif
#if defined( USE_THREE_BONES )
    vBinormal += MulBone3( input.Binormal, BoneIndices.z, input.BoneWeights.z );
#endif
	vBinormal = normalize( vBinormal );
	
	// Apply view and projection to position
    float4 vPositionWS = float4( vPosition.xyz, 1.0 );
    float4 vPositionES = mul( g_matView,  vPositionWS );
    float4 vPositionCS = mul( g_matProj,  vPositionES );
    
    vso.Position = vPositionCS;
    
    // Pass light and eye normal
    float3x3 TBNMatrix = float3x3( vTangent, vBinormal, vNormal );
    vso.Light = mul( ( g_lightPos.xyz - vPosition.xyz ), TBNMatrix );
    vso.EyeNormal = mul( ( g_cameraPos.xyz - vPosition.xyz ), TBNMatrix );

	// Pass texture coordinates
    vso.Tex0 = float2( input.TexCoord.x, input.TexCoord.y );

    return vso;
}

