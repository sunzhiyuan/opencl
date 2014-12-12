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
[ClothVS]
#include "CommonVS.glsl"

uniform   mat4 g_matModel;
uniform   mat4 g_matModelView;
uniform   mat4 g_matModelViewProj;
uniform   mat3 g_matNormal;

attribute vec4 g_vPositionOS;
attribute vec3 g_vNormalOS;
attribute vec3 g_vTexCoord;
attribute vec3 g_vTangent;
attribute vec3 g_vBitangent;

varying   vec3 g_vViewVecES;
varying   vec2 g_vTexCoord0;
varying   vec3 g_vOutWorldPos;
varying   vec3 g_vOutNormal;
varying   vec3 g_vOutTangent;
varying   vec3 g_vOutBitangent;

void main()
{
    vec4 vPositionES = g_matModelView     * g_vPositionOS;
    vec4 vPositionCS = g_matModelViewProj * g_vPositionOS;

    g_vTexCoord0 = g_vTexCoord.xy;

    // Pass everything off to the fragment shader
    gl_Position  = vPositionCS;
    g_vViewVecES = vec3(0.0,0.0,0.0) - vPositionES.xyz;
    
    g_vOutWorldPos = (g_matModel * vec4(g_vPositionOS.xyz, 1.0)).xyz;
	g_vOutNormal = (g_matModel * vec4(g_vNormalOS, 0.0)).xyz;
	g_vOutTangent = (g_matModel * vec4(g_vTangent.xyz, 0.0)).xyz;
	g_vOutBitangent = (g_matModel * vec4(g_vBitangent.xyz, 0.0)).xyz;

}


//--------------------------------------------------------------------------------------
// The fragment shader
//--------------------------------------------------------------------------------------
[ClothFS]
#include "CommonFS.glsl"

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
