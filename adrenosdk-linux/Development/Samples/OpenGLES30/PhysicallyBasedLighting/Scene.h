//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: Physically Based Lighting
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "BasicMesh.h"

#ifndef SCENE_H
#define SCENE_H

// enum of meshes
enum { CUBE_MESH = 0,
       SPHERE_MESH,
       BUMPY_SPHERE_MESH,
       TORUS_MESH,
       ROCKET_MESH,
       NUM_MESHES };

// enum of shaders
enum { COOK_TORRANCE = 0,
       COOK_TORRANCE_FULL,
	   KEMEN_SZIRMAY_KALOS,
	   OPTIMIZED,
	   BLINN_PHONG,
       NUM_SHADERS };

typedef struct ShaderUniforms
{
	INT32 m_hModelViewMatrixLoc;
	INT32 m_hModelViewProjMatrixLoc;
	INT32 m_hNormalMatrixLoc;
	INT32 m_hLightPositionLoc;
	INT32 m_SkyColorLoc;
    INT32 m_GroundColorLoc;
    INT32 m_MaterialSpecularLoc;
    INT32 m_MaterialDiffuseLoc;
    INT32 m_LightColorLoc;
	INT32 m_MaterialGlossinessLoc;

} UNIFORMS;
//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
public:
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample( const CHAR* strName );

private:
    BOOL InitShaders();

    VOID CheckFrameBufferStatus();

    VOID FreeMeshes();

    VOID RenderScreenAlignedQuad();

    VOID RenderProjectedShadowMapToFBO();
    VOID RenderBlendedLightingAndShadowMap();

    CFrmFontGLES           m_Font;
    CFrmUserInterfaceGLES  m_UserInterface;
    CFrmTimer              m_Timer;

    // Declare shader variables

    UNIFORMS           m_Uniforms[NUM_SHADERS];

    UINT32             m_hOverlayShader;
    UINT32             m_hOverlayScreenSizeLoc;

    UINT32             m_hDepthShader;
    INT32              m_hDepthMVPLoc;

    // Declare texture variables
    CFrmTexture*       m_pLogoTexture;
	BasicMesh		   m_Meshes[ NUM_MESHES ];


    UINT32             m_nCurMeshIndex;

	UINT32			   m_Shaders[NUM_SHADERS];
	UINT32             m_nCurShaderIndex;

    UINT32             m_hDiffuseSpecularTexId;
    UINT32             m_hDiffuseSpecularBufId;
    UINT32             m_hDiffuseSpecularDepthBufId;

    // Declare transformation related variables
    FRMMATRIX4X4       m_matCameraPersp;

    FRMMATRIX4X4       m_matLightPersp;

    FRMMATRIX4X4       m_matCameraView;

    FRMMATRIX4X4       m_matCameraMeshModelView;
    FRMMATRIX4X4       m_matCameraMeshModelViewProj;
    FRMMATRIX3X3       m_matCameraMeshNormal;


    FRMVECTOR3         m_vLightLookAt;
    FRMVECTOR3         m_vLightUp;

	FRMVECTOR3		   m_SkyColor;
    FRMVECTOR3		   m_GroundColor;
    FRMVECTOR3		   m_MaterialSpecular;
    FRMVECTOR3		   m_MaterialDiffuse;
    FRMVECTOR3		   m_LightColor;
	float			   m_MaterialGlossiness;

    // Declare user controled variables
    FRMVECTOR3         m_vLightPosition;
};


#endif // SCENE_H
