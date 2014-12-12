//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: Port of ShaderX7 Practical CSM
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "BasicMesh.h"
#include "Mover.h"
#include "CSM\Shadows.h"
#include "CSM\Settings.h"

#ifndef SCENE_H
#define SCENE_H

// Wrapper for FBO objects and properties
struct FrameBufferObject
{
    UINT32 m_nWidth;
    UINT32 m_nHeight;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTextureRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureShaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pTextureSampler;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthTexture;    
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

};

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

    VOID DrawGround();

    BOOL CreateShadowMapFBO();

    VOID BeginFBO( FrameBufferObject* pFBO );

    VOID EndFBO( FrameBufferObject* pFBO );

    BOOL InitShaders();

    VOID FreeMeshes();

    VOID RenderScreenAlignedQuad();

    VOID RenderProjectedShadowMapToFBO();
    VOID RenderBlendedLightingAndShadowMap();

    CFrmFontD3D            m_Font;
    CFrmUserInterfaceD3D   m_UserInterface;
    CFrmTimer              m_Timer;

    static const int   MODEL_DIMENSION = 5;
    static const int sizeShadow = 512;

    // Declare shader variables
    CFrmShaderProgramD3D m_OverlayShader;
    CFrmShaderProgramD3D m_PreviewShader;
    CFrmShaderProgramD3D m_DepthShader;

    struct OverlayConstantBuffer
    {
        FRMVECTOR4 vScreenSize;
    };

    OverlayConstantBuffer m_OverlayConstantBufferData;
    CFrmConstantBuffer*   m_pOverlayConstantBuffer;

    struct DepthConstantBuffer
    {
        FRMMATRIX4X4 matModelViewProj;
    };

    DepthConstantBuffer   m_DepthConstantBufferData;
    CFrmConstantBuffer*   m_pDepthConstantBuffer;

    
    // Declare texture variables
    CFrmTexture*       m_pLogoTexture;

    // Declare transformation related variables
    FRMMATRIX4X4       m_matCameraPersp;
    FRMMATRIX4X4       m_matCameraView;

    FRMMATRIX4X4       m_matCameraMeshModelView;
    FRMMATRIX4X4       m_matCameraMeshModelViewProj;
    FRMMATRIX3X3       m_matCameraMeshNormal;

    FRMMATRIX4X4	   m_ModelMtxs [ MODEL_DIMENSION * MODEL_DIMENSION ];

    FRMVECTOR3         m_vLightLookAt;
    FRMVECTOR3         m_vLightUp;

    // Declare user controled variables
    FRMVECTOR3         m_vSunDirection;
        
    BasicMesh		   m_Meshes[ NUM_MESHES ];

    UINT32             m_nCurMeshIndex;
    UINT32             m_nCurShaderIndex;

    Mover mover;//camera mover

    Shadows			   m_ShadowMan; //shadow manager
    Settings		   m_ShadowSettings;
    
    bool			   m_Preview;

    FrameBufferObject* m_pFBO;
    CFrmVertexBuffer*  m_pGroundVertexBuffer;
    CFrmIndexBuffer*   m_pGroundIndexBuffer;
    CFrmRenderTextureToScreenD3D m_RenderTextureToScreen;


    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_RasterizerState;   
    
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DepthLEqualStencilState;
};


#endif // SCENE_H
