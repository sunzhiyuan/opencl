//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[CombineShader45VS]
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
[CombineShader45FS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_AOTexture;
uniform sampler2D g_ColorTexture;

void main()
{
    vec4 Color = texture2D( g_ColorTexture, g_TexCoord );
    vec4 BounceAO = texture2D( g_AOTexture, g_TexCoord );

    // Combine colors with bounce light
    vec4 Ret = vec4( 0.0 );
    Ret.rgb = (Color.rgb * 0.5) + BounceAO.rgb;

    gl_FragColor = Ret;
}
