

//--------------------------------------------------------------------------------------
// File: NormalMapping.glsl
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
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
[NormalMappingVS]
#include "CommonVS.glsl"

uniform mat4  matModelViewProj;
uniform vec3  vLightPos;
uniform vec3  vCamPos;

attribute vec4 position;
attribute vec2 texCoord;
attribute vec3 tangent;
attribute vec3 binormal;
attribute vec3 normal;

varying vec2 g_vTexCoord;
varying vec3 g_vLightVec;
varying vec3 g_vViewVec;
varying vec3 g_vNormal;

void main()
{
    vec4 vPosition  = position;
    vec2 vTexCoord  = texCoord;
    vec3 vTangent   = tangent;
    vec3 vBinormal  = binormal;
    vec3 vNormal    = normal;

    gl_Position = matModelViewProj * vPosition;
    g_vTexCoord = vTexCoord;
    g_vNormal   = vNormal;

    vec3 lightVec = vLightPos - vPosition.xyz;
    vec3 viewVec  = vCamPos   - vPosition.xyz;

    // When using normal maps, transform vectors into tangent space
    g_vLightVec.x = dot( lightVec, vTangent);
    g_vLightVec.y = dot( lightVec, vBinormal);
    g_vLightVec.z = dot( lightVec, vNormal);

    g_vViewVec.x  = dot( viewVec, vTangent );
    g_vViewVec.y  = dot( viewVec, vBinormal );
    g_vViewVec.z  = dot( viewVec, vNormal );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[NormalMappingFS]
#include "CommonFS.glsl"

uniform float fAmbient;

uniform sampler2D BaseTexture;
uniform sampler2D BumpTexture;

varying vec2 g_vTexCoord;
varying vec3 g_vLightVec;
varying vec3 g_vViewVec;
varying vec3 g_vNormal;

void main()
{
    vec4 vBaseColor = texture2D( BaseTexture, g_vTexCoord );
    
    // Select the normal in the appropriate space
    // Note: the mip level is biased to sharpen the bumpmapping effect
    vec3 vNormal    = texture2D( BumpTexture, g_vTexCoord, -1.0 ).xyz * 2.0 - 1.0;

    // Standard Phong lighting
    float fAtten    = saturate( 1.0 - 0.05 * dot( g_vLightVec, g_vLightVec ) );
    vec3  vLight    = normalize( g_vLightVec );
    vec3  vView     = normalize( g_vViewVec );
    vec3  vHalf     = normalize( vLight + vView );
    float fDiffuse  = saturate( dot( vLight, vNormal ) );
    float fSpecular = pow( saturate( dot( vHalf, vNormal ) ), 64.0 );

    gl_FragColor = ( fDiffuse * fAtten + fAmbient ) * vBaseColor
                 + ( fSpecular * fAtten ) * 0.7;
}

