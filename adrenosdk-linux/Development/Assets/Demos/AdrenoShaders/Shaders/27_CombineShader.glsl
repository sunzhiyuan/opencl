//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[CombineShader27VS]
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
[CombineShader27FS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_SharpTexture;
uniform sampler2D g_BlurredTexture;
uniform sampler2D g_DepthTexture;
uniform vec2 g_NearQ;
uniform vec2 g_FocalDistRange;
uniform bool g_ThermalColors;


void main()
{
//    gl_FragColor = texture2D( g_SharpTexture, g_TexCoord );
    vec4 FocusPixel = texture2D( g_SharpTexture, g_TexCoord );
    vec4 BlurPixel = texture2D( g_BlurredTexture, g_TexCoord );
    vec4 Depth = texture2D( g_DepthTexture, g_TexCoord );

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

    gl_FragColor = mix( FocusPixel, BlurPixel, Lerpval );
}
