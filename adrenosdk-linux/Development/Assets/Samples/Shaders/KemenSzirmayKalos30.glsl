//--------------------------------------------------------------------------------------
// File: PhysicallyBasedLighting.glsl
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


[KemenSzirmayKalos30FS]

#version 300 es

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

#include "CommonFS.glsl"

uniform vec3 g_MaterialSpecular;
uniform vec3 g_MaterialDiffuse;
uniform vec3 g_LightColor;
uniform float g_MaterialGlossiness;
	
in vec3 NIn;
in vec3 LIn;
in vec3 VIn;

in vec3 vColor;

out vec4 g_FragColor;

float BlinnPhongDistribution( float dotNH, float SpecularExponent )
{
    float D = pow( dotNH, SpecularExponent ) * ( SpecularExponent + 1.0 ) / 2.0;
    return D;
}

vec3 FresnelOptimized( vec3 R, float c )
{
    vec3 F = mix( R, clamp( 60.0 * R, 0.0, 1.0 ), pow(1.0 - c, 4.0) ); 
    return F;
}

void main()
{
    // input vectors
    vec3 N = normalize( NIn );
	vec3 V = normalize( VIn );
	vec3 L = normalize( LIn );
    vec3 H = normalize( L + V );
        
    // dot products    
	float dotNL = saturate( dot( N, L ) );
	float dotNH = saturate( dot( N, H ) );
    float dotNV = saturate( dot( N, V ) );
	float dotLH = saturate( dot( L, H ) );
    
    // inverse gamma conversion
    vec3 MaterialSpecular = g_MaterialSpecular * g_MaterialSpecular;
    vec3 MaterialDiffuse = g_MaterialDiffuse * g_MaterialDiffuse;
    vec3 LightColor = g_LightColor * g_LightColor;

    // glossyness to distribution parameter
    float SpecularExponent = exp2( g_MaterialGlossiness * 12.0 );    

    // distribution term
    float D = BlinnPhongDistribution( dotNH, SpecularExponent );
    
    // fresnel term
    vec3 F = FresnelOptimized( MaterialSpecular, dotLH );

    // specular reflectance distribution from values above
    vec3 SpecBRDF = D * F  / ( 4.0 * dotLH * dotLH );
    
    // components
    float atten = 25.0 / dot( LIn, LIn );
    vec3 LightIntens = atten * LightColor;    
    vec3 AmbIntens = ( vColor ) / 2.0 * ( MaterialDiffuse + MaterialSpecular ) / 2.0;
    vec3 SpecIntens = SpecBRDF * dotNL * LightIntens;
    vec3 DiffIntens = MaterialDiffuse * dotNL * LightIntens;
    
    // color sum
    vec3 result = vec3(0.0);    
    result += AmbIntens;
    result += SpecIntens;
    result += DiffIntens;

    // tone mapping
    result = 1.0 - exp( -result );
    
    // gamma conversion    
    result = sqrt( result );
    
  	g_FragColor = vec4( result, 1.0 );
}
