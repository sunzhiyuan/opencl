//--------------------------------------------------------------------------------------
// File: Cloth.glsl
// Desc: Shaders for ClothSimCLGLES sample
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
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
#extension GL_OES_texture_3D : enable


// Define some useful macros
#define saturate(x) clamp( x, 0.0, 1.0 )
#define lerp        mix



struct MATERIAL
{
    vec4  vAmbient;
    vec4  vDiffuse;
    vec4  vSpecular;
};

uniform sampler2D g_DiffuseMap;
uniform sampler2D g_NormalMap;

uniform MATERIAL g_Material;
uniform vec3     g_vLightPos;

varying   vec3 g_vViewVecES;
varying   vec2 g_vTexCoord0;
varying   vec3 g_vOutWorldPos;
varying   vec3 g_vOutNormal;
varying   vec3 g_vOutTangent;
varying   vec3 g_vOutBitangent;


void PointLight(vec3 worldNormal,
				vec3 worldPos,
				vec3 eyeDir,
				vec3 lightPos,
				vec3 lightColor,
				float range,
				inout vec3 diffuseContrib,
				inout vec3 specularContrib)
{
	vec3 lightVec = lightPos - worldPos;
	vec3 lightDir = normalize(lightVec);
	float dist = length(lightVec);
	float rangeAtten = max(0.0, 1.0 - dist/range);

	float diffuse = max(0.0, dot(worldNormal, lightDir));
	diffuseContrib += diffuse * rangeAtten * lightColor;

	vec3 halfDir = normalize(-eyeDir + lightDir);
	float specular = max(0.0, dot(worldNormal, halfDir));
	specular = pow(specular, g_Material.vSpecular.a);
	specularContrib += specular * rangeAtten * lightColor;
}

void main()
{
    vec4 diffuseMap = texture2D( g_DiffuseMap, g_vTexCoord0 );
	diffuseMap.w = min(1.0, diffuseMap.w * 1.0);

    vec3 normal = normalize( texture2D( g_NormalMap, g_vTexCoord0 ).xyz * 2.0 - 1.0);
    mat3 tanWorld = mat3(normalize(g_vOutTangent), normalize(g_vOutBitangent), normalize(g_vOutNormal));
    vec3 worldNormal = tanWorld * normal;

    vec3 vLight  = normalize( g_vLightPos.xyz );
    vec3 vView   = normalize( g_vViewVecES.xyz );
    vec3 vHalf   = normalize( vLight.xyz + vView.xyz );

    float diffuse  = max( 0.0, dot( worldNormal, vLight ) );
    vec3 ambient = g_Material.vAmbient.rgb;
    float specular = pow( max( 0.0, dot( worldNormal, vHalf ) ), g_Material.vSpecular.a );

    vec3 diffuseColor = vec3(0.0, 0.0, 0.0);
    vec3 specularColor = vec3(0.0, 0.0, 0.0);
    vec3 lightCol = vec3(1.0, 1.0, 1.0);

    PointLight(worldNormal, g_vOutWorldPos, vView, vLight, lightCol, 1.0, diffuseColor, specularColor);

    vec3 litColor = ((ambient + diffuse) * diffuseMap.xyz + specular * diffuseMap.w)
					+ diffuseColor * diffuseMap.xyz
					+ specularColor * diffuseMap.w;

    gl_FragColor = vec4( litColor, 1.0 );
}
