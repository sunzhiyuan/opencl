
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[CombineShader30VS]

#version 300 es

#include "CommonVS.glsl"

in vec4 g_Vertex;

out vec2 g_TexCoord;

void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_Vertex.z, g_Vertex.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[CombineShader30FS]

#version 300 es

#include "CommonFS.glsl"

in vec2 g_TexCoord;

out vec4 g_FragColor;

uniform sampler2D g_SharpTexture;
uniform sampler2D g_BlurredTexture;
uniform sampler2D g_DepthTexture;
uniform vec2 g_NearQ;
uniform vec2 g_FocalDistRange;
uniform bool g_ThermalColors;


void main()
{
//    g_FragColor = texture2D( g_SharpTexture, g_TexCoord );
	vec4 FocusPixel = texture( g_SharpTexture, g_TexCoord);
    vec4 BlurPixel = texture( g_BlurredTexture, g_TexCoord);
    vec4 Depth = texture( g_DepthTexture, g_TexCoord);


/*
    // to visualize the dof blend
    if( g_ThermalColors )
    {
        FocusPixel.gb = vec2( 0.0 );
        BlurPixel.rg = vec2( 0.0 );
    }
*/
    // Q = Zf / Zf - Zn
    // z = -Zn * Q / Zd - Q
    float Zd = Depth.r;
    float Zn = g_NearQ.x;
    float Q = g_NearQ.y;
    float WorldDepth = -( Zn * Q ) / ( Zd - Q );
    float Lerpval = min( 1.0, abs( g_FocalDistRange.x - WorldDepth ) * g_FocalDistRange.y );

    g_FragColor = mix( FocusPixel, BlurPixel, Lerpval );
}
