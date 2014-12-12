

//--------------------------------------------------------------------------------------
// File: Checker.glsl
// Desc: Procedural checker shader
// Author:                 QUALCOMM, Adreno SDK
//
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// File: CommonFS.glsl
// Desc: Useful common shader code for fragment shaders
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

// default to medium precision
precision mediump float;

// OpenGL ES require that precision is defined for a fragment shader
// usage example: varying NEED_HIGHP vec2 vLargeTexCoord;
#ifdef GL_FRAGMENT_PRECISION_HIGH
   #define NEED_HIGHP highp
#else
   #define NEED_HIGHP mediump
#endif

// Enable supported extensions
#extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



uniform vec4  g_vColor1;
uniform vec4  g_vColor2;
uniform float g_fFrequency;

varying vec3 g_vPosition;

void main()
{
    float fXMod = mod( g_vPosition.x * g_fFrequency, 1.0 );
    float fYMod = mod( g_vPosition.y * g_fFrequency, 1.0 );
    float fZMod = mod( g_vPosition.z * g_fFrequency, 1.0 );

    float fValue = 1.0;
    if( fXMod < 0.5 ) fValue = 1.0 - fValue;
    if( fYMod < 0.5 ) fValue = 1.0 - fValue;
    if( fZMod < 0.5 ) fValue = 1.0 - fValue;

    gl_FragColor = lerp( g_vColor1, g_vColor2, fValue );
}
