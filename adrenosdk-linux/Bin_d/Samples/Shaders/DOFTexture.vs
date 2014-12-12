//--------------------------------------------------------------------------------------
// File: Depth Of Field.glsl
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
//--------------------------------------------------------------------------------------
// File: CommonVS.glsl
// Desc: Useful common shader code for vertex shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;

attribute vec4 g_vPositionOS;
attribute vec2 g_vTexCoord;

varying float g_fOutViewLength;
varying vec2  g_vOutTexCoord;


void main()
{
    gl_Position      = g_matModelViewProj * g_vPositionOS;
    vec4 vPositionES = g_matModelView * g_vPositionOS;
    g_fOutViewLength = length( -vPositionES.xyz );
    g_vOutTexCoord   = g_vTexCoord;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
