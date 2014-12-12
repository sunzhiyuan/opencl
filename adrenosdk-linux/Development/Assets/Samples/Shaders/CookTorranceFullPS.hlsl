
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
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

half BeckmannDistribution( half dotNH, half SpecularExponent )
{
    half invm2 = SpecularExponent / 2.0;
    half D = exp( -cos(tan( dotNH )) * invm2 ) / pow( dotNH, 4.0 ) * invm2;
    return D;
}

half3 FresnelFull( half3 R, half c )
{
    // convert reflectance R into (real) refractive index n
    half3 n = (1.0 + sqrt(R))/(1.0-sqrt(R));
    
    half sinC = sin(c);
    half cos2sinC = cos( sinC );
    cos2sinC *= cos2sinC;
    
    half3 sqrtOneMinusPow = sqrt(1.0-pow(cos2sinC/n,half3(2.0, 2.0, 2.0)));
    
    // then use Fresnel eqns to get angular variance
    half3 FS = (c-n*sqrtOneMinusPow)/(c+n*sqrtOneMinusPow);
    half3 FP = (sqrtOneMinusPow-n*c)/(sqrtOneMinusPow+n*c);
    
    return (FS*FS+FP*FP)/2.0;
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
    half D = BeckmannDistribution( dotNH, SpecularExponent );
    
    // fresnel term
    half3 F = FresnelFull( MaterialSpecular, dotLH );

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

