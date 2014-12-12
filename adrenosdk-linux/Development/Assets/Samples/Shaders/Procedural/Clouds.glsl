

//--------------------------------------------------------------------------------------
// File: Clouds.glsl
// Desc: Clouds shader
//
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[CloudsVS]
#include "../CommonVS.glsl"

uniform mat4 g_matModelViewProj;
uniform float g_fScale;

attribute vec4 g_vVertex;

varying vec3 g_vOutScaledPosition;

void main()
{
    gl_Position = g_matModelViewProj * g_vVertex;
    g_vOutScaledPosition = g_vVertex.xyz * g_fScale;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[CloudsFS]
#include "../CommonFS.glsl"

#extension GL_OES_texture3D : enable
uniform sampler3D g_sTurb;

uniform float g_fIntensity;
uniform vec3 g_vColor1;
uniform vec3 g_vColor2;

varying vec3 g_vOutScaledPosition;

void main()
{
    // Unisgned turbulence
    float fUSTurb = 0.5 * ( 1.0 + texture3D( g_sTurb, g_vOutScaledPosition ).x );
    fUSTurb *= g_fIntensity;

    vec3 vResultColor = mix( g_vColor1, g_vColor2, fUSTurb );

    gl_FragColor = vec4( vResultColor, 1.0 );
}


