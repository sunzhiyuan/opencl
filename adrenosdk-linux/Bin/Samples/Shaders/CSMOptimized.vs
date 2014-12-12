
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



/******************************************************************************\
* Vertex shader
\******************************************************************************/

#version 300 es

in vec4 g_vPositionOS;
in vec4 g_vNormalOS;

uniform mat4 g_matModelViewProj;
uniform mat4 g_matWorld;

uniform vec3 light;
uniform mat3 g_matNormal;

out vec4 color;
out vec4 WorldSpace;

void main()
{
	WorldSpace = g_matWorld * g_vPositionOS;
	
	vec3 normal = g_matNormal * g_vNormalOS.xyz;
	color = vec4(0.75,0.75,1.0,1.0);
	
	gl_Position = g_matModelViewProj * g_vPositionOS;
	
	color = color * (dot(normal,light) * 0.5 + 0.5);
}
