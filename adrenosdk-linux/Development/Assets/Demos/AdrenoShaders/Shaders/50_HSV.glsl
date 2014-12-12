//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[HSVtoRGBFS]
#include "CommonFS.glsl"

uniform sampler2D g_HSVTexture;
uniform vec2 g_HSVParams;
varying vec2 g_TexCoord;


// emulates the HLSL function frac
float frac( float Val )
{
    return ( Val - floor( Val ) );
}


vec4 ConvertHSVtoRGB( vec4 Value )
{
    const float PI2 = 6.283185307179586476925286766559;
    const float RT3 = 1.0 / sqrt( 3.0 );

    const float HueScale = 1.0;
          float HueBias = g_HSVParams.x;
          float SaturationScale = g_HSVParams.y;
    const float SaturationBias = 0.0;
    const float IntensityScale = 1.0;
    const float IntensityBias = 0.0;

    // Scale and bias the components
    float H = frac    ( Value.r * HueScale        + HueBias );
    float S = saturate( Value.g * SaturationScale + SaturationBias );
    float I = saturate( Value.b * IntensityScale  + IntensityBias );

    float H3 = H * 3.0;
    float X = ( 2.0 * floor( H3 ) + 1.0 ) / 6.0;

    // Extract RGB values
    float R = ( 1.0 - S ) * I;
    float H2 = RT3 * tan( ( H - X ) * PI2 );
    float B = ( ( 3.0 + 3.0 * H2 ) * I - ( 1.0 + 3.0 * H2 ) * R ) / 2.0;
    float G = 3.0 * I - B - R;

    // Put them in the right order
    vec3 RGB;
    if( H3 < 1.0 )
    {
        RGB = vec3( G, B, R );
    }
    else if( H3 < 2.0 )
    {
        RGB = vec3( R, G, B );
    }
    else
    {
        RGB = vec3( B, R, G );
    }

    return vec4( RGB, 1.0 );
}

void main()
{
    vec4 Color = texture2D( g_HSVTexture, g_TexCoord );
    gl_FragColor = ConvertHSVtoRGB( Color );
}
