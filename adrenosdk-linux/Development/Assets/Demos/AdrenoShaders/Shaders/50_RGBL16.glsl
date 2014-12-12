//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[RGBtoL16FS]
#include "CommonFS.glsl"

uniform sampler2D g_Texture;
varying vec2 g_TexCoord;

vec4 ConvertRGBtoL16( vec4 Value )
{
//    float4 rgba = tex2D(Image, texCoord);
//     
//    // RGB -> XYZ conversion
//    const float3x3 RGB2XYZ = {0.5141364, 0.3238786,  0.16036376,
//                              0.265068,  0.67023428, 0.06409157,
//                              0.0241188, 0.1228178,  0.84442666};                                
//    float3 XYZ = mul(RGB2XYZ, rgba.rgb); 
//    
//    // u and v convert to Luv
//    // u' = 4*X/(X + 15*Y + 3*Z)
//    // v' = 9*Y/(X + 15*Y + 3*Z)
//    float2 uv = float2(4 * XYZ.x, 9 * XYZ.y) * (1 / (XYZ.x + 15 * XYZ.y + 3 * XYZ.z));
//    
//    float fhi;
//    modf(XYZ.y / 8.0 * 255.0, fhi);
//    fhi /= 255.0f;
//    float flow = (XYZ.y / 8.0 - fhi) *255.0f;  
//    
// 
//    // L lower range | u | v | L higher range
//    return float4(flow, uv.x, uv.y, fhi);

    // RGB -> XYZ conversion
    mat3 RGB2XYZ;
    RGB2XYZ[0] = vec3( 0.5141364, 0.3238786,  0.16036376 );
    RGB2XYZ[1] = vec3( 0.265068,  0.67023428, 0.06409157 );
    RGB2XYZ[2] = vec3( 0.0241188, 0.1228178,  0.84442666 );
    vec3 XYZ = Value.rgb * RGB2XYZ;

    // u and v convert to Luv
    // u' = 4*X/(X + 15*Y + 3*Z)
    // v' = 9*Y/(X + 15*Y + 3*Z)
    vec2 uv = vec2( 4.0 * XYZ.x, 9.0 * XYZ.y ) * ( 1.0 / ( XYZ.x + 15.0 * XYZ.y + 3.0 * XYZ.z ) );

    float fhi;
    fhi = floor( XYZ.y / 8.0 * 255.0 );
    fhi /= 255.0;
    float flow = ( XYZ.y / 8.0 - fhi ) * 255.0;

    // L lower range | u | v | L higher range
    return vec4( flow, uv.x, uv.y, fhi );
}

void main()
{
    vec4 Color = texture2D( g_Texture, g_TexCoord );
    gl_FragColor = ConvertRGBtoL16( Color );
}

