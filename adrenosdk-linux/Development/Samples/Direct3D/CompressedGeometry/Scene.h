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

    VOID FreeMeshes();


    CFrmFontD3D            m_Font;
    CFrmUserInterfaceD3D   m_UserInterface;
    CFrmTimer              m_Timer;

    // Declare texture variables
    CFrmTexture*       m_pLogoTexture;
	CompressedMesh     m_MeshesF16[ NUM_MESHES ];
	CompressedMesh     m_Meshes101012[ NUM_MESHES ];
	CompressedMesh     m_Meshes24[ NUM_MESHES ];
	CompressedMesh     m_Meshes8[ NUM_MESHES ];

    struct CompressedGeometryConstantBuffer
    {
        FRMMATRIX4X4 matModelView;
        FRMMATRIX4X4 matCT; // compression transform (actually it's inverse)
        FRMMATRIX4X4 matModelViewProj;
        FRMMATRIX4X4 matNormal;
        FRMVECTOR4   vAmbient;
        FRMVECTOR4   vDiffuse;
        FRMVECTOR4   vSpecular;
        FRMVECTOR4   vLightPos;
    };

    CompressedGeometryConstantBuffer m_CompressedGeometryConstantBufferData;
    CFrmConstantBuffer*              m_pCompressedGeometryConstantBuffer;


    UINT32             m_nCurMeshIndex;
	CompressedMesh*    m_CurMeshArray;

	FRMVECTOR3		   m_MaterialDiffuse;

	CFrmShaderProgramD3D    m_CompressionShaders[ NUM_SHADER ];
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

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_RasterizerState;   
};


#endif // SCENE_H
