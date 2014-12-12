//--------------------------------------------------------------------------------------
// File: LightStreakShader.glsl
// Desc: Shaders for per-pixel lighting
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
// This no longer supported on Droid.
// #extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



varying vec2 g_TexCoord;

uniform sampler2D g_ColorTexture;
uniform vec2 g_StepSize;
uniform vec4 g_Weights;
uniform vec2 g_StreakDir;
uniform float g_StreakLength;

void main()
{
    vec4 Color = vec4( 0.0 );
    float Weight = g_Weights.x;

    // set up the SC variable
    // The streak vector and length are precomputed and sent down from the application level to the pixel shader.
    vec2 SC = g_StreakDir * vec2( g_StreakLength ) * g_StepSize;

    // get the center pixel
    Color = saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy );

    // sample outwards in both directions
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    // again
    SC += SC;
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    // again
    SC += SC;
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    // again
    SC += SC;
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    // again
    SC += SC;
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    // again
    SC += SC;
    Weight *= 0.5;
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy + SC );
    Color += saturate( Weight ) * texture2D( g_ColorTexture, g_TexCoord.xy - SC );

    gl_FragColor = saturate( Color );
}
