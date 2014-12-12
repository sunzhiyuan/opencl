
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

[CompressedGeometry101012VS]
#version 300 es
//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------

#include "CommonVS.glsl"

uniform   mat4 g_matModelView;
uniform   mat4 g_matCT;// compression transform (actually it's inverse)

uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;

in vec4 g_vPositionOS;
in vec4 g_vNormalOS;

out   vec3 g_vNormalES;
out   vec3 g_vViewVecES;


void main()
{
//2^11 = 2048

	float enc1 = mod(g_vPositionOS.w,2.);
	float extraZ = (1024.0*enc1)
	             + (2048.0*clamp(g_vPositionOS.w - 1., 0.0, 1.0));
	
	vec4 vPos = g_vPositionOS / 1023.;
	
	
	
	vPos.z = (g_vPositionOS.z + extraZ)/ 4095.;
	vPos.w = 1.;
	
	vPos = g_matCT * vPos;
    vec4 vPositionES = g_matModelView     * vPos;
    vec4 vPositionCS = g_matModelViewProj * vPos;

    // Transform object-space normals to eye-space
    vec3 vNormal = g_vNormalOS.xyz/511.0;
    vec3 vNormalES = g_matNormal * vNormal;//vNormal;

    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    g_vNormalES  = vNormalES;
    g_vViewVecES = vec3(0.0,0.0,0.0) - vPositionES.xyz;
}

[CompressedGeometry101012FS]
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
