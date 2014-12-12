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

    VOID FreeMeshes();

    CFrmFontD3D            m_Font;
    CFrmUserInterfaceD3D   m_UserInterface;
    CFrmTimer              m_Timer;

    // Declare shader variables
    struct PhysicallyBasedLightingConstantBuffer
    {
        FRMMATRIX4X4 matModelView;
        FRMMATRIX4X4 matModelViewProj;
        FRMMATRIX4X4 matNormal;
        FRMVECTOR4   vLightPos;
        FRMVECTOR4   SkyColor;
        FRMVECTOR4   GroundColor;
        FRMVECTOR4   vMaterialSpecular;
        FRMVECTOR4   vMaterialDiffuse;
        FRMVECTOR4   vLightColor;
        float        fMaterialGlossiness;
        FRMVECTOR3   Padding; // Pad to multiple of 16-bytes
    };
    PhysicallyBasedLightingConstantBuffer m_PhysicallyBasedLightingConstantBufferData;
    CFrmConstantBuffer*                   m_pPhysicallyBasedLightingConstantBuffer;

    // Declare texture variables
    CFrmTexture*       m_pLogoTexture;
	BasicMesh		   m_Meshes[ NUM_MESHES ];


    UINT32             m_nCurMeshIndex;

	CFrmShaderProgramD3D m_Shaders[NUM_SHADERS];
	UINT32               m_nCurShaderIndex;

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

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_RasterizerState;
};


#endif // SCENE_H
