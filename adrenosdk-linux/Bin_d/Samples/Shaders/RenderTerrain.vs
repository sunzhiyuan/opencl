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



// size of border for textures tiles (>=2)
// this size has to be increased, if mip-mapping for the tiles is applied!!!
#define	TILE_BORDER 4.0

in vec3 g_PositionIn;
in vec3 g_TexCoordIn;

uniform vec4 rtSize;
uniform vec4 texCoordBiasScale;
uniform vec4 cameraPosition;

uniform mat4 g_MatModelViewProj;

out vec4 texcoord0;
out vec4 texcoord1;
out vec4 texcoord2;

struct SCATTERING
{
    vec4 E, I;
};

SCATTERING	atmosphereScatter( float distance, vec3 toCamera )
{
	// constants for a fixed atmospheric condition
	vec4	CV_CONSTANTS              = vec4( 1.0, 1.4427, 0.5, 0.0 );
	vec4	CV_BETA_1_PLUS_2          = vec4( 4.61e-5, 6.65e-5, 0.000114, 0.0 );
	vec4	CV_TERRAIN_REFLECTANCE    = vec4( 0.014, 0.014, 0.014, 0.0 );
	vec4	CV_HG                     = vec4( 0.00599, 1.997, 1.994, 0.0 );
	vec4	CV_BETA_DASH_1            = vec4( 1.04e-6, 1.76e-6, 3.647e-6, 0.0 );
	vec4	CV_BETA_DASH_2            = vec4( 6.67e-6, 8.67e-6, 1.25e-5, 0.0 );
	vec4	CV_ONE_OVER_BETA_1_PLUS_2 = vec4( 21677.2, 15044.2, 8796.22, 0.0 );
	vec4	CV_TERM_MULTIPLIERS       = vec4( 1.0, 0.7, 0.0, 0.0 );
	vec4	sunDirection			  = vec4( -0.438, 0.509, 0.741, 0.0 );
	vec4	sunColor				  = vec4( 0.77, 0.70, 0.55, 100.0 );

	SCATTERING coeff;

	// theta: angle between sun direction and view direction
	vec4 r0_theta;

	r0_theta.x = dot( sunDirection.xyz, toCamera );
	r0_theta.y = r0_theta.x * r0_theta.x + 1.0;

	// r0_theta = [cos(theta), 1+cos^2(theta), s] 

	// extinction term E
	vec4 r1;

	r1 = exp2( -distance * CV_BETA_1_PLUS_2 );

	// apply reflectance to E to get total net effective E (total extinction)
	coeff.E = r1 * CV_TERRAIN_REFLECTANCE;

	// phase2(theta) = (1-g^2)/(1+g-2g*cos(theta))^(3/2)
	// CV_HG = [1-g^2, 1+g, 2g]
	vec4 r4, r5;

	r4.x = inversesqrt( CV_HG.z * r0_theta.x + CV_HG.y );
	r4.y = r4.x * r4.x;
	r4.x = r4.y * r4.x;
	r0_theta.w = r4.x * CV_HG.x;

	// inscattering (I) = (beta'_1 * phase_1(theta) + beta'_2 * phase_2(theta)) * 
	//                    [1-exp(-beta_1*s).exp(-beta_2*s)] / (beta_1 + beta_2)

	r4 = CV_BETA_DASH_1 * r0_theta.y;
	r5 = CV_BETA_DASH_2 * r0_theta.w;

	coeff.I  = r4 + r5;
	coeff.I *= 1.0 - r1;
	coeff.I *= CV_ONE_OVER_BETA_1_PLUS_2;
	coeff.I *= CV_TERM_MULTIPLIERS.y;

	coeff.I *= sunColor * sunColor.w;
	coeff.E *= sunColor * sunColor.w;

	return coeff;
}

void main()
{
	gl_Position = g_MatModelViewProj * vec4( g_PositionIn, 1.0 );
	
	// texture atlas coordinates
	texcoord0.xy = g_TexCoordIn.xy;
	texcoord0.xy -= 0.5;
	texcoord0.xy *= ( rtSize.z - 2.0 * TILE_BORDER - 0.0 ) / rtSize.z;
	texcoord0.xy += 0.5;
	
	texcoord0.xy *= texCoordBiasScale.zw;
	texcoord0.xy += texCoordBiasScale.xy;
	
	//texcoord0.y = 1.0 - texcoord0.y;
	
	// distance to camera
	vec3 toCamera = cameraPosition.xyz - g_PositionIn;
	float distance = length( toCamera ) * 0.1;
	
	toCamera = normalize( toCamera );
	
	SCATTERING scatter = atmosphereScatter( distance, toCamera );
	
	texcoord1 = scatter.I;
	texcoord2 = scatter.E;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
