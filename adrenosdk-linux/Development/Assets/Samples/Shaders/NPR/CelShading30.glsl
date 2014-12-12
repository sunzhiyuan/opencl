

//--------------------------------------------------------------------------------------
// File: CelShading.glsl
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
[CelShading30VS]

#version 300 es
#include "../CommonVS.glsl"

uniform mat3 g_matNormal;
uniform mat4 g_matModelViewProj;

#define ATTR_POSITION	0
#define ATTR_NORMAL		3
#define ATTR_TEXCOORD	6

layout(location = ATTR_POSITION) in vec4 g_vVertex;
layout(location = ATTR_NORMAL) in vec3 g_vNormal;
layout(location = ATTR_TEXCOORD) in vec2 g_vTexCoord;

out vec3 g_vOutNormal;
out vec2 g_vOutTexCoord;
out float g_fOutDepth;

void main()
{
    gl_Position  = g_matModelViewProj * g_vVertex;
    g_vOutNormal = g_matNormal * g_vNormal;
    g_vOutTexCoord = g_vTexCoord;
    
    g_fOutDepth  = gl_Position.z / gl_Position.w;
    
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[CelShading30FS]

#version 300 es
#include "../CommonFS.glsl"

uniform sampler2D g_sCelShadingTexture;
uniform sampler2D g_sTexture;
uniform vec3 g_vLightPos;
uniform vec3 g_vAmbient;

in vec3 g_vOutNormal;
in vec2 g_vOutTexCoord;
in float g_fOutDepth;

#define FRAG_COL0		0
#define FRAG_COL1		1

layout(location = FRAG_COL0) out vec4 Color;    
layout(location = FRAG_COL1) out vec4 Other;    

void main()
{
    float vAngle          = max( 0.0, dot( normalize( g_vOutNormal ), normalize( g_vLightPos ) ) );
    vec4 vCelShadingColor = texture( g_sCelShadingTexture, vec2( vAngle, 0.0 ) );
    vec4 vTexColor        = texture( g_sTexture, g_vOutTexCoord );
    

     Color = vCelShadingColor * ( vTexColor + vec4( g_vAmbient, 0.0 ) );
     Other = vec4( normalize( g_vOutNormal ), g_fOutDepth );
} 
