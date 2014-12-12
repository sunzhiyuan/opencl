//--------------------------------------------------------------------------------------
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// ComputeLayerLighting (only FS, uses VS from AddLayer shader)
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------

#version 300 es


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



#define FRAG_COL0	0

uniform sampler2D normalMap;
uniform sampler2D shadowTexture;
uniform sampler2D occlusionTexture;
uniform sampler2D colorMap;

in vec4 texcoord0;
in vec4 texcoord1;
in vec4 texcoord2;
in vec4 texcoord3;

layout(location = FRAG_COL0) out vec4 Color;

#define BS 255.0
float decodeHeight( vec3 h )
{
    return dot( h, vec3( 1.0f, 1.0f/BS, 1.0f/BS/BS ) );
}

vec3 encodeHeight( float h )
{
    vec3 r;

    h *= BS;

    r.x = floor( h );
    h  -= r.x;
    r.y = floor( h * BS );
    h  -= r.y / BS;
    r.z = floor( h * BS * BS );

    return r / BS;
}


vec3 computeLighting( vec4 color, vec4 normal )
{
    // Same used in RenderTerrain Shader
    vec4 sunDirection = vec4( -0.438, 0.509, 0.741, 0.0 );

    // Sun color
    vec3 light = vec3( 3.52, 3.42, 2.99 );
    
    light *= max( 0.0, dot( normal.xyz, sunDirection.zyx ) );

    // shadow information and ambient occlusion
    float	shadow = texture( shadowTexture, texcoord2.xy ).x;
    float	ambientOcclusion = texture( occlusionTexture, texcoord2.xy ).x * 2.0 - 0.7;
    
    // combine light * shadow + ambient light
    return	color.xyz * ( light * shadow + ambientOcclusion * vec3( 0.39, 0.47, 0.75 ) );
}


void main()
{
    vec4 normal	      = texture( normalMap, texcoord0.xy ) * 2.0 - 1.0;
    vec4 surfaceColor = texture( colorMap, texcoord0.xy );

    surfaceColor.xyz = computeLighting( surfaceColor, normal );
    
    // allow higher dynamic range by using alpha channel	
    float maxV = max( surfaceColor.x, max( surfaceColor.y, surfaceColor.z ) );
    maxV = max( 1.0f, maxV );
    surfaceColor.w = maxV / 4.0f;
    surfaceColor.xyz /= maxV;

    Color = surfaceColor;

}

