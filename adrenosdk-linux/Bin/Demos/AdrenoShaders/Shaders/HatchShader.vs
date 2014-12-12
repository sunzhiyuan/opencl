//--------------------------------------------------------------------------------------
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



// inputs
attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;
attribute vec2 g_vTexCoord;

// shader variables
uniform mat4 g_matModelView;
uniform mat4 g_matModelViewProj;
uniform mat3 g_matNormal;

// outputs to the fs
varying vec3 g_vNormalES;
varying vec4 g_vOutTexCoord;

void main()
{
    // Basic transforms
    vec4 vPositionES = g_matModelView     * g_vPositionOS;
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;
    vec3 vNormalES = g_matNormal * g_vNormalOS;

    // Generate hatch uv's procedurally
    vec2 HatchUV = vec2( g_vPositionOS.x * 12.1 + sin( g_vPositionOS.y * 19.34 ), g_vPositionOS.y * 9.3 + g_vPositionOS.z * 13.8 + cos( g_vPositionOS.x * 13.11 ) );

    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    g_vNormalES  = normalize(vNormalES.xyz);
    g_vOutTexCoord = vec4( g_vTexCoord.x, g_vTexCoord.y, HatchUV.x, HatchUV.y );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
