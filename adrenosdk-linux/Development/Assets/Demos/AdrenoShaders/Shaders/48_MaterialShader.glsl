//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[MaterialShader48VS]
#include "CommonVS.glsl"

uniform mat4  g_MatModelViewProj;

attribute vec4 g_PositionIn;
attribute vec2 g_TexCoordIn;

varying vec2 g_TexCoord;

void main()
{
    vec4 Position  = g_PositionIn;
    vec2 TexCoord  = g_TexCoordIn;

    gl_Position = g_MatModelViewProj * Position;
    g_TexCoord = TexCoord;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[MaterialShader48FS]
#include "CommonFS.glsl"

uniform sampler2D g_DiffuseTexture;

varying vec2 g_TexCoord;

void main()
{
    vec4 Color = texture2D( g_DiffuseTexture, g_TexCoord );
    
    gl_FragColor = Color;
}
