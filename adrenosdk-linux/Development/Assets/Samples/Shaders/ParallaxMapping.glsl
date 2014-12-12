

//--------------------------------------------------------------------------------------
// File: ParallaxMapping.glsl
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
[ParallaxMappingVS]
#include "CommonVS.glsl"

uniform mat4  g_matModelViewProj;
uniform mat4  g_matModelMatrix;
uniform mat3  g_matNormal;
uniform vec3  g_vViewPosition;
uniform vec3  g_vLightPosition;
uniform float g_vTexScale;

attribute vec4 g_vPositionOS;
attribute vec3 g_vTangentOS;
attribute vec3 g_vBinormalOS;
attribute vec3 g_vNormalOS;
attribute vec2 g_vTexCoord;

varying vec2 g_vOutTexCoord;
varying vec3 g_vOutViewTS;
varying vec3 g_vOutLightTS;
varying vec3 g_vOutNormalWS;
varying vec3 g_vOutViewWS;
varying vec3 g_vOutLightWS;

void main()
{
    gl_Position         = g_matModelViewProj * g_vPositionOS;
    vec4 vWorldPosition = g_matModelMatrix * g_vPositionOS;
    g_vOutTexCoord      = g_vTexCoord * g_vTexScale;

    // Transform to world space from object space
    vec3 vTangentWS  = g_matNormal * g_vTangentOS;
    vec3 vBinormalWS = g_matNormal * g_vBinormalOS;
    vec3 vNormalWS   = g_matNormal * g_vNormalOS;

    // Calculate our world space light and view vectors
    vec3 vViewWS  = g_vViewPosition - vWorldPosition.xyz;
	vec3 vLightWS = g_vLightPosition - vWorldPosition.xyz;

	// Transform vectors into tangent space
    g_vOutViewTS.x = dot( vViewWS, vTangentWS );
    g_vOutViewTS.y = dot( vViewWS, vBinormalWS );
    g_vOutViewTS.z = dot( vViewWS, vNormalWS );
    
    // Transform vectors into tangent space
    g_vOutLightTS.x = dot( vLightWS, vTangentWS );
    g_vOutLightTS.y = dot( vLightWS, vBinormalWS );
    g_vOutLightTS.z = dot( vLightWS, vNormalWS );	

    g_vOutViewWS   = vViewWS;
    g_vOutLightWS  = vLightWS;
    g_vOutNormalWS = vNormalWS;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[ParallaxMappingFS]
#include "CommonFS.glsl"

struct MATERIAL
{
    vec4  vAmbient;
    vec4  vDiffuse;
    vec4  vSpecular;
};

uniform MATERIAL  g_Material;
uniform sampler2D g_sBaseTexture;
uniform sampler2D g_sBumpTexture;
uniform float     g_fParallax;
uniform float     g_fScale;
uniform int       g_nMode;

varying vec2 g_vOutTexCoord;
varying vec3 g_vOutViewTS;
varying vec3 g_vOutLightTS;
varying vec3 g_vOutNormalWS;
varying vec3 g_vOutViewWS;
varying vec3 g_vOutLightWS;

#define TEXTURE_MAP           0
#define BUMP_MAP              1
#define PARALLAX_MAP          2
#define BUMP_AND_PARALLAX_MAP 3

void main()
{
    if( g_nMode == TEXTURE_MAP )
    {
        vec3 vViewWS  = normalize( g_vOutViewWS );
        vec3 vLightWS = normalize( g_vOutLightWS );
        vec3 vHalfWS  = normalize( vLightWS + vViewWS );

        float fDiffuse = clamp( dot( g_vOutNormalWS, vLightWS), 0.0, 1.0 );
        float fSpecular = pow( clamp( dot( g_vOutNormalWS, vHalfWS ), 0.0, 1.0 ), g_Material.vSpecular.a );

        vec4 vBaseTexture = texture2D( g_sBaseTexture, g_vOutTexCoord );

        gl_FragColor.rgba  = g_Material.vAmbient.rgba * vBaseTexture.rgba;
        gl_FragColor.rgba += g_Material.vDiffuse.rgab * vBaseTexture.rgba * vec4( vec3( fDiffuse ), 1.0 );
        gl_FragColor.rgb  += g_Material.vSpecular.rgb * fSpecular;
    }
    else if( g_nMode == BUMP_MAP )
    {
        vec3 vViewTS = normalize( g_vOutViewTS );

        vec4 vBaseTexture = texture2D( g_sBaseTexture, g_vOutTexCoord );

        vec3 vBumpTS = texture2D( g_sBumpTexture, g_vOutTexCoord ).xyz * 2.0 - 1.0;
        vBumpTS = normalize( vBumpTS );

        vec3 vLightTS = normalize( g_vOutLightTS );
        vec3 vHalfTS  = normalize( vLightTS + vViewTS );

        float fDiffuse = clamp( dot( vBumpTS, vLightTS ), 0.0, 1.0 );
        float fSpecular = pow( clamp( dot( vBumpTS, vHalfTS ), 0.0, 1.0 ), g_Material.vSpecular.a );
    
        gl_FragColor.rgba  = g_Material.vAmbient.rgba * vBaseTexture.rgba;
        gl_FragColor.rgba += g_Material.vDiffuse.rgab * vBaseTexture.rgba * vec4( vec3( fDiffuse ), 1.0 );
        gl_FragColor.rgb  += g_Material.vSpecular.rgb * fSpecular;
    }
    else if( g_nMode == PARALLAX_MAP )
    {
        vec3 vViewTS = normalize( g_vOutViewTS );

        float fHeight = texture2D( g_sBumpTexture, g_vOutTexCoord ).a;
        float fOffset = g_fParallax * ( g_fScale * fHeight - 0.5 * g_fScale );
        vec2 vParallaxTexCoord = g_vOutTexCoord + fOffset * vViewTS.xy;

        fHeight += texture2D( g_sBumpTexture, vParallaxTexCoord ).a;
        fOffset = g_fParallax * ( fHeight - 1.0 );
        vParallaxTexCoord = g_vOutTexCoord + fOffset * vViewTS.xy;

        vec4 vBaseTexture = texture2D( g_sBaseTexture, vParallaxTexCoord );

        vec3 vViewWS  = normalize( g_vOutViewWS );
        vec3 vLightWS = normalize( g_vOutLightWS );
        vec3 vHalfWS  = normalize( vLightWS + vViewWS );

        float fDiffuse = clamp( dot( g_vOutNormalWS, vLightWS), 0.0, 1.0 );
        float fSpecular = pow( clamp( dot( g_vOutNormalWS, vHalfWS ), 0.0, 1.0 ), g_Material.vSpecular.a );

        gl_FragColor.rgba  = g_Material.vAmbient.rgba * vBaseTexture.rgba;
        gl_FragColor.rgba += g_Material.vDiffuse.rgab * vBaseTexture.rgba * vec4( vec3( fDiffuse ), 1.0 );
        gl_FragColor.rgb  += g_Material.vSpecular.rgb * fSpecular;

    }
    else if( g_nMode == BUMP_AND_PARALLAX_MAP )
    {
        vec3 vViewTS = normalize( g_vOutViewTS );

        float fHeight = texture2D( g_sBumpTexture, g_vOutTexCoord ).a;
        float fOffset = g_fParallax * ( g_fScale * fHeight - 0.5 * g_fScale );
        vec2 vParallaxTexCoord = g_vOutTexCoord + fOffset * vViewTS.xy;

        fHeight += texture2D( g_sBumpTexture, vParallaxTexCoord ).a;
        fOffset = g_fParallax * ( fHeight - 1.0 );
        vParallaxTexCoord = g_vOutTexCoord + fOffset * vViewTS.xy;

        vec4 vBaseTexture = texture2D( g_sBaseTexture, vParallaxTexCoord );

        vec3 vBumpTS = texture2D( g_sBumpTexture, vParallaxTexCoord ).xyz * 2.0 - 1.0;
        vBumpTS = normalize( vBumpTS );

        vec3 vLightTS = normalize( g_vOutLightTS );
        vec3 vHalfTS  = normalize( vLightTS + vViewTS );

        float fDiffuse = clamp( dot( vBumpTS, vLightTS ), 0.0, 1.0 );
        float fSpecular = pow( clamp( dot( vBumpTS, vHalfTS ), 0.0, 1.0 ), g_Material.vSpecular.a );

        gl_FragColor.rgba  = g_Material.vAmbient.rgba * vBaseTexture.rgba;
        gl_FragColor.rgba += g_Material.vDiffuse.rgab * vBaseTexture.rgba * vec4( vec3( fDiffuse ), 1.0 );
        gl_FragColor.rgb  += g_Material.vSpecular.rgb * fSpecular;
    }
}
