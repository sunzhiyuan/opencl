//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
// 
// //--------------------------------------------------------------------------------------
// // The vertex shader
// //--------------------------------------------------------------------------------------
// [MaterialShaderVS]
// #include "CommonVS.glsl"
// 
// uniform mat4  g_MatModelViewProj;
// uniform mat4  g_MatModel;
// uniform mat4  g_MatNormal;
// uniform vec3  g_LightPos;
// uniform vec3  g_EyePos;
// 
// attribute vec4 g_PositionIn;
// attribute vec2 g_TexCoordIn;
// attribute vec3 g_NormalIn;
// 
// varying vec2 g_TexCoord;
// varying vec3 g_LightVec;
// varying vec3 g_ViewVec;
// varying vec3 g_Normal;
// 
// void main()
// {
//     vec4 Position  = g_PositionIn;
//     vec2 TexCoord  = g_TexCoordIn;
//     vec3 Normal    = g_NormalIn;
// 
//     gl_Position = g_MatModelViewProj * Position;
//     g_Normal    = ( g_MatNormal * vec4( Normal, 1.0 ) ).xyz;
//     g_TexCoord  = TexCoord;
// 
//     vec4 WorldPos = g_MatModel * Position;
//     g_LightVec = g_LightPos - WorldPos.xyz;
//     g_ViewVec  = g_EyePos   - WorldPos.xyz;
// }



//--------------------------------------------------------------------------------------
// The vertex shader.
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



uniform sampler2D g_Texture;
uniform float g_Exposure;
varying vec2 g_TexCoord;

// Convert floating-point color to RGBE
// float2rgbe(unsigned char rgbe[4], float red, float green, float blue)
// {
//   float v;
//   int e;
// 
//   v = red;
//   if (green > v) v = green;
//   if (blue > v) v = blue;
//   if (v < 1e-32) {
//     rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
//   }
//   else {
//     v = frexp(v,&e) * 256.0/v;
//     rgbe[0] = (unsigned char) (red * v);
//     rgbe[1] = (unsigned char) (green * v);
//     rgbe[2] = (unsigned char) (blue * v);
//     rgbe[3] = (unsigned char) (e + 128);
//   }
// }
// 
// Convert RGBE to floating-point color
// rgbe2float(float *red, float *green, float *blue, unsigned char rgbe[4])
// {
//   float f;
// 
//   if (rgbe[3]) {   /*nonzero pixel*/
//     f = ldexp(1.0,rgbe[3]-(int)(128+8));         = 1.0 * 2 ^ (...)    = pow( 2.0, rgbe[3] - (128+8) )        = pow( 2.0, RGBE.a - 0.5 )
//     *red = rgbe[0] * f;
//     *green = rgbe[1] * f;
//     *blue = rgbe[2] * f;
//   }
//   else
//     *red = *green = *blue = 0.0;
// }

vec4 ConvertRGBEtoFloatingPoint( vec4 Value )
{
    // Alpha values get gamma corrected in openGL or by the texture packer.
    // Values of 127 turn into 104.  This only happens to the alpha channel,
    // not the other three.
    float MidPoint = ( 104.0 / 255.0 );

    float Brightness = pow( 2.0, ( Value.a - MidPoint ) * 10.0 );
    return vec4( Value.rgb * Brightness, 1.0 );
}

void main()
{
    vec4 Color = texture2D( g_Texture, g_TexCoord );
    gl_FragColor = ConvertRGBEtoFloatingPoint( Color ) * g_Exposure;
}
