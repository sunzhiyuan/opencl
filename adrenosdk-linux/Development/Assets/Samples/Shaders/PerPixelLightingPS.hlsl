

//--------------------------------------------------------------------------------------
// File: PerPixelLightingPS.hlsl
// Desc: Shaders for per-pixel lighting
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
cbuffer PerPixelLightingConstantBuffer
{
    float4x4 matModelView;
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
    float3 vNormalES : TEXCOORD0;
    float3 vViewVecES : TEXCOORD1;
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
