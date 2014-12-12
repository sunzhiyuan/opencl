//--------------------------------------------------------------------------------------
// File: RenderTerrain.glsl
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//
// RenderTerrainVS, RenderTerrainPS
// renders the terrain using the texture atlas. atmospheric effects are added.
//
//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
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



uniform sampler2D terrainTexture;

uniform float gammaControl;
uniform float exposureControl;

in vec4 texcoord0;
in vec4 texcoord1;
in vec4 texcoord2;

out vec4 out_color;

void main()
{
	vec4 surfaceColor = texture( terrainTexture, texcoord0.xy );
	
	// decode higher dynamic range by using alpha
	surfaceColor.xyz = surfaceColor.xyz * surfaceColor.w * 4.0;
	
	// apply atmosphereic effects
	surfaceColor = surfaceColor * texcoord2 + texcoord1;
	
	// opt 1: exposure
	//surfaceColor = 1.0 - exp( -3.0 * surfaceColor );
	
	// opt 2: exposure + tone mapping
	surfaceColor = 1.0 - exp( exposureControl * surfaceColor );
	surfaceColor.xyz = pow( surfaceColor.xyz, vec3(gammaControl) );

	out_color = surfaceColor;
}

