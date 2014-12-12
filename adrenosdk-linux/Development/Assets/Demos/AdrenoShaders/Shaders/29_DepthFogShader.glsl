//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[DepthFogShaderVS]
#include "CommonVS.glsl"

attribute vec4 g_Vertex;

varying vec2 g_TexCoord;


void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_Vertex.z, g_Vertex.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[DepthFogShaderFS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform sampler2D g_DepthTexture;
uniform vec2 g_NearQ;
uniform vec4 g_FogColor;
uniform vec3 g_FogParams;


void main()
{
    vec4 Color = texture2D( g_ColorTexture, g_TexCoord );
    vec4 Depth = texture2D( g_DepthTexture, g_TexCoord );

    float FogStart = g_FogParams.x;
    float FogEnd = g_FogParams.x + g_FogParams.y;
    float FogDensity = g_FogParams.z;

    // Q = Zf / Zf - Zn
    // z = -Zn * Q / Zd - Q
    float Zd = Depth.r;
    float Zn = g_NearQ.x;
    float Q = g_NearQ.y;
    float WorldDepth = -( Zn * Q ) / ( Zd - Q );

    // calculate fog amount based on pixel depth
    float Lerpval = smoothstep( FogStart, FogEnd, WorldDepth ) * FogDensity;
    gl_FragColor = mix( Color, g_FogColor, Lerpval );
}
