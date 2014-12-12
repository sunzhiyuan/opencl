//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[L16toRGBFS]
#include "CommonFS.glsl"

uniform sampler2D g_L16Texture;
varying vec2 g_TexCoord;

vec4 ConvertL16toRGB( vec4 Value )
{
//    // x = 9u' / (6u'-16v'+12)
//    // y = 4v' / (6u'-16v'+12)
//    float2 xy = float2(9 * Luv.y,4 * Luv.z) * 1.0 / (6 * Luv.y - 16 * Luv.z + 12);
//    
//    float Ld = Luv.w * 8.0 + Luv.x * (8.0 / 255.0);
//    
//    // Yxy -> XYZ conversion
//    float3 XYZ = float3(Ld * xy.x/ xy.y, Ld, Ld * (1 - xy.x - xy.y) / xy.y);
//  
//    // XYZ -> RGB conversion
//    const float3x3 XYZ2RGB = { 2.5651,-1.1665,-0.3986,
//                               -1.0217, 1.9777, 0.0439, 
//                                0.0753, -0.2543, 1.1892};
//                         
//   return float4(mul(XYZ2RGB, XYZ), 1.0);

    vec4 Luv = Value;

    // x = 9u' / (6u'-16v'+12)
    // y = 4v' / (6u'-16v'+12)
    vec2 xy = vec2( 9.0 * Luv.y, 4.0 * Luv.z ) * 1.0 / ( 6.0 * Luv.y - 16.0 * Luv.z + 12.0 );

    float Ld = Luv.w * 8.0 + Luv.x * ( 8.0 / 255.0 );

    // Yxy -> XYZ conversion
    vec3 XYZ = vec3( Ld * xy.x / xy.y, Ld, Ld * ( 1.0 - xy.x - xy.y ) / xy.y );

    // XYZ -> RGB conversion
    mat3 XYZ2RGB;
    XYZ2RGB[0] = vec3(  2.5651, -1.1665, -0.3986 );
    XYZ2RGB[1] = vec3( -1.0217,  1.9777,  0.0439 );
    XYZ2RGB[2] = vec3(  0.0753, -0.2543,  1.1892 );
    vec3 RGB = XYZ * XYZ2RGB;

    return vec4( RGB, 1.0 );
}

void main()
{
    vec4 Color = texture2D( g_L16Texture, g_TexCoord );
    gl_FragColor = ConvertL16toRGB( Color );
}
