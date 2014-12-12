//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[CombineShader14VS]
#include "CommonVS.glsl"

// input
attribute vec4 g_Vertex;

// vs variables
uniform float g_StepSize;

// output to the fs
varying vec2 g_TexCoords0;
varying vec2 g_TexCoords1;
varying vec2 g_TexCoords2;
varying vec2 g_TexCoords3;

void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );

    g_TexCoords0 = vec2( g_Vertex.z - g_StepSize, g_Vertex.w - g_StepSize );
    g_TexCoords1 = vec2( g_Vertex.z + g_StepSize, g_Vertex.w - g_StepSize );
    g_TexCoords2 = vec2( g_Vertex.z - g_StepSize, g_Vertex.w + g_StepSize );
    g_TexCoords3 = vec2( g_Vertex.z + g_StepSize, g_Vertex.w + g_StepSize );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[CombineShader14FS]
#include "CommonFS.glsl"

// input from the vs
varying vec2 g_TexCoords0;
varying vec2 g_TexCoords1;
varying vec2 g_TexCoords2;
varying vec2 g_TexCoords3;

// ps variables
uniform sampler2D g_NormalTexture;
uniform sampler2D g_ToonTexture;
uniform float g_SlopeBias;

void main()
{
    vec4 toonPixel = texture2D( g_ToonTexture, g_TexCoords0 );

    // Sample the target pixel and some neighbors
    vec4 normal0 = texture2D( g_NormalTexture, g_TexCoords0 );
    vec4 normal1 = texture2D( g_NormalTexture, g_TexCoords1 );
    vec4 normal2 = texture2D( g_NormalTexture, g_TexCoords2 );
    vec4 normal3 = texture2D( g_NormalTexture, g_TexCoords3 );

    // Calculate horizontal and vertical changes
    vec4 slope1 = ( ( normal0 + normal1 ) - ( normal2 + normal3 ) ) * g_SlopeBias;
    vec4 slope2 = ( ( normal0 + normal2 ) - ( normal1 + normal3 ) ) * g_SlopeBias;

    // threshold function
    vec4 x = slope1 * slope1 + slope2 * slope2;
    float y = 1.0 - length( x );
    if( y < 0.8 )
    {
        y = 0.0;
    }
    gl_FragColor = vec4( toonPixel.rgb * y, 1.0 );
}
