//--------------------------------------------------------------------------------------
// File: Ward.glsl
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
[WardVS]
#include "CommonVS.glsl"

uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;
attribute vec2 g_vTexCoord;
attribute vec3 g_vTangent;


varying   vec3 g_vNormalES;
varying   vec3 g_vViewVecES;
varying   vec4 g_vLightVecES;
varying   vec3 g_Tangent;

varying vec2 g_vOutTexCoord;
uniform vec3 g_vLightPos;


void main()
{
    vec4 vPositionES = g_matModelView     * g_vPositionOS;
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;

    // Transform object-space normals to eye-space
    vec3 vNormalES = normalize(g_matNormal * g_vNormalOS);
    g_Tangent = normalize(g_matNormal * g_vTangent);

    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    g_vNormalES  = vNormalES.xyz;
    g_vViewVecES = vec3(0.0,0.0,0.0) - vPositionES.xyz;
    
    g_vLightVecES.xyz = normalize(g_vLightPos - vPositionES.xyz);
    
    float LightRange = 0.1;        
    g_vLightVecES.w = max( 0.0, 1.0 - dot(g_vLightVecES.xyz * LightRange, g_vLightVecES.xyz * LightRange));
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[WardFS]
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
varying vec3 g_Tangent;

uniform sampler2D g_WardTexture;
varying vec2 g_vOutTexCoord;
varying vec4 g_vLightVecES;

void main()
{
	const float Si = 0.7;
	const float Di = 0.3;
	
    vec3 L = normalize( g_vLightVecES.xyz );
    vec3 V = normalize( g_vViewVecES );
    vec3 Half = normalize( L + V );

	vec3 N = normalize(g_vNormalES);
	vec3 T = normalize(g_Tangent);
	
    float NL = saturate(dot(L, N));

	float NH = saturate(dot(Half, N));
	
	vec3 P = normalize(Half - NH * N);
	float TP = 0.5 * dot(T,P) + 0.5;
	
	//												( (T.P)2 + (T.P)2  ) * (N.H)2 - 1
	//	Lr = ( pd N.L + ks    1          1       exp      mx2      my2        (N.H)2    ) * N.L
	//		   			    vN.L N.V  4p * mx * my        
	vec4 S = texture2D( g_WardTexture, vec2(NH, TP));
 
	// unpack the 8:8:8:8 look-up texture
	float Spec = (dot( S, vec4( 1.0, 256.0, 65536.0, 0.0 ) ) * 5.0);
	
	const vec4 Color = vec4(1.0, 1.0, 1.0, 0.0);
	
	gl_FragColor = (Di * Color * NL + Si * Spec) * NL;	
}
