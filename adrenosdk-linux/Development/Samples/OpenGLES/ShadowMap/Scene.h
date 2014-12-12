//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H

// enum of floor meshes
enum { PLANE_FLOOR = 0,
       TERRAIN_FLOOR,
       NUM_FLOORS };

// enum of meshes
enum { CUBE_MESH = 0,
       SPHERE_MESH,
       BUMPY_SPHERE_MESH,
       TORUS_MESH,
       ROCKET_MESH,
       NUM_MESHES };

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

    VOID CreateShadowMapFBO();
    VOID FreeShadowMapFBO();
    VOID GetShadowMapFBO();

    VOID CreateLightingFBO();
    VOID FreeLightingFBO();
    VOID GetLightingFBO();

    VOID RenderScreenAlignedQuad();
    VOID RenderShadowMapToFBO();
    VOID RenderLightingToFBO();
    VOID RenderProjectedShadowMapToFBO();
    VOID RenderBlendedLightingAndShadowMap();

	BOOL				   m_Initialize;

    CFrmFontGLES           m_Font;
    CFrmUserInterfaceGLES  m_UserInterface;
    CFrmTimer              m_Timer;

    // Declare shader variables
    UINT32             m_hShadowMapShader;
    INT32              m_hShadowMapModelViewProjLoc;
    INT32              m_hShadowMapShadowMatrixLoc;
    INT32              m_hShadowMapScreenCoordMatrixLoc;
    INT32              m_hShadowMapShadowMapLoc;
    INT32              m_hShadowMapDiffuseSpecularMapLoc;
    INT32              m_hShadowMapAmbientLoc;

    UINT32             m_hShadowMapPCFShader;
    INT32              m_hShadowMapPCFModelViewProjLoc;
    INT32              m_hShadowMapPCFShadowMatrixLoc;
    INT32              m_hShadowMapPCFScreenCoordMatrixLoc;
    INT32              m_hShadowMapPCFShadowMapLoc;
    INT32              m_hShadowMapPCFDiffuseSpecularMapLoc;
    INT32              m_hShadowMapPCFAmbientLoc;
    INT32              m_hShadowMapPCFEpsilonLoc;

    UINT32             m_hPerPixelLightingShader;
    INT32              m_hModelViewMatrixLoc;
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

    // Declare mesh variables
    CFrmMesh           m_Floor[ NUM_FLOORS ];
    CFrmMesh           m_Meshes[ NUM_MESHES ];
    UINT32             m_nCurFloorIndex;
    UINT32             m_nCurMeshIndex;

    // Declare FBO related variables
    INT32              m_nShadowMapFBOTextureWidth;
    INT32              m_nShadowMapFBOTextureHeight;
    UINT32             m_nShadowMapFBOTextureExp;
    UINT32             m_hShadowMapBufId;
    UINT32             m_hShadowMapTexId;

    UINT32             m_hDiffuseSpecularTexId;
    UINT32             m_hDiffuseSpecularBufId;
    UINT32             m_hDiffuseSpecularDepthBufId;

    // Declare transformation related variables
    FRMMATRIX4X4       m_matCameraPersp;
    FRMMATRIX4X4       m_matLightPersp;

    FRMMATRIX4X4       m_matCameraView;
    FRMMATRIX4X4       m_matCameraFloorModelView;
    FRMMATRIX4X4       m_matCameraFloorModelViewProj;
    FRMMATRIX3X3       m_matCameraFloorNormal;

    FRMMATRIX4X4       m_matCameraMeshModelView;
    FRMMATRIX4X4       m_matCameraMeshModelViewProj;
    FRMMATRIX3X3       m_matCameraMeshNormal;

    FRMMATRIX4X4       m_matFloorModel;

    FRMVECTOR3         m_vLightLookAt;
    FRMVECTOR3         m_vLightUp;

    FRMMATRIX4X4       m_matLightModelViewProj;
    FRMMATRIX4X4       m_matLightFloorModelViewProj;
    FRMMATRIX4X4       m_matLightMeshModelViewProj;

    FRMMATRIX4X4       m_matFloorShadowMatrix;
    FRMMATRIX4X4       m_matMeshShadowMatrix;

    FRMMATRIX4X4       m_matFloorScreenCoordMatrix;
    FRMMATRIX4X4       m_matMeshScreenCoordMatrix;

    FRMMATRIX4X4       m_matScaleAndBias;

    // Declare user controled variables
    FRMVECTOR3         m_vLightPosition;
    BOOL               m_bShowShadowMap;
    BOOL               m_bUsePCF;
};


#endif // SCENE_H
