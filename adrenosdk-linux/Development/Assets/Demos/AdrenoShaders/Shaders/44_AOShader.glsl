//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[AOShader44VS]
#include "CommonVS.glsl"

attribute vec4 g_Vertex;

varying vec2 g_TexCoord;


void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_Vertex.z, g_Vertex.w );
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[AOShader44FS]
#include "CommonFS.glsl"

varying vec2 g_TexCoord;

uniform sampler2D g_NormalTexture;
uniform sampler2D g_DepthTexture;
uniform sampler2D g_JitterTexture;
uniform mat4 g_ProjInv;
uniform vec2 g_StepSize;
uniform float g_AOEdgeFinder;
uniform float g_AOIntensity;
uniform float g_JitterAmount;


//------------------------------------------------------------------------------
// Returns the position of the pixel (in view space)
//
vec3 GetPixelPosition( vec2 TexCoord )
{
    // Reconstruct view position of this pixel
    float Depth = texture2D( g_DepthTexture, TexCoord ).r;

    //vec4 ScreenPos = vec4( TexCoord.x * 2.0 - 1.0, TexCoord.y * 2.0 - 1.0, Depth.r, 1.0 );
    vec4 ScreenPos = vec4( TexCoord * vec2( 2.0 ) - vec2( 1.0 ), Depth, 1.0 );

    vec4 ViewPos = ScreenPos * g_ProjInv;
    ViewPos.xyz /= ViewPos.w;

    return ViewPos.xyz;
}


//------------------------------------------------------------------------------
// Returns the normal of the pixel (in view space)
//
vec3 GetPixelNormal( vec2 TexCoord )
{
    vec4 Normal = texture2D( g_NormalTexture, g_TexCoord ) * 2.0 - 1.0;
    return Normal.xyz;
}


//------------------------------------------------------------------------------
// Calculates ambient occlusion term given for the pixel provided.  Relative
// to the other two input parameters, which come from the target pixel in the PS.
//
float AmbientOcclusionAt( vec2 NeighborUV, vec3 TargetPosition, vec3 TargetNormal )
{
    vec3  Delta = GetPixelPosition( NeighborUV ) - TargetPosition;
    vec3  N = GetPixelNormal( NeighborUV );
    vec3  V = normalize( Delta );
    float D = length( Delta ) * 0.1;
    float Epsilon = 0.5;

    float a0 = 1.0 - saturate( dot( N, -V ) - Epsilon );
    float a1 = saturate( dot( TargetNormal, V ) - Epsilon );
    float a2 = 1.0 - 1.0 / sqrt( 1.0 / ( D * D * g_AOEdgeFinder ) + 1.0 );

    return a0 * a1 * a2;
}


void main()
{
    #define NUM_ITERATIONS 4

    float SampleAngle = 0.0;

    // Accumulate AO in this
    float AO = 0.0;

    // For noise
    vec4 Noise = texture2D( g_JitterTexture, g_TexCoord.xy * vec2( 4.0 ) );

    // Grab these values for the target pixel, to compare against neighbors
    vec3 TargetNormal = GetPixelNormal( g_TexCoord.xy );
    vec3 TargetPosition = GetPixelPosition( g_TexCoord.xy );

    // Used to reduce jitter on distant surfaces
    float InvDepth = ( 1.0 / TargetPosition.z );

    // Iterations will spread out at this rate
    vec2 OutwardCrawl = g_StepSize; // * vec2( 0.1 );

    vec2 SampleDist = OutwardCrawl;

    for( int i = 0; i < NUM_ITERATIONS; i++ )
    {
        // Pick a new probe direction
        float dx = cos( SampleAngle );
        float dy = sin( SampleAngle );

        // Calculate jitter
        vec2 Jitter = ( SampleDist + Noise.xy * vec2( g_JitterAmount ) * vec2( 1.0 + InvDepth ) );

        // Choose a new sample point
        vec2 UVoffset = vec2( dx - dy, dy + dx );
        UVoffset *= Jitter;

        // Calculate ambient occlusion term for this new probe
        AO += AmbientOcclusionAt( g_TexCoord.xy + UVoffset, TargetPosition, TargetNormal );

        SampleDist += OutwardCrawl;

        SampleAngle += ( 6.283185307179586476925286766559 / float( NUM_ITERATIONS ) );
    }
    AO *= ( 1.0 / float( NUM_ITERATIONS ) );


    gl_FragColor = vec4( saturate( 1.0 - AO * g_AOIntensity ) );
}
