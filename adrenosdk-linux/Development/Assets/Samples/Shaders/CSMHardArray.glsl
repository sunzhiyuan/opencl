
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

[HardArrayFS]
/******************************************************************************\
* Fragment shader
\******************************************************************************/

#version 300 es

#include "CommonFS.glsl"

//#define SHOW_SPLITS 1

uniform sampler2DArray s_texture_8;

in vec4 VaryingTexCoord[6];

out vec4 g_FragColor;

void main()
{
	float shadow = 0.0;
	vec4 color = VaryingTexCoord[0];
	
	// select split
	float sample_distance = 0.0;
	vec3 sample_texcoord = vec3(VaryingTexCoord[1].xy / VaryingTexCoord[1].w,0.0);
	if(max(abs(sample_texcoord.x - 0.5),abs(sample_texcoord.y - 0.5)) < 0.48)
	{
		sample_distance = VaryingTexCoord[5].x;
		#ifdef SHOW_SPLITS
			color = vec4(1.0,0.0,0.0,1.0);
		#endif
	}
	else
	{
		sample_texcoord.xy = VaryingTexCoord[2].xy / VaryingTexCoord[2].w;
		if(max(abs(sample_texcoord.x - 0.5),abs(sample_texcoord.y - 0.5)) < 0.48)
		{
			sample_distance = VaryingTexCoord[5].y;
			sample_texcoord.z = 1.0;
			#ifdef SHOW_SPLITS
				color = vec4(0.0,1.0,0.0,1.0);
			#endif
		}
		else
		{
			sample_texcoord.xy = VaryingTexCoord[3].xy / VaryingTexCoord[3].w;
			if(max(abs(sample_texcoord.x - 0.5),abs(sample_texcoord.y - 0.5)) < 0.48)
			{
				sample_distance = VaryingTexCoord[5].z;
				sample_texcoord.z = 2.0;
				#ifdef SHOW_SPLITS
					color = vec4(0.0,0.0,1.0,1.0);
				#endif
			}
			else
			{
				sample_texcoord.xy = VaryingTexCoord[4].xy / VaryingTexCoord[4].w;
				if(max(abs(sample_texcoord.x - 0.5),abs(sample_texcoord.y - 0.5)) < 0.48)
				{
					sample_distance = VaryingTexCoord[5].w;
					sample_texcoord.z = 3.0;
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
		vec4 sample_texture = texture(s_texture_8, sample_texcoord); 
		shadow = float(sample_texture.x > sample_distance - 0.01250);
	}
			
	g_FragColor = color * shadow;
}

