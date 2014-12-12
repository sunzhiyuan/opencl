

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
