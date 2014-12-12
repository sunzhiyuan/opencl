

//--------------------------------------------------------------------------------------
// File: Warp.glsl
// Desc: Warp shader for the ImageEffects sample
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
[SWEVS]
#include "CommonVS.glsl"


attribute vec4 g_vVertex;

varying vec2 otexcoord;	
									

void main()
{
    	gl_Position = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
	otexcoord = vec2( g_vVertex.z, g_vVertex.w );
}

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[SWEFS]
#include "CommonFS.glsl"

varying vec2 otexcoord;												

uniform sampler2D basemap;
uniform sampler2D envmap;
uniform sampler2D normalmap;

	
void main()
{
	vec4 coords  = texture2D( normalmap, otexcoord );
	vec4 skycolor = texture2D( envmap, coords.xy );
	vec4 groundcolor = texture2D( basemap, coords.zw );
	gl_FragColor = 0.6 * skycolor + 0.6 * groundcolor;
}
