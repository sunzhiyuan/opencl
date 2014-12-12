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

    VOID FreeMeshes();

    BOOL CreateShadowMapFBO();
    VOID FreeShadowMapFBO();
    VOID GetShadowMapFBO();

    BOOL CreateLightingFBO();
    VOID FreeLightingFBO();
    VOID GetLightingFBO();

    VOID RenderScreenAlignedQuad();
    VOID RenderShadowMapToFBO();
    VOID RenderLightingToFBO();
    VOID RenderProjectedShadowMapToFBO();
    VOID RenderBlendedLightingAndShadowMap();

    CFrmFontD3D            m_Font;
    CFrmUserInterfaceD3D   m_UserInterface;
    CFrmTimer              m_Timer;

    // Declare shader variables    
    struct ShadowMapConstantBuffer
    {
        FRMMATRIX4X4 matModelViewProj;
        FRMMATRIX4X4 matShadow;
        FRMMATRIX4X4 matScreenCoord;
        FRMVECTOR4   vAmbient;
        FLOAT32      fEpsilon;
        FRMVECTOR3   Padding; // Pad to 16-byte alignment

    };
    CFrmShaderProgramD3D    m_ShadowMapShader;
    ShadowMapConstantBuffer m_ShadowMapConstantBufferData;
    CFrmConstantBuffer*     m_pShadowMapConstantBuffer;
    
    CFrmShaderProgramD3D m_ShadowMapPCFShader;
    
    struct PerPixelLightingConstantBuffer
    {
        FRMMATRIX4X4 matModelView;
        FRMMATRIX4X4 matModelViewProj;
        FRMMATRIX4X4 matNormal;
        FRMVECTOR4   vAmbient;
        FRMVECTOR4   vDiffuse;
        FRMVECTOR4   vSpecular;
        FRMVECTOR4   vLightPos;
    };

    CFrmShaderProgramD3D           m_PerPixelLightingShader;    
    PerPixelLightingConstantBuffer m_PerPixelLightingConstantBufferData;
    CFrmConstantBuffer*            m_pPerPixelLightingConstantBuffer;

    CFrmShaderProgramD3D m_OverlayShader;
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
    CFrmShaderProgramD3D  m_DepthShader;
    DepthConstantBuffer   m_DepthConstantBufferData;
    CFrmConstantBuffer*   m_pDepthConstantBuffer;

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

    CFrmRenderTextureToScreenD3D m_RenderTextureToScreen;

    // Shadowmap render target
    Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_pShadowTexture;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   m_pShadowTextureRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShadowTextureShaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_pShadowTextureSampler;

    Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_pShadowDepthTexture;    
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_pShadowDepthStencilView;

    // Diffuse/Specular render target
    Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_pDiffuseSpecularTexture;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   m_pDiffuseSpecularTextureRenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pDiffuseSpecularTextureShaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_pDiffuseSpecularTextureSampler;

    Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_pDiffuseSpecularDepthTexture;    
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_pDiffuseSpecularDepthStencilView;
    
    // State Blocks
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_DefaultRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_PolygonOffsetRasterizerState;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DepthLEqualStencilState;
};


#endif // SCENE_H
