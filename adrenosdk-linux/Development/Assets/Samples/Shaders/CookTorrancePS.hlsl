
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// The fragment shader
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
	
struct PixelShaderInput
{
    float4 vPosition : SV_POSITION;
    float3 NIn: TEXCOORD0;
    float3 LIn: TEXCOORD1;
    float3 VIn: TEXCOORD2;
    float3 vColor : COLOR;
};

half BlinnPhongDistribution( half dotNH, half SpecularExponent )
{
    half D = pow( dotNH, SpecularExponent ) * ( SpecularExponent + 1.0 ) / 2.0;
    return D;
}

half3 FresnelOptimized( half3 R, half c )
{
    half3 F = lerp( R, clamp( 60.0 * R, 0.0, 1.0 ), pow(1.0 - c, 4.0) ); 
    return F;
}

half4 main(PixelShaderInput input) : SV_TARGET
{

    // input vectors
    half3 N = normalize( input.NIn );
	half3 V = normalize( input.VIn );
	half3 L = normalize( input.LIn );
    half3 H = normalize( L + V );
        
    // dot products    
	half dotNL = saturate( dot( N, L ) );
	half dotNH = saturate( dot( N, H ) );
    half dotNV = saturate( dot( N, V ) );
	half dotLH = saturate( dot( L, H ) );
    
    // inverse gamma conversion
    half3 MaterialSpecular = vMaterialSpecular.rgb * vMaterialSpecular.rgb;
    half3 MaterialDiffuse = vMaterialDiffuse.rgb * vMaterialDiffuse.rgb;
    half3 LightColor = vLightColor.rgb * vLightColor.rgb;

    // glossyness to distribution parameter
    half SpecularExponent = exp2( fMaterialGlossiness * 12.0 );    

    // distribution term
    half D = BlinnPhongDistribution( dotNH, SpecularExponent );
    
    // fresnel term
    half3 F = FresnelOptimized( MaterialSpecular, dotLH );

    // geometric term
    half G = saturate( 2.0 * dotNH * min( dotNV, dotNL ) / dotLH );

    // specular reflectance distribution from values above
    half3 SpecBRDF = D * F * G / ( 4.0 * dotNV * dotNL );
    
    // components
    half atten = 25.0 / dot( input.LIn, input.LIn );
    half3 LightIntens = atten * LightColor;    
    half3 AmbIntens = ( input.vColor ) / 2.0 * ( MaterialDiffuse + MaterialSpecular ) / 2.0;
    half3 SpecIntens = SpecBRDF * dotNL * LightIntens;
    half3 DiffIntens = MaterialDiffuse * dotNL * LightIntens;
    
    // color sum
    half3 result = half3(0.0, 0.0, 0.0);    
    result += AmbIntens;
    result += SpecIntens;
    result += DiffIntens;

    // tone mapping
    result = 1.0 - exp( -result );
    
    // gamma conversion    
    result = sqrt( max( result, 0.0 ) );
    
  	return half4( result, 1.0 );
}
