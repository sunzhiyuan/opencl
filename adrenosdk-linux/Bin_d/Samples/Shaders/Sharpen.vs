

//--------------------------------------------------------------------------------------
// File: Sharpen.glsl
// Desc: Sharpen shader for the ImageEffects sample
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



uniform float g_fStepSizeX;
uniform float g_fStepSizeY;

attribute vec4 g_vVertex;

varying vec2 g_vTexCoords11;
varying vec2 g_vTexCoords00;
varying vec2 g_vTexCoords02;
varying vec2 g_vTexCoords20;
varying vec2 g_vTexCoords22;

void main()
{
    gl_Position  = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );
    vec2 vTexCoords = vec2( g_vVertex.z, g_vVertex.w );

    g_vTexCoords11 = vTexCoords;
    g_vTexCoords00 = vTexCoords + vec2( -g_fStepSizeX, -g_fStepSizeY );
    g_vTexCoords02 = vTexCoords + vec2(  g_fStepSizeX, -g_fStepSizeY );
    g_vTexCoords20 = vTexCoords + vec2(  g_fStepSizeX,  g_fStepSizeY );
    g_vTexCoords22 = vTexCoords + vec2( -g_fStepSizeX,  g_fStepSizeY );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
