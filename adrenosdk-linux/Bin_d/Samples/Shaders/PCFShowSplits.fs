
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

#define SHOW_SPLITS 1

uniform sampler2D s_texture_8;

in vec4 VaryingTexCoord[6];

out vec4 g_FragColor;

float ChebyshevUpperBound(vec2 Moments, float t)  
{  
	float g_MinVariance = 0.001;

	// One-tailed inequality valid if t > Moments.x  
	float p = float(t <= Moments.x); 

	// Compute variance.  
	float Variance = Moments.y - (Moments.x*Moments.x);
	Variance = max(Variance, g_MinVariance);

	// Compute probabilistic upper bound.  
	float d = t - Moments.x;  
	float p_max = Variance / (Variance + d*d);  
	return max(p, p_max);
} 

vec2 ComputeMoments(float Depth)
{ 
	vec2 Moments;  
	// First moment is the depth itself.  
	Moments.x = Depth;  
	
	// Compute partial derivatives of depth.  
	float dx = dFdx(Depth);  
	float dy = dFdy(Depth); 
	 
	// Compute second moment over the pixel extents.  
	Moments.y = Depth*Depth + 0.25*(dx*dx + dy*dy);  
	return Moments;  
}

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
		vec4 sample_texture;
		
		sample_texture = texture(s_texture_8, sample_texcoord + vec2(-1.0, -1.0) / 1024.0);
		shadow += float(sample_texture.x > sample_distance - 0.01250);
		
		sample_texture = texture(s_texture_8, sample_texcoord + vec2(-1.0, 0.0) / 1024.0);
		shadow += float(sample_texture.x > sample_distance - 0.01250);

		sample_texture = texture(s_texture_8, sample_texcoord + vec2(-1.0, 1.0) / 1024.0);
		shadow += float(sample_texture.x > sample_distance - 0.01250);
		
		sample_texture = texture(s_texture_8, sample_texcoord + vec2(0.0, -1.0) / 1024.0);
		shadow += float(sample_texture.x > sample_distance - 0.01250);
		
		sample_texture = texture(s_texture_8, sample_texcoord + vec2(0.0, 0.0) / 1024.0);
		shadow += float(sample_texture.x > sample_distance - 0.01250);

		sample_texture = texture(s_texture_8, sample_texcoord + vec2(0.0, 1.0) / 1024.0);
		shadow += float(sample_texture.x > sample_distance - 0.01250);
		
		sample_texture = texture(s_texture_8, sample_texcoord + vec2(1.0, -1.0) / 1024.0);
		shadow += float(sample_texture.x > sample_distance - 0.01250);
		
		sample_texture = texture(s_texture_8, sample_texcoord + vec2(1.0, 0.0) / 1024.0);
		shadow += float(sample_texture.x > sample_distance - 0.01250);

		sample_texture = texture(s_texture_8, sample_texcoord + vec2(1.0, 1.0) / 1024.0);
		shadow += float(sample_texture.x > sample_distance - 0.01250);
		
		shadow /= 9.0;
	}
			
	g_FragColor = color * shadow;
}
