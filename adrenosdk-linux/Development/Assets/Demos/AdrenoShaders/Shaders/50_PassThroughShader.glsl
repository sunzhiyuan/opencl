//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// The vertex shader.  This vertex program will be used by all the other shaders.
//--------------------------------------------------------------------------------------
[PassThroughVS]
#include "CommonVS.glsl"

attribute vec4 g_PositionIn;
varying vec2 g_TexCoord;

void main()
{
    gl_Position  = vec4( g_PositionIn.x, g_PositionIn.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_PositionIn.z, g_PositionIn.w );
}
