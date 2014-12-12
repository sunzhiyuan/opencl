//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[RGBtoCIEFS]
#include "CommonFS.glsl"

uniform sampler2D g_Texture;
varying vec2 g_TexCoord;

vec4 ConvertRGBtoCIE( vec4 Value )
{
    // RGB -> XYZ conversion
    mat3 RGB2XYZ;
    RGB2XYZ[0] = vec3( 0.5141364, 0.3238786,  0.16036376 );
    RGB2XYZ[1] = vec3( 0.265068,  0.67023428, 0.06409157 );
    RGB2XYZ[2] = vec3( 0.0241188, 0.1228178,  0.84442666 );
    vec3 XYZ = Value.rgb * RGB2XYZ;

    // XYZ -> Yxy conversion
    vec3 Yxy;
    Yxy.r = XYZ.g;  // copy luminance Y

    // prevent division by zero to keep the compiler happy
    float temp = max( dot( vec3( 1.0, 1.0, 1.0 ), XYZ.rgb ), 0.000001 );
    Yxy.g = XYZ.r / temp;
    Yxy.b = XYZ.g / temp;

    return vec4( Yxy.r, Yxy.g, Yxy.b, 0.0 );
}

void main()
{
    vec4 Color = texture2D( g_Texture, g_TexCoord );
    gl_FragColor = ConvertRGBtoCIE( Color );
}

