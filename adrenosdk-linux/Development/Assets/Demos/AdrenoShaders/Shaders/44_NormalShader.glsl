//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[NormalShader44VS]
#include "CommonVS.glsl"

attribute vec4 g_PositionIn;
attribute vec3 g_NormalIn;

uniform mat4 g_matModelViewProj;
uniform mat4 g_matModelView;

// outputs
varying vec4 g_ViewNormal;


void main()
{
    // Calculate screen position
    gl_Position = g_matModelViewProj * g_PositionIn;

    // Transform normals to view space
    g_ViewNormal = g_matModelView * vec4( g_NormalIn, 0.0 );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[NormalShader44FS]
#include "CommonFS.glsl"

// inputs from the vs
varying vec4 g_ViewNormal;


void main()
{
    // Put view normal in color space
    vec3 ViewNormal = normalize( g_ViewNormal.xyz ) * vec3( 0.5 ) + vec3( 0.5 );

    gl_FragColor = vec4( ViewNormal, 0.0 );
}
