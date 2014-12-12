
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// File: CompressedGeometry8PS.hlsl
// Desc: Simple shader demonstrating geometry compression -an OpenglES3.0 port of 
//  Dean Calver's Demo:"Using Vertex Shaders For Geometry Compression" in Shaders X2
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The fragment shader
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

struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float3 vNormalES : NORMAL;
    float3 vViewVecES : TEXCOORD0;
};

half4 main(PixelShaderInput input) : SV_TARGET
{
    // Normalize per-pixel vectors	
    half3 vNormal = normalize( input.vNormalES );
    half3 vLight  = normalize( vLightPos.xyz );
    half3 vView   = normalize( input.vViewVecES );
    half3 vHalf   = normalize( vLight + vView );

    // Compute the lighting in eye-space
    half fDiffuse  = max( 0.0, dot( vNormal, vLight ) );
    half fSpecular = pow( max( 0.0, dot( vNormal, vHalf ) ), vSpecular.a );

    // Combine lighting with the material properties
    half4 outColor;
    outColor.rgba  = vAmbient.rgba;
    outColor.rgba += vDiffuse.rgba * fDiffuse;
    outColor.rgb  += vSpecular.rgb * fSpecular;

    return outColor;
}
