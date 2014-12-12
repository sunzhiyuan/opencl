
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// File: CompressedGeometry.glsl
// Desc: Simple shader demonstrating geometry compression -an OpenglES3.0 port of 
//  Dean Calver's Demo:"Using Vertex Shaders For Geometry Compression" in Shaders X2
//--------------------------------------------------------------------------------------

[CompressedGeometryF16VS]

#version 300 es

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------

#include "CommonVS.glsl"

uniform   mat4 g_matModelView;
uniform   mat4 g_matCT;// compression transform (actually it's inverse)

uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;

in vec3 g_vPositionOS;
in vec3 g_vNormalOS;

out   vec3 g_vNormalES;
out   vec3 g_vViewVecES;


void main()
{
	vec4 vPos = vec4(g_vPositionOS* 1./65535., 1.0);//unmap from short range
	vPos = g_matCT * vPos;//unmap from (0,1)
	
    vec4 vPositionES = g_matModelView     * vPos;
    vec4 vPositionCS = g_matModelViewProj * vPos;

	// Unpack normal
	vec3 vNormal = (g_vNormalOS - 128.0)/127.0;
	
    // Transform object-space normals to eye-space
    vec3 vNormalES = g_matNormal * vNormal;

    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    g_vNormalES  = vNormalES;
    g_vViewVecES = vec3(0.0,0.0,0.0) - vPositionES.xyz;
}

[CompressedGeometryF16FS]

#version 300 es

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

#include "CommonFS.glsl"



struct MATERIAL
{
    vec4  vAmbient;
    vec4  vDiffuse;
    vec4  vSpecular;
};

uniform MATERIAL g_Material;
uniform vec3     g_vLightPos;

out vec4 g_FragColor;

in vec3     g_vViewVecES;
in vec3     g_vNormalES;

void main()
{
    // Normalize per-pixel vectors	
    vec3 vNormal = normalize( g_vNormalES );
    vec3 vLight  = normalize( g_vLightPos );
    vec3 vView   = normalize( g_vViewVecES );
    vec3 vHalf   = normalize( vLight + vView );

    // Compute the lighting in eye-space
    float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );
    float fSpecular = pow( max( 0.0, dot( vNormal, vHalf ) ), g_Material.vSpecular.a );

    // Combine lighting with the material properties
    g_FragColor.rgba  = g_Material.vAmbient.rgba;
    g_FragColor.rgba += g_Material.vDiffuse.rgba * fDiffuse;
    g_FragColor.rgb  += g_Material.vSpecular.rgb * fSpecular;
}
