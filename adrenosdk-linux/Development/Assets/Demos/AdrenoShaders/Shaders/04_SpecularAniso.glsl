//--------------------------------------------------------------------------------------
// File: SpecularAniso.glsl
// Desc: Shaders for anisotropic specular lighting
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
[SpecularAnisoVS]
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
    
    vec3 vLight  = normalize( g_vLightPos );
    vec3 vView   = normalize( (vec3(0.0,0.0,0.0) - vPositionES.xyz) );
    vec3 vHalf   = normalize( vLight + vView );
    
    g_vTextureCoordinate.x = max(dot(vLight, vNormalES.xyz), 0.0);
    g_vTextureCoordinate.y = max(dot(vHalf, vNormalES.xyz), 0.0);    
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[SpecularAnisoFS]
#include "CommonFS.glsl"

varying vec2 g_vTextureCoordinate;
uniform sampler2D AnisoTextureSampler;

void main()
{
	// fetch the look-up texture with the anisotropic specular model
	vec4 Texture  = texture2D( AnisoTextureSampler, g_vTextureCoordinate ).rgba;
	
	gl_FragColor.rgb  = Texture.rgb * Texture.aaa;
	gl_FragColor.a  = 1.0;
}
