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
    VOID RenderScreenAlignedQuad();

    CFrmFontGLES           m_Font;
    CFrmUserInterfaceGLES  m_UserInterface;
    CFrmTimer              m_Timer;

    UINT32             m_hShadowVolumeShader;
    INT32              m_hShadowVolumeLightDirectionLoc;
    INT32              m_hShadowVolumeNormalLoc;
    INT32              m_hShadowVolumeModelViewProjLoc;
    INT32              m_hShadowVolumeViewProjLoc;

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

    CFrmTexture*       m_pLogoTexture;

    CFrmMesh           m_Floor[ NUM_FLOORS ];
    CFrmMesh           m_FloorEdges[ NUM_FLOORS ];
    CFrmMesh           m_Meshes[ NUM_MESHES ];
    CFrmMesh           m_MeshEdges[ NUM_MESHES ];
    UINT32             m_nCurFloorIndex;
    UINT32             m_nCurMeshIndex;

    FRMVECTOR3         m_vPosition;
    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matPersp;
    FRMMATRIX4X4       m_matFloorModelView;
    FRMMATRIX4X4       m_matFloorModelViewProj;
    FRMMATRIX3X3       m_matFloorWorldNormal;
    FRMMATRIX3X3       m_matFloorEyeNormal;

    FRMMATRIX4X4       m_matMeshModel;
    FRMMATRIX4X4       m_matMeshModelView;
    FRMMATRIX4X4       m_matMeshModelViewProj;
    FRMMATRIX3X3       m_matMeshWorldNormal;
    FRMMATRIX3X3       m_matMeshEyeNormal;
    FRMMATRIX4X4       m_matViewProj;

    BOOL               m_bVisualizeShadowVolume;
    FRMVECTOR4         m_vLightDirection;
    FRMVECTOR3         m_vLightPosition;
};


#endif // SCENE_H
