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

    VOID RenderFloor();
    VOID RenderMesh();
    VOID RenderProjectiveShadows();

    CFrmFontGLES           m_Font;
    CFrmUserInterfaceGLES  m_UserInterface;
    CFrmTimer              m_Timer;

    // Declare shader variables
    UINT32             m_hPerPixelLightingShader;
    INT32              m_hModelViewMatrixLoc;
    INT32              m_hModelViewProjMatrixLoc;
    INT32              m_hNormalMatrixLoc;
    INT32              m_hLightPositionLoc;
    INT32              m_hMaterialAmbientLoc;
    INT32              m_hMaterialDiffuseLoc;
    INT32              m_hMaterialSpecularLoc;

    UINT32             m_hConstantShader;
    INT32              m_hConstantMVPLoc;
    INT32              m_hConstantColorLoc;

    // Declare texture variables
    CFrmTexture*       m_pLogoTexture;

    // Declare mesh variables
    CFrmMesh           m_Floor;
    CFrmMesh           m_Meshes[ NUM_MESHES ];
    UINT32             m_nCurMeshIndex;

    // Declare transformation related variables
    FRMMATRIX4X4       m_matCameraPersp;

    FRMMATRIX4X4       m_matCameraView;
    FRMMATRIX4X4       m_matMeshModel;
    FRMMATRIX4X4       m_matCameraFloorModelView;
    FRMMATRIX4X4       m_matCameraFloorModelViewProj;
    FRMMATRIX3X3       m_matCameraFloorNormal;

    FRMMATRIX4X4       m_matCameraMeshModelView;
    FRMMATRIX4X4       m_matCameraMeshModelViewProj;
    FRMMATRIX3X3       m_matCameraMeshNormal;

    FRMMATRIX4X4       m_matLightFloorModelViewProj;
    FRMMATRIX4X4       m_matLightMeshModelViewProj;

    // Declare user controled variables
    FRMVECTOR3         m_vLightPosition;
    FRMVECTOR4         m_vPlane;
    BOOL               m_bUseStencil;
    BOOL               m_bShowFalseShadow;
};


#endif // SCENE_H
