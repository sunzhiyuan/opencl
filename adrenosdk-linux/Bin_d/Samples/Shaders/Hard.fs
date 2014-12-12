
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



//#define SHOW_SPLITS 1

uniform sampler2D s_texture_8;

in vec4 VaryingTexCoord[6];

out vec4 g_FragColor;

void main()
{
	float shadow = 0.0;
	vec4 color = VaryingTexCoord[0];
	
	// select split
	float sample_distance = 0.0;
	vec2 sample_texcoord = VaryingTexCoord[1].xy / VaryingTexCoord[1].w;
	if(max(abs(sample_texcoord.x - 0.25),abs(sample_texcoord.y - 0.25)) < 0.24)
	{
		sample_distance = VaryingTexCoord[5].x;
		#ifdef SHOW_SPLITS
			color = vec4(1.0,0.0,0.0,1.0);
		#endif
	}
	else
	{
		sample_texcoord = VaryingTexCoord[2].xy / VaryingTexCoord[2].w;
		if(max(abs(sample_texcoord.x - 0.75),abs(sample_texcoord.y - 0.25)) < 0.24)
		{
			sample_distance = VaryingTexCoord[5].y;
			#ifdef SHOW_SPLITS
				color = vec4(0.0,1.0,0.0,1.0);
			#endif
		}
		else
		{
			sample_texcoord = VaryingTexCoord[3].xy / VaryingTexCoord[3].w;
			if(max(abs(sample_texcoord.x - 0.25),abs(sample_texcoord.y - 0.75)) < 0.24)
			{
				sample_distance = VaryingTexCoord[5].z;
				#ifdef SHOW_SPLITS
					color = vec4(0.0,0.0,1.0,1.0);
				#endif
			}
			else
			{
				sample_texcoord = VaryingTexCoord[4].xy / VaryingTexCoord[4].w;
				if(max(abs(sample_texcoord.x - 0.75),abs(sample_texcoord.y - 0.75)) < 0.24)
				{
					sample_distance = VaryingTexCoord[5].w;
					#ifdef SHOW_SPLITS
						color = vec4(1.0,1.0,1.0,1.0);
					#endif
				}
				else
				{
					shadow = 1.0;
				}
			}
		}
	}
	
	// calculate shadow
	if(shadow == 0.0)
	{
		vec4 sample_texture = texture(s_texture_8,sample_texcoord);
		shadow = float(sample_texture.x > sample_distance - 0.01250);
	}
			
	g_FragColor = color * shadow;
}
