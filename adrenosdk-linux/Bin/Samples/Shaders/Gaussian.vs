

//--------------------------------------------------------------------------------------
// File: Gaussian.glsl
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



uniform vec2 g_vStepSize;

attribute vec4 g_vVertex;

varying vec2 g_vTexCoordM;
varying vec2 g_vTexCoordB0;
varying vec2 g_vTexCoordF0;
varying vec2 g_vTexCoordB1;
varying vec2 g_vTexCoordF1;
varying vec2 g_vTexCoordB2;
varying vec2 g_vTexCoordF2;

void main()
{
    gl_Position  = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );

    g_vTexCoordM  = vec2( g_vVertex.z, g_vVertex.w );
    g_vTexCoordB0 = g_vTexCoordM - g_vStepSize;
    g_vTexCoordF0 = g_vTexCoordM + g_vStepSize;
    g_vTexCoordB1 = g_vTexCoordM - g_vStepSize * 2.0;
    g_vTexCoordF1 = g_vTexCoordM + g_vStepSize * 2.0;
    g_vTexCoordB2 = g_vTexCoordM - g_vStepSize * 3.0;
    g_vTexCoordF2 = g_vTexCoordM + g_vStepSize * 3.0;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
