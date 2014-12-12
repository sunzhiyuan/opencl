
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The fragment shader for terrain (uses SAME vert shader as the sky) 
//--------------------------------------------------------------------------------------
[HoffmanScatterTerrainFS]
#include "CommonFS.glsl"
varying vec3	Lin;
varying vec3	Fex;
varying vec3    g_vVSTexCoord;
varying vec3    LightVec;

uniform   sampler2D g_sBaseTexture; 
uniform   sampler2D g_sNormTexture;

uniform   mat4 g_matModelView;

void main()
{
	vec3 normal = 2.0 * (texture2D( g_sNormTexture, g_vVSTexCoord.xy ).xyz - 0.5);
	normal = vec4(g_matModelView * vec4( normal, 0.0 )).xyz;
	
	vec3 light  = vec3(saturate( dot( normal, LightVec ) ));
	vec3 albedo = texture2D( g_sBaseTexture, g_vVSTexCoord.xy ).xyz; 
 
    gl_FragColor.rgba  = vec4(Lin + (Fex * light * albedo), 1.);
} 
