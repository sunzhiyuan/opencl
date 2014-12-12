
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

#version 300 es
//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------

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

