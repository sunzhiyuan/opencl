//--------------------------------------------------------------------------------------
// File: OrenNayarShader.glsl
// Desc: Shaders for per-pixel lighting
//
// Author:                 QUALCOMM, Adreno SDK
//
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



varying vec2 g_Tex;
varying vec3 g_Light;
varying vec3 g_View;
varying mat3 g_Test;

uniform sampler2D g_DiffuseTexture;
uniform sampler2D g_BumpTexture;
uniform sampler2D g_SinTanTexture;
uniform vec4 g_DiffuseColor;
uniform vec4 g_SpecularColor;
uniform vec4 g_AmbientLight;

uniform bool g_NormalsOnly;

//--------------------------------------------------------------------------------------
//  Unpacks a float16 from the pixel provided
//--------------------------------------------------------------------------------------
float UnpackSinTan( vec4 Texture )
{
    // packing formula:
    //
    // float NL = Pos.x;
    // float VN = Pos.y;
    // float Minimum = ((NL < VN) ? NL : VN);
    // float Maximum = ((NL < VN) ? VN : NL);
    // float Result = sin(acos(Minimum)) * tan(acos(Maximum));
    // Result = max(0.0f, Result);
    // float A = fmod(Result, 1.0f);
    // Result = (Result - A) / 256.0f;
    // float B = fmod(Result, 1.0f);
    // Result = (Result - B) / 256.0f;
    // float C = fmod(Result, 1.0f);
    // float D = 0.0f;
    // return float4(A, B, C, D);   // pixel color

    // reconstruct the floating point value
    float Result = dot( Texture, vec4( 1.0, 256.0, 65536.0, 0.0 ) );
    return Result;
}


void main()
{
    vec4 Color = texture2D( g_DiffuseTexture, g_Tex.xy ) * g_DiffuseColor;
    vec4 N = texture2D( g_BumpTexture, g_Tex.xy ) * 2.0 - 1.0;

    vec3 L = normalize( g_Light );
    vec3 V = normalize( g_View );
    vec3 Half = normalize( L + V );
    float NL = max( 0.0, dot( N.xyz, L ) );
    float VN = max( 0.0, dot( N.xyz, V ) );

    // specular shine
    float Specular = pow( max( 0.0, dot( N.xyz, Half ) ), g_SpecularColor.a );
	float SelfShadow = 4.0 * NL;

    // fetch texture with sin(alpha) * tan(alpha)
    vec4 SinTan = texture2D( g_SinTanTexture, vec2( NL * 0.5 + 0.5, VN * 0.5 + 0.5 ) );
    vec3 LProjected = normalize( L - vec3( NL ) * N.xyz );
    vec3 VProjected = normalize( V - vec3( VN ) * N.xyz );
    float C = saturate( dot( LProjected, VProjected ) );

    // unpack the SinTan value
    float SinTanValue = UnpackSinTan( SinTan );

 	// A in Color.w and B in N.w
 	gl_FragColor = g_AmbientLight;
    gl_FragColor += Color * saturate( ( vec4( NL ) * ( vec4( Color.w ) + vec4( N.w * C * SinTanValue ) ) ) );
    gl_FragColor.rgb  += SelfShadow * vec3(0.15, 0.15, 0.15) * g_SpecularColor.rgb * Specular;

    if( g_NormalsOnly )
        gl_FragColor = vec4( vec3( NL ), 1.0 );
}
