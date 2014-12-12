//--------------------------------------------------------------------------------------
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// The vertex shader
//--------------------------------------------------------------------------------------
[CombineShader40VS]
#include "CommonVS.glsl"

attribute vec4 g_Vertex;

varying vec2 g_TexCoord;
varying vec3   g_vFreqTexCoords;

uniform float  g_fFrequency;
uniform float  g_fTime;

void main()
{
    gl_Position  = vec4( g_Vertex.x, g_Vertex.y, 0.0, 1.0 );
    g_TexCoord = vec2( g_Vertex.z, g_Vertex.w );
    g_vFreqTexCoords = vec3( g_TexCoord * g_fFrequency, g_fTime );
   
}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[CombineShader40FS]
#include "CommonFS.glsl"

// Enable supported extensions
#extension GL_OES_texture_3D : enable


varying vec2 g_TexCoord;
varying vec3   g_vFreqTexCoords;

uniform sampler2D g_SharpTexture;
uniform sampler2D g_BlurredTexture;
uniform sampler3D g_Rand3DTexture;

uniform float g_BloomIntensity;
uniform float g_Contrast;
uniform float g_Saturation;
uniform vec3 g_ColorCorrect;
uniform vec3 g_ColorAdd;


void main()
{
    vec4 FocusPixel = texture2D( g_SharpTexture, g_TexCoord );
    vec4 BlurPixel = texture2D( g_BlurredTexture, g_TexCoord );
    
    vec4 Color = clamp(FocusPixel + (BlurPixel * g_BloomIntensity), 0.0, 1.0);
 //   gl_FragColor =  Color;

//	float rand = texture3D(g_Rand3DTexture, g_vFreqTexCoords).r; 
	
    vec3 IntensityConverter = vec3(0.299, 0.587, 0.114); 
	float Y = dot(IntensityConverter, Color.xyz); 

	vec4 sepiaConvert = vec4(0.191, -0.054,-0.221,0.0); 
    
 //   gl_FragColor = vec4(rand, rand, rand, rand) * (Y + sepiaConvert);
    gl_FragColor =  (Y + sepiaConvert);
}
