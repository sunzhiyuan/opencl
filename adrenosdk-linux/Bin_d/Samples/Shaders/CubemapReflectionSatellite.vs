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
// The satellite vertex shader
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



const vec3  g_vAmbient   = vec3(0.3, 0.3, 0.3);
const vec3  g_vDiffuse   = vec3(0.8, 0.8, 0.8);
const vec3  g_vSpecular  = vec3(1.0, 1.0, 1.0);
const float g_fShininess = 1.0;

uniform   mat4 g_matModelViewProj;
uniform   mat4 g_matModelView;
uniform   mat3 g_matNormal;
uniform   vec3 g_vLightPos;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;
attribute vec2 g_vTexcoordIN;

varying   vec4 g_vOutColor;
varying   vec2 g_vTexcoord;

void main()
{
    // Output the clip space position
    gl_Position = g_matModelViewProj * g_vPositionOS;

    // Calculate the eye space position for lighting
    vec4 vPositionES = g_matModelView * g_vPositionOS;
    
    // Transform object-space normals to eye-space
    vec3 vNormal   = normalize( g_matNormal * g_vNormalOS );
    
    vec3 vLightVec = normalize( g_vLightPos.xyz - vec3( gl_Position.xyz / gl_Position.w ) );
    vec3 vViewVec  = -normalize( gl_Position.xyz );

    // Compute intermediate vectors
    vec3 vLight  = normalize( g_vLightPos );
    vec3 vView   = normalize( vec3(0.0,0.0,0.0) - vPositionES.xyz );
    vec3 vHalf   = normalize( vLight + vView );
    
    // Compute the lighting in eye-space
    float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );
    float fSpecular = pow( max( 0.0, dot( vNormal, vHalf ) ), g_fShininess );
    
    // Combine lighting with the material properties
    g_vOutColor.rgb  = g_vAmbient + g_vDiffuse*fDiffuse + g_vSpecular*fSpecular;
    g_vOutColor.a    = 1.0;
    
    g_vTexcoord = g_vTexcoordIN;
}


//--------------------------------------------------------------------------------------
// The satellite fragment shader
//--------------------------------------------------------------------------------------
