
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------




/******************************************************************************\
* Fragment shader
\******************************************************************************/

#version 300 es

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



uniform vec4 gShadowSpheres[4];
uniform mat4 modelviewprojections[4];
uniform sampler2D s_texture_8;

in vec4 WorldSpace;
in vec4 color;

out vec4 g_FragColor;

void main() 
{
	// 1. calculate distance between two points
	// 2. compare to the radius. If smaller it is in the sphere
	// Original equation
	// inside = (sqrt((WorldSpace.x - CenterOfSphere.x)^2 + (WorldSpace.y - CenterOfSphere.y)^2 + (WorldSpace.z - CenterOfSphere.z)^2) < RadiusOfSphere
	// The equation is simplified like this
	// inside = ((CenterOfSphere.x - WorldSpace.x)^2 + (CenterOfSphere.y - WorldSpace.y)^2 + (CenterOfSphere.z - WorldSpace.z)^2) < RadiusOfSphere^2
	// For certain hardware platforms those are too many constants. Check out [Valient] for a solution that only utilizes texture coordinate registers

	vec4 dist;
	dist.x = dot((WorldSpace.xyz - gShadowSpheres[0].xyz), (WorldSpace.xyz - gShadowSpheres[0].xyz));
	dist.y = dot((WorldSpace.xyz - gShadowSpheres[1].xyz), (WorldSpace.xyz - gShadowSpheres[1].xyz));
	dist.z = dot((WorldSpace.xyz - gShadowSpheres[2].xyz), (WorldSpace.xyz - gShadowSpheres[2].xyz));
	dist.w = dot((WorldSpace.xyz - gShadowSpheres[3].xyz), (WorldSpace.xyz - gShadowSpheres[3].xyz));
	
	vec4 sphereColor = vec4(1.0, 0.0, 1.0, 1.0);
	float shadow = 1.0;
	
	vec4 pos;
	float depth;
	
	if(dist.x < gShadowSpheres[0].w)
	{
		sphereColor = vec4(1.0, 0.0, 0.0, 1.0);
		pos = modelviewprojections[0] * vec4(WorldSpace.xyz, 1.0);
		depth = texture(s_texture_8, pos.xy).x;
		shadow = clamp(2.0 - exp((pos.z - depth - 0.00450) * 100.0), 0.0, 1.0);
	}
	else if(dist.y < gShadowSpheres[1].w)
	{
		sphereColor = vec4(0.0, 1.0, 0.0, 1.0);
		pos = modelviewprojections[1] * vec4(WorldSpace.xyz, 1.0);
		depth = texture(s_texture_8, pos.xy).x;
		shadow = clamp(2.0 - exp((pos.z - depth - 0.00450) * 100.0), 0.0, 1.0);
	}
	else if(dist.z < gShadowSpheres[2].w)
	{
		sphereColor = vec4(1.0, 1.0, 0.0, 1.0);
		pos = modelviewprojections[2] * vec4(WorldSpace.xyz, 1.0);
		depth = texture(s_texture_8, pos.xy).x;
		shadow = clamp(2.0 - exp((pos.z - depth - 0.00450) * 100.0), 0.0, 1.0);
	}
	else if(dist.w < gShadowSpheres[3].w)
	{
		sphereColor = vec4(0.0, 0.0, 1.0, 1.0);
		pos = modelviewprojections[3] * vec4(WorldSpace.xyz, 1.0);
		depth = texture(s_texture_8, pos.xy).x;
		shadow = clamp(2.0 - exp((pos.z - depth - 0.00450) * 100.0), 0.0, 1.0);
	}

	g_FragColor = sphereColor * color * shadow;
}

