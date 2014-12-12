//--------------------------------------------------------------------------------------
// File: IllumMap.glsl
// Desc: Shaders for per-pixel lighting
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
[IllumMapVS]
#include "CommonVS.glsl"

uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;

varying   vec3 g_vNormalES;
varying   vec3 g_vViewVecES;
varying vec2 g_vTextureCoordinate;

uniform vec3     g_vLightPos;

void main()
{
    vec4 vPositionES = g_matModelView     * g_vPositionOS;
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;

    // Transform object-space normals to eye-space
    vec3 vNormalES = g_matNormal * g_vNormalOS;
    
    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    
    // Normalize per-pixel vectors	
    vec3 vNormal = normalize( vNormalES.xyz );
    vec3 vLight  = normalize( g_vLightPos );
    vec3 vView   = normalize( vec3(0.0,0.0,0.0) - vPositionES.xyz );
    vec3 vHalf   = normalize( vLight + vView );
	
    g_vTextureCoordinate.x = max( 0.0, dot( vNormal, vLight));			// N.L
    g_vTextureCoordinate.y = 1.0 - max( 0.0, dot( vNormal, vHalf ));	// N.H
    
}

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[IllumMapFS]
#include "CommonFS.glsl"

struct MATERIAL
{
    vec4  vAmbient;
    vec4  vDiffuse;
    vec4  vSpecular;
};

uniform MATERIAL g_Material;

uniform sampler2D IllumSampler;
varying vec2 g_vTextureCoordinate;

void main()
{
	vec4 NLNH  = texture2D( IllumSampler, g_vTextureCoordinate).rgba;

	float SelfShadow = 4.0 * NLNH.r;
	
    // Combine lighting with the material properties
    gl_FragColor.rgba  = vec4(0.15, 0.15, 0.15, 0.15) * g_Material.vAmbient.rgba;
    gl_FragColor.rgba += g_Material.vDiffuse.rgba * NLNH.r;
    gl_FragColor.rgb  += SelfShadow * vec3(0.15, 0.15, 0.15) * g_Material.vSpecular.rgb * pow( NLNH.a , 20.0 );
}
