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


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// File: CommonFS.glsl
// Desc: Useful common shader code for fragment shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

// default to medium precision
precision mediump float;

// OpenGL ES require that precision is defined for a fragment shader
// usage example: varying NEED_HIGHP vec2 vLargeTexCoord;
#ifdef GL_FRAGMENT_PRECISION_HIGH
   #define NEED_HIGHP highp
#else
   #define NEED_HIGHP mediump
#endif

// Enable supported extensions
#extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



uniform vec3 g_MaterialSpecular;
uniform vec3 g_MaterialDiffuse;
uniform vec3 g_LightColor;
uniform float g_MaterialGlossiness;
	
varying vec3 NIn;
varying vec3 LIn;
varying vec3 VIn;

varying vec3 vColor;

float BeckmannDistribution( float dotNH, float SpecularExponent )
{
    float invm2 = SpecularExponent / 2.0;
    float D = exp( -cos(tan( dotNH )) * invm2 ) / pow( dotNH, 4.0 ) * invm2;
    return D;
}

vec3 FresnelFull( vec3 R, float c )
{
    // convert reflectance R into (real) refractive index n
    vec3 n = (1.0 + sqrt(R))/(1.0-sqrt(R));
    
    float sinC = sin(c);
    float cos2sinC = cos( sinC );
    cos2sinC *= cos2sinC;
    
    vec3 sqrtOneMinusPow = sqrt(1.0-pow(cos2sinC/n,vec3(2.0)));
    
    // then use Fresnel eqns to get angular variance
    vec3 FS = (c-n*sqrtOneMinusPow)/(c+n*sqrtOneMinusPow);
    vec3 FP = (sqrtOneMinusPow-n*c)/(sqrtOneMinusPow+n*c);
    
    return (FS*FS+FP*FP)/2.0;
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
    float D = BeckmannDistribution( dotNH, SpecularExponent );
    
    // fresnel term
    vec3 F = FresnelFull( MaterialSpecular, dotLH );

    // geometric term
    float G = saturate( 2.0 * dotNH * min( dotNV, dotNL ) / dotLH );

    // specular reflectance distribution from values above
    vec3 SpecBRDF = D * F * G / ( 4.0 * dotNV * dotNL );
    
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
    
  	gl_FragColor = vec4( result, 1.0 );
}
