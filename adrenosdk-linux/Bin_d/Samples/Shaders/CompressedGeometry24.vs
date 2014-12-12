
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



uniform   mat4 g_matModelView;
uniform   mat4 g_matCT;// compression transform (actually it's inverse)

uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;

in uvec3 g_vPositionOS;
//in vec4 g_vNormalOS;

out vec3 g_vNormalES;
out vec3 g_vViewVecES;

void main()
{	
//			[ 8 bits ][ 8 bits ][ 8 bits ][ 8 bits ]
// p.x   =  [ n.x    ][ lx     ][ ly     ][ lz     ]
// p.y   =  [ n.y    ][ mx     ][ my     ][ mz     ]
// p.z   =  [ n.z    ][ hx     ][ hy     ][ hz     ]

	uint uposX = g_vPositionOS.x;
	uint uposY = g_vPositionOS.y;
	uint uposZ = g_vPositionOS.z;
	
	uint ix = ((uposX & 0x00FF0000U) >> 16U) | ((uposY & 0x00FF0000U) >> 8U) | (uposZ & 0x00FF0000U);
	uint iy = ((uposX & 0x0000FF00U) >> 8U) | (uposY & 0x0000FF00U) | ((uposZ & 0x0000FF00U) << 8U);
	uint iz = (uposX & 0x000000FFU) | ((uposY & 0x000000FFU) << 8U) | ((uposZ & 0x000000FFU) << 16U);
	
	uint nx = (uposX & 0xFF000000U) >> 24U;
	uint ny = (uposY & 0xFF000000U) >> 24U;
	uint nz = (uposZ & 0xFF000000U) >> 24U;
	
	vec4 vPos = vec4(ix,
					 iy, 
					 iz, 8388607.0)/8388607.0;//8388687.0 ;
					 
    vec3 vNormal = (vec3( nx,
					  ny,
					  nz) - 128.0) / 127.0;

	vPos = g_matCT * vPos;
    vec4 vPositionES = g_matModelView     * vPos;
    vec4 vPositionCS = g_matModelViewProj * vPos;

    // Transform object-space normals to eye-space
						 
    vec3 vNormalES = g_matNormal * vNormal;//vNormal;

    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    g_vNormalES  = vNormalES;
    g_vViewVecES = vec3(0.0,0.0,0.0) - vPositionES.xyz;
}

