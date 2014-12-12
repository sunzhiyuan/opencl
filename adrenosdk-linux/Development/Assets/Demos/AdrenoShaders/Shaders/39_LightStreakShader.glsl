//--------------------------------------------------------------------------------------
// File: LightStreakShader.glsl
// Desc: Shaders for per-pixel lighting
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
[LightStreakShader39VS]
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
[LightStreakShader39FS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
uniform vec4 g_Weights;
uniform vec2 g_StreakDir;
uniform float g_StreakLength;

void main()
{
    vec4 Color = vec4( 0.0 );
    float Weight = g_Weights.x;

    // set up the SC variable
    // The streak vector and length are precomputed and sent down from the application level to the pixel shader.
    vec2 SC = g_StreakDir * vec2( g_StreakLength ) * g_StepSize;

    // get the center pixel
    Color = saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy );

    // sample outwards in both directions
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    // again
    SC += SC;
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    // again
    SC += SC;
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    // again
    SC += SC;
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    // again
    SC += SC;
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    // again
    SC += SC;
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    gl_FragColor = saturate( Color );
}
