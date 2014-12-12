
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------




/******************************************************************************\
*
* Vertex shader
*
\******************************************************************************/
#version 300 es

in vec4 g_vPositionOS;
in vec4 g_vNormalOS;

uniform mat4 g_matModelViewProj;
uniform mat4 g_matWorld;

uniform mat4 modelviewprojections[4];
uniform vec3 offsets[4];
uniform vec3 light;
uniform   mat3 g_matNormal;

out vec4 VaryingTexCoord[6];

void main()
{
	vec4 vertex = g_vPositionOS;
	
	vertex = g_matWorld * vertex;
	
	vec3 normal = g_matNormal * g_vNormalOS.xyz;
	vec4 color = vec4(0.75,0.75,1.0,1.0);
	
	vec3 lightNormalized = normalize( light );
	
	gl_Position = g_matModelViewProj * g_vPositionOS;
	
	VaryingTexCoord[0] = color * (dot(normal,lightNormalized) * 0.5 + 0.5);
	
	VaryingTexCoord[1] = modelviewprojections[0] * g_matWorld * g_vPositionOS;
	VaryingTexCoord[2] = modelviewprojections[1] * g_matWorld * g_vPositionOS;
	VaryingTexCoord[3] = modelviewprojections[2] * g_matWorld * g_vPositionOS;
	VaryingTexCoord[4] = modelviewprojections[3] * g_matWorld * g_vPositionOS;
	
	VaryingTexCoord[5].x = dot((offsets[0] - vertex.xyz), lightNormalized) * 0.00502765209;
	VaryingTexCoord[5].y = dot((offsets[1] - vertex.xyz), lightNormalized) * 0.00502765209;
	VaryingTexCoord[5].z = dot((offsets[2] - vertex.xyz), lightNormalized) * 0.00502765209;
	VaryingTexCoord[5].w = dot((offsets[3] - vertex.xyz), lightNormalized) * 0.00502765209;

}
