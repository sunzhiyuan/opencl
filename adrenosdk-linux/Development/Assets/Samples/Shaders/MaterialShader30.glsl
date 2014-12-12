

//--------------------------------------------------------------------------------------
// File: MaterialShader.glsl
// Desc: Shaders for per-pixel lighting
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
[MaterialShader30VS]

#version 300 es

#include "CommonVS.glsl"

uniform mat4  g_MatModelViewProj;
uniform mat4  g_MatModel;
uniform vec3  g_LightPos;
uniform vec3  g_EyePos;

in vec4 g_PositionIn;
in vec2 g_TexCoordIn;
in vec3 g_TangentIn;
in vec3 g_BinormalIn;
in vec3 g_NormalIn;

out vec2 g_TexCoord;
out vec4 g_LightVec;
out vec3 g_ViewVec;
out vec3 g_Normal;

void main()
{
    vec4 Position  = g_PositionIn;
    vec2 TexCoord  = g_TexCoordIn;
    vec3 Tangent   = g_TangentIn;
    vec3 Binormal  = g_BinormalIn;
    vec3 Normal    = g_NormalIn;

    gl_Position = g_MatModelViewProj * Position;
    g_TexCoord = TexCoord;
    g_Normal   = Normal;

    vec4 WorldPos = g_MatModel * Position;
    vec3 lightVec = g_LightPos - WorldPos.xyz;
    vec3 viewVec  = g_EyePos   - WorldPos.xyz;

    // Transform vectors into tangent space
    g_LightVec.x = dot( lightVec, Tangent);
    g_LightVec.y = dot( lightVec, Binormal);
    g_LightVec.z = dot( lightVec, Normal);
    g_LightVec.w = saturate( 1.0 - 0.03 * dot( g_LightVec.xyz, g_LightVec.xyz ) );
    g_LightVec.xyz = normalize(g_LightVec.xyz);

    g_ViewVec.x  = dot( viewVec, Tangent );
    g_ViewVec.y  = dot( viewVec, Binormal );
    g_ViewVec.z  = dot( viewVec, Normal );
    g_ViewVec = normalize(g_ViewVec);
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[MaterialShader30FS]

#version 300 es

#include "CommonFS.glsl"

uniform vec4 g_AmbientColor;
uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;

in vec2 g_TexCoord;
in vec4 g_LightVec;
in vec3 g_ViewVec;
in vec3 g_Normal;

out vec4 g_FragColor;

void main()
{    
	vec4 vBaseColor = texture( g_DiffuseTexture, g_TexCoord);       
		
    //     
    // get normal from 11:11 EAC compressed and re-construct z ...
    // 
    vec3 vNormal;    
	vNormal.xy = texture( g_BumpTexture, g_TexCoord).xy * 2.0 - 1.0;

    vNormal.z = sqrt(1.0 - dot(vNormal.xy, vNormal.xy));
    
    //vec3 vNormal = texture2D( g_BumpTexture, g_TexCoord).xyz * 2.0 - 1.0;

    // Standard Phong lighting
    vec3  vHalf     = normalize( g_LightVec.xyz + g_ViewVec );
    float fDiffuse  = saturate( dot( g_LightVec.xyz, vNormal ) );
    
    //float fSpecular = saturate( dot( vHalf, vNormal ) * dot( vHalf, vNormal ) * dot( vHalf, vNormal ));
    float fSpecular = pow( saturate( dot( vHalf, vNormal ) ), 20.0 );

	g_FragColor = saturate(vec4(g_AmbientColor.x * vBaseColor.x, g_AmbientColor.x * vBaseColor.x, g_AmbientColor.x * vBaseColor.x, 0.0) + vec4(fDiffuse * vBaseColor.x + fSpecular, fDiffuse * vBaseColor.y + fSpecular, fDiffuse * vBaseColor.z + fSpecular, 0.0) * g_LightVec.w);
}
