

//--------------------------------------------------------------------------------------
// File: VarianceShadowMap.glsl
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader for rendering to the shadow map
//--------------------------------------------------------------------------------------
[VarianceDepthVS]
#include "../CommonVS.glsl"

uniform   mat4 g_matModelViewProj;

attribute vec4 g_vVertex;

varying float g_fDepth;
varying float g_fDepthSquared;

void main()
{
    gl_Position     = g_matModelViewProj * g_vVertex;
    g_fDepth        = 0.5 + 0.5 * ( gl_Position.z / gl_Position.w );
    g_fDepthSquared = g_fDepth * g_fDepth;
}


//--------------------------------------------------------------------------------------
// The fragment shader for rendering to the shadow map
//--------------------------------------------------------------------------------------
[VarianceDepthFS]
#include "../CommonFS.glsl"

varying float g_fDepth;
varying float g_fDepthSquared;

void main()
{
    gl_FragColor = vec4( g_fDepth, g_fDepthSquared, 0.0, 1.0 );
}


//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[VarianceShadowMapVS]
#include "../CommonVS.glsl"

uniform mat4 g_matModelViewProj;
uniform mat4 g_matShadow;
uniform mat4 g_matScreenCoord;

attribute vec4 g_vVertex;

varying vec4 g_vOutShadowCoord;
varying vec2 g_vOutTexCoord;
varying vec4 g_vOutScreenCoord;

void main()
{
    g_vOutShadowCoord = g_matShadow * g_vVertex;

    g_vOutScreenCoord = g_matScreenCoord * g_vVertex;

    gl_Position = g_matModelViewProj * g_vVertex;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[VarianceShadowMapFS]
#include "../CommonFS.glsl"

uniform sampler2D g_sVarianceShadowMap;
uniform sampler2D g_sDiffuseSpecularMap;
uniform vec3 g_vAmbient;

varying vec4 g_vOutShadowCoord;
varying vec4 g_vOutScreenCoord;

void main()
{
    float fEx    = ( texture2DProj( g_sVarianceShadowMap, g_vOutShadowCoord ) ).r;
    float fDepth = g_vOutShadowCoord.z / g_vOutShadowCoord.w;
    
    vec4 vDiffuseAndSpecular = texture2DProj( g_sDiffuseSpecularMap, g_vOutScreenCoord );

    if( fDepth <= fEx + 0.001 )
    {
        gl_FragColor =  vec4( g_vAmbient, 0.0 ) + vDiffuseAndSpecular;
    }
    else
    {
        float fExSquared = ( texture2DProj( g_sVarianceShadowMap, g_vOutShadowCoord ) ).g;
        float fVariance  = abs( fExSquared - fEx * fEx );
        float fMD = fEx - fDepth + 0.001;
        float fPMax = fVariance / ( fVariance + fMD * fMD);

        fPMax = max( float( fMD > 0.0 ), fPMax);

//        fPMax = fPMax * fPMax * (3.0 - 2.0 * fPMax);

        gl_FragColor = vec4( g_vAmbient, 0.0 ) + fPMax * vDiffuseAndSpecular;
    }

/*
//Normal Shadow Map algorithm:

    float fLightDepth = ( texture2DProj( g_sVarianceShadowMap, g_vOutShadowCoord ) ).r;

    vec4 vShadowColor;
    if( g_vOutShadowCoord.z / g_vOutShadowCoord.w > fLightDepth )
    {
        vShadowColor = vec4( vec3( 0.0 ), 1.0 );
    }
    else
    {
        vShadowColor = vec4( vec3( 1.0 ), 1.0 );
    }

    vec4 vDiffuseAndSpecular = texture2DProj( g_sDiffuseSpecularMap, g_vOutScreenCoord );
    gl_FragColor = vec4( g_vAmbient, 0.0 ) + vShadowColor * vDiffuseAndSpecular;
*/
}

