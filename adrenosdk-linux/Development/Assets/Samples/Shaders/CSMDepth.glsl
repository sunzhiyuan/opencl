
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------
// The vertex shader for rendering to the shadow map
//--------------------------------------------------------------------------------------
[CSMDepthVS]
#include "CommonVS.glsl"

uniform   mat4 g_matModelViewProj;

attribute vec4 g_vVertex;

void main()
{
    gl_Position = g_matModelViewProj * g_vVertex;
}


//--------------------------------------------------------------------------------------
// The fragment shader for rendering to the shadow map
//--------------------------------------------------------------------------------------
[CSMDepthFS]
#include "CommonFS.glsl"

void main()
{
	//Last channel only used by VSM (depth^2)
	gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 0.0);
}
