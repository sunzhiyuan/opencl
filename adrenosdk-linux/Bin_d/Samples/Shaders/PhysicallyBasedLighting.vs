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
// The vertex shader
//--------------------------------------------------------------------------------------

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
	vec4 P = g_matModelView * g_vPositionOS;
	
	NIn = g_matNormal * g_vNormalOS.xyz;
	
	LIn = g_vLightPos - P.xyz;
	
	VIn = vec3(0.0,0.0,0.0) - P.xyz;
	
    gl_Position = g_matModelViewProj * g_vPositionOS;
    
    vColor = g_SkyColor * dot( NIn, vec3( 0., 1., 0.)) + g_GroundColor * dot( NIn, vec3( 0., -1., 0.));
}
