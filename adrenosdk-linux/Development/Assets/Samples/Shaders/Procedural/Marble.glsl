

//--------------------------------------------------------------------------------------
// File: Wood.glsl
// Desc: Wood shader
//
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[MarbleVS]
#include "../CommonVS.glsl"

uniform mat4  g_matModelViewProj;
uniform mat4  g_matModelView;
uniform mat3  g_matNormal;
uniform float g_fScale;

attribute vec4 g_vVertex;
attribute vec3 g_vNormalES;

varying vec3 g_vOutPosition;
varying vec3 g_vOutScaledPosition;
varying vec3 g_vOutNormalES;
varying vec3 g_vOutViewVec;

void main()
{
    gl_Position = g_matModelViewProj * g_vVertex;

    g_vOutPosition       = g_vVertex.xyz;
    g_vOutScaledPosition = g_fScale * g_vVertex.xyz;
    g_vOutNormalES       = g_matNormal * g_vNormalES;
    g_vOutViewVec        = -vec3( g_matModelView * g_vVertex );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[MarbleFS]
#include "../CommonFS.glsl"

#extension GL_OES_texture3D : enable
uniform sampler3D g_sTurb;

uniform float g_Kd;
uniform float g_Ks;
uniform vec4  g_vColor1;
uniform vec4  g_vColor2;
uniform float g_fTurbScale;
uniform vec4  g_vLightDir;

varying vec3 g_vOutPosition;
varying vec3 g_vOutScaledPosition;
varying vec3 g_vOutNormalES;
varying vec3 g_vOutViewVec;

void main()
{
    // Signed turbulence
    float fSTurb = texture3D( g_sTurb, g_vOutScaledPosition ).x;

    float fLrp = sin( g_fTurbScale * fSTurb + g_vOutScaledPosition.x + g_vOutScaledPosition.y + g_vOutScaledPosition.z );
    vec4 fMarbleColor = mix( g_vColor1, g_vColor2, fLrp );

    vec3 vNormal = normalize( g_vOutNormalES );
    // Soft diffuse
    float fDiffuse = 0.5 + 0.5 * dot( g_vLightDir.xyz, vNormal );
    // Standard specular
    float fSpecular = pow( clamp( dot( reflect( -normalize( g_vOutViewVec ), vNormal ), g_vLightDir.xyz ), 0.0, 1.0 ), 12.0 );

    gl_FragColor = g_Kd * fDiffuse * fMarbleColor + g_Ks * fSpecular;
}


