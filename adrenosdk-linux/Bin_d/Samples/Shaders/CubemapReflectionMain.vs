//--------------------------------------------------------------------------------------
// File: CubemapReflection.glsl
// Desc: Reflection shaders for the CubemapReflection sample
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



uniform   mat4 g_matModelViewProj;
uniform   mat4 g_matModelView;
uniform   mat3 g_matNormal;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;
attribute vec2 g_vTexcoordIN;

varying   vec3 g_vNormalES;
varying   vec3 g_vViewVecES;
varying   vec3 g_vTexcoord;

void main()
{
    // Compute the eye space position
    vec4 vPositionES = g_matModelView * g_vPositionOS;

    // Compute the clip space position
    gl_Position = g_matModelViewProj * g_vPositionOS;
    
    // Transform object-space normals to eye-space
    g_vNormalES  = g_matNormal * g_vNormalOS;
    
    // Pass the view vector to the fragment shader
    g_vViewVecES = -vPositionES.xyz;
    
    // Pass the texture coordinates to the fragment shader
    g_vTexcoord = reflect( normalize( vPositionES.xyz/vPositionES.w ), g_vNormalES );
}


//--------------------------------------------------------------------------------------
// The main fragment shader
//--------------------------------------------------------------------------------------
