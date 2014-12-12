
//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------

[MultiSampleVS]

#version 310 es

uniform mat4 mvp;

layout(location = 0) in vec4 vertex;

void main()
{
   gl_Position = mvp * vertex;
}

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[MultiSampleFS]

#version 310 es

out vec4 result;

void main()
{
   result = vec4(1.0, 0.0, 0.0, 1.0);
}