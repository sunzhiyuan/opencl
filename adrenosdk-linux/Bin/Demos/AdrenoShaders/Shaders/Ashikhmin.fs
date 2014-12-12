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
// This no longer supported on Droid.
// #extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



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
varying   vec3 g_BiTangent;


uniform sampler2D g_WardTexture;
varying vec2 g_vOutTexCoord;
varying vec4 g_vLightVecES;

void main()
{
	const float PI = 3.141592653589793238462643383279502884197169399375105820974944592;
	const float RI = 0.7;

	const float mx = 1000.0;
	const float my = 10.0;
	const float A = 1.0 / (8.0 * PI) * sqrt((mx + 1.0) * (my + 1.0));

	const float kd = 0.3;
	float ks = 1.0 - kd;

	vec3 N = normalize(g_vNormalES);
	vec3 Bi = normalize(g_BiTangent);
	vec3 T = normalize(g_Tangent);
	vec3 V = normalize( g_vViewVecES );

	vec3 L = normalize( g_vLightVecES.xyz );
	vec3 H = normalize( L + V );

	float NV = saturate(dot(N, V));
	float NL = saturate(dot(N, L));
	float NH = saturate(dot(N, H));
	float HL = saturate(dot(H, L));
	float T1H = dot(Bi, H);
	float T2H = dot(T, H);

	// Calculate diffuse
	// Rd = 28 * kd (1 - ks) (1 - (1 - N.V)5) * (1 - (1 - N.L)5)
	// 	    23p                       2                  2    
	float Rd = (28.0 /(23.0 * PI)) * (1.0 - pow(1.0 - (NV / 2.0), 5.0)) * (1.0 - pow(1.0 - (NL / 2.0), 5.0));

	// Calculate specular 
	//                B                               
	// 	 (mx * T1.H^2 + my * T2.H^2) / (1 - (H.N)^2)
	// N.H
	float B = (pow(NH,(mx * T1H * T1H + my * T2H * T2H) / (1.0 - NH * NH)));   

	//      F
	//RI + (1 - RI)(1 - H.L)^5
	//HL * max(NV, NL)
	float F = (RI + (1.0 - RI) * pow((1.0 - HL), 5.0)) / HL * max(NV, NL);

	//         A               
	// sqrt(mx + 1)(my + 1) 
	// 	   8PI       
	float Rs = A * B * F;

	// Lr = N.L (kd * (1 - ks) * Dc + ks * Sc)
	// return Color + NL * (kd * (1 - ks) * Rd + ks * Rs);
	float Color =  NL * (kd * (1.0 - ks) * Rd + ks * Rs);
	
	float Diffuse = 0.3 * NL;

	gl_FragColor = vec4(Color + Diffuse, Color + Diffuse, Color + Diffuse, 1.0);
}
