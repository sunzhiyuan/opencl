

//--------------------------------------------------------------------------------------
// File: PerVertexLighting.glsl
// Desc: Shaders for per-vertex lighting
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



struct MATERIAL
{
    vec4  vAmbient;
    vec4  vDiffuse;
    vec4  vSpecular;
};

uniform   mat4     g_matModelView;
uniform   mat4     g_matModelViewProj;
uniform   mat3     g_matNormal;
uniform   vec3     g_vLightPos;
uniform   MATERIAL g_Material;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;

varying   vec4     g_vColor;


void main()
{
    vec4 vPositionES = g_matModelView     * g_vPositionOS;
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;

    // Output clip-space position
    gl_Position = vPositionCS;

    // Transform object-space normals to eye-space
    vec3 vNormal = normalize( g_matNormal * g_vNormalOS );

    // Compute intermediate vectors
    vec3 vLight  = normalize( g_vLightPos );
    vec3 vView   = normalize( vec3(0.0,0.0,0.0) - vPositionES.xyz );
    vec3 vHalf   = normalize( vLight + vView );

    // Compute the lighting in eye-space
    float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );
    float fSpecular = pow( max( 0.0, dot( vNormal, vHalf ) ), g_Material.vSpecular.a );

    // Combine lighting with the material properties
    g_vColor.rgba  = g_Material.vAmbient.rgba;
    g_vColor.rgba += g_Material.vDiffuse.rgba * fDiffuse;
    g_vColor.rgb  += g_Material.vSpecular.rgb * fSpecular;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
