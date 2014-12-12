//--------------------------------------------------------------------------------------
// File: CookTorrance.glsl
// Desc: Shaders for per-pixel lighting
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
[CookTorranceVS]
#include "CommonVS.glsl"

uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;
attribute vec2 g_vTexCoord;

varying   vec3 g_vNormalES;
varying   vec3 g_vViewVecES;
varying   vec4 g_vLightVecES;

varying vec2 g_vOutTexCoord;
uniform vec3 g_vLightPos;


void main()
{
    vec4 vPositionES = g_matModelView     * g_vPositionOS;
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;

    // Transform object-space normals to eye-space
    vec3 vNormalES = g_matNormal * g_vNormalOS;

    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    g_vNormalES  = vNormalES.xyz;
    g_vViewVecES = vec3(0.0,0.0,0.0) - vPositionES.xyz;
    
    g_vLightVecES.xyz = normalize(g_vLightPos - vPositionES.xyz);
    
    float LightRange = 0.1;        
    g_vLightVecES.w = max( 0.0, 1.0 - dot(g_vLightVecES.xyz * LightRange, g_vLightVecES.xyz * LightRange));
  
//    g_vOutTexCoord = g_vTexCoord; // * g_vTexScale;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[CookTorranceFS]
#include "CommonFS.glsl"

struct MATERIAL
{
    vec4  vAmbient;
    vec4  vDiffuse;
    vec4  vSpecular;
};

uniform MATERIAL g_Material;

varying vec3     g_vViewVecES;
varying vec3     g_vNormalES;

uniform sampler2D g_sCTTexture;
varying vec2 g_vOutTexCoord;
varying vec4 g_vLightVecES;

void main()
{
    // copper-like color RGB(184, 115, 51)
	vec4 C = vec4(184.0 / 255.0, 115.0 / 255.0, 51.0 / 255.0, 1.0);
	
    const float m = 0.2;	
    const float RI = 0.01;
    const float PI = 3.14159926535;
   
    // Normalize per-pixel vectors	
    vec3 N = normalize( g_vNormalES );
    vec3 L = normalize( g_vLightVecES.xyz );
    vec3 V = normalize( g_vViewVecES );
    vec3 H = normalize( L + V);
    
	float NH = max( 0.0, dot(N, H));
    float VH = max( 0.0, dot(V,H));
	float NV = dot(N, V);
	float NL = max( 0.0, dot(L, N));
	
    // Beckmann's distribution function
    float NH2 = NH * NH;
    float m2 = m * m;
    float D = (1.0 / m2 * NH2 * NH2) * (exp( -((1.0 - NH2) / (m2 * NH2))));

    // Fresnel
    float F = RI + (1.0 - RI) * pow((1.0 - NV), 5.0);   
   
    // geometric term
    float G = min(1.0, min((2.0 * NH * NL) / VH, (2.0 * NH * NV) / VH));
   
    float S = (F * D * G) / (PI * NL * NV);
	
	//------------------------------
	// Original: Ir = Ai * Ac + Ii(N.L)dw * (Si * Sc + Di * Dc)
	// My formula: Ir = Ia * C + N.L * sat(lidw * (Di * Dc * C + Si * Sc))
	//------------------------------
	const float Ai = 0.1;
	const float dw = 3.0;
	const float Di = 0.7;
	const float Si = 0.3;
	//gl_FragColor.rgba  = Ai * C + (NL * saturate(dw * ((Di * NL * C) + (Si * S)))) * g_vLightVecES.w;
	gl_FragColor.rgba  = Ai * C + (NL * saturate(dw * ((Di * NL * C) + (Si * S)))) * g_vLightVecES.w;
	
}
