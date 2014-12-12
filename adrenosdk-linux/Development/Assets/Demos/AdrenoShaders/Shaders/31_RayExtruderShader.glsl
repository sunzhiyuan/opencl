//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[RayExtruderShaderVS]
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
[RayExtruderShaderFS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_RayTexture;
uniform vec2 g_ScreenLightPos;
uniform vec3 g_RayParams;


void main()
{
    float Aspect = g_RayParams.x;
    float RayStart = g_RayParams.y;
    float RayLength = g_RayParams.z;
    vec2 ScreenPos = ( g_TexCoord * 2.0 - 1.0 ) * vec2( Aspect, 1.0 );
    vec2 RayVector = ( g_ScreenLightPos - ScreenPos );
    vec2 RayOffset = RayVector * RayStart;
    RayVector *= RayLength;

    vec4 BaseColor = texture2D( g_RayTexture, g_TexCoord );
    vec4 Color = BaseColor;
    Color += max( BaseColor, texture2D( g_RayTexture, g_TexCoord + RayOffset + RayVector * 0.33 ) );
    Color += max( BaseColor, texture2D( g_RayTexture, g_TexCoord + RayOffset + RayVector * 0.66 ) );
    Color += max( BaseColor, texture2D( g_RayTexture, g_TexCoord + RayOffset + RayVector * 1.0 ) );

    Color *= 0.25;

    gl_FragColor = Color;
}
