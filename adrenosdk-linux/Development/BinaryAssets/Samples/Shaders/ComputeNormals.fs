//--------------------------------------------------------------------------------------
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//
// ComputeNormals
//

//--------------------------------------------------------------------------------------
// The vertex shader
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

uniform sampler2D heightMap;

in vec4 texcoord0;
in vec4 texcoord1;
in vec4 texcoord2;

layout(location = FRAG_COL0) out vec4 NormalOut;

#define BS 255.0
float decodeHeight( vec3 h )
{
    return dot( h, vec3( 1.0f, 1.0f/BS, 1.0f/BS/BS ) );
}

void main()
{
    float height;
    height = decodeHeight( texture( heightMap, texcoord0.xy ).xyz );
    
    // compute and store normal
    
    // edge length of height map grid
    float edgeLength = texcoord1.w;
    vec3 a,b;
    a.x = edgeLength;
    a.y = decodeHeight( texture( heightMap, texcoord1.xy ).xyz ) - height;
    a.z = 0.0;
    b.x = 0.0;
    b.y = decodeHeight( texture( heightMap, texcoord2.xy ).xyz ) - height;
    b.z = edgeLength;
    
    vec3 nrml = cross( b, a );
    nrml = normalize( nrml );
    nrml = vec3( -nrml.z, nrml.y, nrml.x );
    nrml = nrml * 0.5 + 0.5;
    
    // slope
    
    // height scale
    float heightScale = texcoord1.z;
    float maxSlope = max( abs( a.y ), abs( b.y ) ) * heightScale * 0.25;
    
    NormalOut = vec4( nrml.xyz, maxSlope );

}


