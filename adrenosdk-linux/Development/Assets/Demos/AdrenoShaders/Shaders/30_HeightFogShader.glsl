//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[HeightFogShaderVS]
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
[HeightFogShaderFS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform sampler2D g_DepthTexture;
uniform mat4 g_MatModelViewProjInv;
uniform vec3 g_CamPos;
uniform vec4 g_FogColor;
uniform vec3 g_FogParams;


void main()
{
    vec4 Color = texture2D( g_ColorTexture, g_TexCoord );
    vec4 Depth = texture2D( g_DepthTexture, g_TexCoord );

    float FogTop = g_FogParams.x;
    float FogBottom = g_FogParams.y;
    float FogDensity = g_FogParams.z;

    // recalculate world position of this pixel
    vec4 ScreenPos = vec4( g_TexCoord.x * 2.0 - 1.0, g_TexCoord.y * 2.0 - 1.0, Depth.r, 1.0 );
    vec4 WorldPos = ScreenPos * g_MatModelViewProjInv;
    WorldPos.xyz /= WorldPos.w;

    // calculate fog amount based on world y and whether the camera is itself in the fog
    float FogAmount = smoothstep( FogTop, FogBottom, WorldPos.y );
    float CameraFog = smoothstep( FogTop, FogBottom, g_CamPos.y );
    gl_FragColor = mix( Color, g_FogColor, max( FogAmount, CameraFog ) * FogDensity );
}
