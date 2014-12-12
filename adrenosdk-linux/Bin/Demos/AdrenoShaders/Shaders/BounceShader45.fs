//--------------------------------------------------------------------------------------
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

uniform sampler2D g_NormalTexture;
uniform sampler2D g_DepthTexture;
uniform sampler2D g_JitterTexture;
uniform sampler2D g_ColorTexture;
uniform mat4 g_ProjInv;
uniform vec2 g_StepSize;
uniform float g_AOEdgeFinder;
uniform float g_AOIntensity;
uniform float g_JitterAmount;
uniform float g_BounceIntensity;


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


//------------------------------------------------------------------------------
// Calculates how much a neighbor pixel's light should influence the target
// pixel's.
//
float BounceLightAt( vec2 NeighborUV, vec3 TargetPosition, vec3 TargetNormal )
{
    vec3  Delta = GetPixelPosition( NeighborUV ) - TargetPosition;
    vec3  N = GetPixelNormal( NeighborUV );
    vec3  V = normalize( Delta );
    float D = length( Delta ) * 0.1;
    float Epsilon = 0.0;


    float b0 = 1.0 - saturate( dot( N, -V ) - Epsilon );
    float b1 = 1.0 - saturate( dot( TargetNormal, V ) - Epsilon );
    float b2 = 1.0; // / sqrt( 1.0 / ( D * D * 150.0 ) + 1.0 );

    return b0 * b1 * b2;
}


void main()
{
    //#define NUM_ITERATIONS 32
    #define NUM_ITERATIONS 2

    // To calculate the ambient occlusion term, turn this on (the alpha channel
    // of this pixel will then be the ambient term, which can then be used in
    // the combine shader)
    //#define USE_AMBIENT_OCCLUSION

    float SampleAngle = 0.0;

    // Accumulation variables
    float AO = 0.0;
    vec4 B = vec4( 0.0 );

    // Light color of the target pixel
    vec4 Color = texture2D( g_ColorTexture, g_TexCoord.xy );

    // For noise
    vec4 Noise = texture2D( g_JitterTexture, g_TexCoord.xy * vec2( 12.0 ) );

    // Grab these values for the target pixel, to compare against neighbors
    vec3 TargetNormal = GetPixelNormal( g_TexCoord.xy );
    vec3 TargetPosition = GetPixelPosition( g_TexCoord.xy );

    // Used to reduce jitter on distant surfaces
    float InvDepth = ( 1.0 / TargetPosition.z );

    // Iterations will spread out at this rate
    vec2 OutwardCrawl = g_StepSize * 0.5;

    vec2 SampleDist = OutwardCrawl;

    for( int i = 0; i < NUM_ITERATIONS; i++ )
    {
        // Pick a new probe direction
        float dx = cos( SampleAngle );
        float dy = sin( SampleAngle );

        // Choose a new sample point
        vec2 UVoffset = vec2( dx - dy, dy + dx );

        // Calculate ambient occlusion term for this new probe
#ifdef USE_AMBIENT_OCCLUSION
        vec2 Jitter = ( SampleDist + Noise.xy * vec2( g_JitterAmount ) * vec2( 1.0 + InvDepth ) );
        vec2 UVoffsetAO = g_TexCoord.xy + UVoffset * Jitter;
        AO += AmbientOcclusionAt( UVoffsetAO, TargetPosition, TargetNormal );
#endif

        vec2 UVoffsetBounce = g_TexCoord.xy + UVoffset * SampleDist * 6.0;

        // Calculate bounce lighting term
        B += BounceLightAt( UVoffsetBounce, TargetPosition, TargetNormal ) * texture2D( g_ColorTexture, UVoffsetBounce) ;

        SampleDist += OutwardCrawl;
        SampleAngle += ( 6.283185307179586476925286766559 / float( NUM_ITERATIONS ) );
    }
#ifdef USE_AMBIENT_OCCLUSION    
    AO *= ( 1.0 / float( NUM_ITERATIONS ) * g_BounceIntensity );
#endif
    B *= vec4( 1.0 / float( NUM_ITERATIONS ) ) * g_BounceIntensity;

#ifdef USE_AMBIENT_OCCLUSION    
    gl_FragColor = vec4( B.rgb, saturate( 1.0 - AO * g_AOIntensity ) );
#else
    gl_FragColor = vec4( B.rgb, 1.0 );
#endif
}
