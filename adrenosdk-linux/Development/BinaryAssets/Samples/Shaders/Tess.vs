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
#version 300
#define NUM_VERTS 1900//If this is too big, nothing will draw and no error will print!

#define N 100
//--------------------------------------------------------------------------------------
// File: CommonVS.glsl
// Desc: Useful common shader code for vertex shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix




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
