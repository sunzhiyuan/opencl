//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "CompressedMesh.h"

#ifndef SCENE_H
#define SCENE_H

// enum of meshes
enum { CUBE_MESH = 0,
       SPHERE_MESH,
       BUMPY_SPHERE_MESH,
       TORUS_MESH,
       ROCKET_MESH,
       NUM_MESHES };

// enum of compression shaders
enum { COMPRESS_F16 = 0,
       COMPRESS_101012,
	   COMPRESS_24,//Can't send unsigned ints to shader yet*   
	   COMPRESS_8,
       NUM_SHADER };

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

    INT32              m_hModelViewMatrixLoc;
	INT32              m_hCTMatrixLoc; // compression transform matrix
    INT32              m_hModelViewProjMatrixLoc;
    INT32              m_hNormalMatrixLoc;
    INT32              m_hLightPositionLoc;
    INT32              m_hMaterialAmbientLoc;
    INT32              m_hMaterialDiffuseLoc;
    INT32              m_hMaterialSpecularLoc;

    UINT32             m_hOverlayShader;
    UINT32             m_hOverlayScreenSizeLoc;

    UINT32             m_hDepthShader;
    INT32              m_hDepthMVPLoc;

    // Declare texture variables
    CFrmTexture*       m_pLogoTexture;
	CompressedMesh     m_MeshesF16[ NUM_MESHES ];
	CompressedMesh     m_Meshes101012[ NUM_MESHES ];
	CompressedMesh     m_Meshes24[ NUM_MESHES ];
	CompressedMesh     m_Meshes8[ NUM_MESHES ];

    UINT32             m_nCurMeshIndex;
	CompressedMesh*    m_CurMeshArray;

	FRMVECTOR3		   m_MaterialDiffuse;

	UINT32			   m_CompressionShaders[ NUM_SHADER ];
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

    // Declare user controled variables
    FRMVECTOR3         m_vLightPosition;
};


#endif // SCENE_H
