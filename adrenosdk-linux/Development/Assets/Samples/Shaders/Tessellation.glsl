//--------------------------------------------------------------------------------------
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

// Desc: Port of ShaderX7 Chapter 2.5 
// An Efficient and Physically Plausible Real Time Shading Model
//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[TessVS]
#version 300
#define NUM_VERTS 1900//If this is too big, nothing will draw and no error will print!

#define N 100
#include "CommonVS.glsl"


uniform   vec3 g_bufVertices [NUM_VERTS*2];
//uniform   int g_bufIndices  [NUM_VERTS];

uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;


uniform   vec3 g_vLightPos;

attribute vec4 g_vPositionOS;
attribute vec4 g_vNormalOS;

varying   vec3 g_vNormalES;
varying   vec3 g_vViewVecES;

varying vec3 NIn;
varying vec3 LIn;
varying vec3 VIn;

varying vec3 vColor;

uniform vec3 g_SkyColor;
uniform vec3 g_GroundColor;

void main()
{

	//vec4 v = vec4( 1., 1., 1., 0.0) * g_bufIndices[ gl_VertexID ];//vec4( g_bufVertices[ gl_VertexID ], 1.0);

	//float f = 1.03*dot(g_vNormalOS, normalize(v));// * vec4( 0.1, 0.1, 0.1, 0.0);

	vec4 vPositonOS = vec4( g_bufVertices[ (gl_VertexID)*2], 1.0);//g_vPositionOS + g_vPositionOS*f; 
	vec4 vNormalOS = vec4( g_bufVertices[ (gl_VertexID)*2 + 1 ], 1.0);

	vec4 P = g_matModelView * vPositonOS;
	
	NIn = g_matNormal * vNormalOS.xyz;
	
	LIn = g_vLightPos - P.xyz;
	
	VIn = vec3(0.0,0.0,0.0) - P.xyz;
	
    gl_Position = g_matModelViewProj * vPositonOS;
    
    vColor = g_SkyColor * dot( NIn, vec3( 0., 1., 0.)) + g_GroundColor * dot( NIn, vec3( 0., -1., 0.));
	
}
//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[TessFS]
#include "CommonFS.glsl"

uniform vec3 g_MaterialSpecular;
uniform vec3 g_MaterialDiffuse;
uniform vec3 g_LightColor;
uniform float g_MaterialGlossiness;
	
varying vec3 NIn;
varying vec3 LIn;
varying vec3 VIn;

varying vec3 vColor;

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

    // components
    float atten = 25.0 / dot( LIn, LIn );
    vec3 LightIntens = atten * LightColor;  
    
      
    vec3 AmbIntens = ( vColor ) / 2.0 * ( MaterialDiffuse + MaterialSpecular ) / 2.0;
    vec3 SpecIntens = MaterialSpecular *  pow ( dotNH, SpecularExponent + 1.0) * LightIntens;
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

