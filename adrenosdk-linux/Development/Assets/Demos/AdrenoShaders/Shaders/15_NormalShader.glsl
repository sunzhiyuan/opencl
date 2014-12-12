//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[NormalShader15VS]
#include "CommonVS.glsl"

uniform mat4 g_matModelView;
uniform mat4 g_matModelViewProj;
uniform mat3 g_matNormal;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;

// outputs
varying vec3 g_Normal;
varying float g_OutDepth;

void main()
{
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;
    gl_Position  = vPositionCS;
    g_OutDepth  = gl_Position.z / gl_Position.w;

    // transform object-space normals to eye-space
    g_Normal = g_matNormal * g_vNormalOS;
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[NormalShader15FS]
#include "CommonFS.glsl"

// inputs from the vs
varying vec3 g_Normal;
varying float g_OutDepth;

void main()
{
    gl_FragColor = vec4( normalize( g_Normal * vec3( 0.5 ) + vec3( 0.5 ) ), g_OutDepth );
    //gl_FragColor = vec4( g_vOutTexCoord.x, g_vOutTexCoord.y, 0.0, 1.0 );
}
