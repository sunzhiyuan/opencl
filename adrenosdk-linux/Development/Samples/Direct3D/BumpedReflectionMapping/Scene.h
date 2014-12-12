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

#ifndef SCENE_H
#define SCENE_H

// enum of meshes
enum { SPHERE_MESH = 0,
       SKYDOME_MESH,
       NUM_MESHES };

// enum of shaders
enum { BUMPED_REFLECTION = 0,
       SKYDOME,
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

    FLOAT32                m_FresnelPower;
    FLOAT32                m_SpecularPower;

    // Declare shader variables
    struct BumpedReflectionConstantBuffer
    {
        FRMMATRIX4X4 MatModelViewProj;
        FRMMATRIX4X4 MatModelView;
        FRMMATRIX4X4 MatModel;
        FRMMATRIX4X4 MatNormal;
        FRMVECTOR4   LightPos;
        FRMVECTOR4   EyePos;
        FLOAT32      FresnelPower;
        FLOAT32      SpecularPower;
        FRMVECTOR2   Padding; // Pad to multiple of 16-bytes
    };

    BumpedReflectionConstantBuffer m_BumpedReflectionConstantBufferData;
    CFrmConstantBuffer*            m_pBumpedReflectionConstantBuffer;

    struct SkydomeConstantBuffer
    {
        FRMMATRIX4X4 MatModelViewProj;
        FRMMATRIX4X4 MatNormal;
    };

    SkydomeConstantBuffer m_SkydomeConstantBufferData;
    CFrmConstantBuffer*   m_pSkydomeConstantBuffer;

    // Declare texture variables
    CFrmTexture*       m_pLogoTexture;
	CFrmMesh		   m_Meshes[ NUM_MESHES ];
    
	CFrmShaderProgramD3D m_Shaders[NUM_SHADERS];
	
    // Declare transformation related variables
    FRMMATRIX4X4       m_matCameraPersp;

    FRMMATRIX4X4       m_matLightPersp;

    FRMMATRIX4X4       m_matCameraView;

    FRMMATRIX4X4       m_matCameraMeshModelView;
    FRMMATRIX4X4       m_matCameraMeshModelViewProj;
    FRMMATRIX4X4       m_matModel;
    FRMMATRIX3X3       m_matCameraMeshNormal;
    
    FRMMATRIX4X4       m_matSkyDomeModelViewProj;
    FRMMATRIX3X3       m_matSkyDomeNormal;


    FRMVECTOR3         m_vLightLookAt;
    FRMVECTOR3         m_vLightUp;

	FRMVECTOR3         m_LightPos;
    FRMVECTOR3         m_CameraPos;

    CFrmTexture*       m_pDiffuseMap;
    CFrmTexture*       m_pNormalMap;
    CFrmCubeMap*       m_pCubeMap;

    // Declare user controled variables
    FRMVECTOR3         m_vLightPosition;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_RasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_BackFaceRasterizerState;
};


#endif // SCENE_H
