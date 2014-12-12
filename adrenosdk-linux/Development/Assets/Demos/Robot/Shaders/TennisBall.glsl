//--------------------------------------------------------------------------------------
// Desc: Shaders for the Robot demo
//
// Adreno SDK
//
// Copyright (c) 2013 QUALCOMM Technologies, Inc.
// All Rights Reserved. 
// QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


[TennisBallVS]
#include "CommonVS.glsl"
//--------------------------------------------------------------------------------------
// External constants
//--------------------------------------------------------------------------------------
uniform vec3 g_vLightDir;
uniform vec4 g_vMaterialDiffuse;
uniform vec4 g_vMaterialSpecular;
uniform vec3 g_vMaterialAmbient;
uniform vec3 g_vMaterialEmissive;

// Matrix palette
uniform mat3 g_matNormal;
uniform mat4 g_matWorld;
uniform mat4 g_matView;
uniform mat4 g_matProj;


//--------------------------------------------------------------------------------------
// Vertex structures
//--------------------------------------------------------------------------------------
attribute vec4  In_Pos;
attribute vec3  In_Normal;
attribute vec2  In_Tex0;

varying	  vec3  Out_vNormal;
varying   vec4  Out_Diffuse;
varying   vec2  Out_Tex0;


//--------------------------------------------------------------------------------------
// Vertex shader
//--------------------------------------------------------------------------------------
void main()
{
    // Transform position into clip space
    vec4 vPositionOS = In_Pos.xyzw;
    vec4 vPositionWS = g_matWorld * vPositionOS;
    vec4 vPositionES = g_matView * vPositionWS;
    vec4 vPositionCS = g_matProj * vPositionES;
    gl_Position = vPositionCS;

    vec3 vNormalOS = In_Normal;
    vec3 vNormalWS = g_matNormal * vNormalOS;
    vec4 vNormalES = g_matView * vec4( vNormalWS.x, vNormalWS.y, vNormalWS.z, 0.0 );
//	vec3 vNormal = normalize( vNormalWS );
    vec3 vNormal = normalize( vNormalES.xyz );
    
    // Compute intermediate vectors
    vec3 vLight  = g_vLightDir;
    Out_vNormal = vNormal;

    // Compute the lighting in eye-space
    float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );

    // Combine lighting with the material properties
    Out_Diffuse.rgb  = g_vMaterialAmbient.rgb + g_vMaterialDiffuse.rgb * fDiffuse;
    Out_Diffuse.a    = g_vMaterialDiffuse.a;

    // Copy the input texture coordinate through
    Out_Tex0.x  = In_Tex0.x;
    Out_Tex0.y  = 1.0 - In_Tex0.y;
}


[TennisBallFS]
#include "CommonFS.glsl"
//--------------------------------------------------------------------------------------
// Fragment shader
//--------------------------------------------------------------------------------------
uniform vec3 g_vLightDir;
uniform vec4 g_vMaterialDiffuse;
uniform vec4 g_vMaterialSpecular;
uniform vec3 g_vMaterialAmbient;
uniform vec3 g_vMaterialEmissive;

uniform sampler2D BaseTexture;

varying	vec3  Out_vNormal;
varying vec4  Out_Diffuse;
varying vec2  Out_Tex0;

void main()
{
    vec4 vDiffuse  = Out_Diffuse;

    if( false )
    {
        vec3  vNormal = normalize( Out_vNormal );
        vec3  vLight  = g_vLightDir;

        // Compute the lighting in eye-space
        float fDiffuse  = max( 0.0, dot( vNormal, vLight ) );

        // Combine lighting with the material properties
        vDiffuse.rgb  = g_vMaterialAmbient.rgb + g_vMaterialDiffuse.rgb * fDiffuse;
        vDiffuse.a    = g_vMaterialDiffuse.a;
    }

    gl_FragColor.rgba  = vDiffuse.rgba * texture2D( BaseTexture, Out_Tex0 ).rgba;
}
