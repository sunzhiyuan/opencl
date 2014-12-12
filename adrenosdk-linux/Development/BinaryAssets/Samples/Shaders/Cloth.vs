//--------------------------------------------------------------------------------------
// File: Cloth.glsl
// Desc: Shaders for ClothSimCLGLES sample
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

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



uniform   mat4 g_matModel;
uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;
attribute vec3 g_vTexCoord;
attribute vec3 g_vTangent;
attribute vec3 g_vBitangent;

varying   vec3 g_vViewVecES;
varying   vec2 g_vTexCoord0;
varying   vec3 g_vOutWorldPos;
varying   vec3 g_vOutNormal;
varying   vec3 g_vOutTangent;
varying   vec3 g_vOutBitangent;

void main()
{
    vec4 vPositionES = g_matModelView     * g_vPositionOS;
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;

    g_vTexCoord0 = g_vTexCoord.xy;

    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    g_vViewVecES = vec3(0.0,0.0,0.0) - vPositionES.xyz;
    
    g_vOutWorldPos = (g_matModel * vec4(g_vPositionOS.xyz, 1.0)).xyz;
	g_vOutNormal = (g_matModel * vec4(g_vNormalOS, 0.0)).xyz;
	g_vOutTangent = (g_matModel * vec4(g_vTangent.xyz, 0.0)).xyz;
	g_vOutBitangent = (g_matModel * vec4(g_vBitangent.xyz, 0.0)).xyz;

}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
