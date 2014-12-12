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

#include <vector>

// Wrapper for FBO objects and properties
struct FrameBufferObject
{
	UINT32 m_nWidth;
	UINT32 m_nHeight;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture0;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTexture0RenderTargetView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture0ShaderResourceView;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pTexture0Sampler;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pDepthTexture;    
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
};

//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
public:
    CSample( const CHAR* strName );

    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Update();
    virtual VOID Render();
    virtual VOID Destroy();

private:
    BOOL InitShaders();
    BOOL CreateFBO( UINT32 nWidth, UINT32 nHeight, DXGI_FORMAT nFormat, UINT32 nSampleCount, FrameBufferObject** ppFBO );
    VOID DestroyFBO( FrameBufferObject* pFBO );
	VOID BeginFBO( FrameBufferObject* pFBO );
	VOID EndFBO( FrameBufferObject* pFBO );
    VOID RenderScreenAlignedQuad();

    CFrmFontD3D           m_Font;
    CFrmTimer             m_Timer;
    CFrmUserInterfaceD3D  m_UserInterface;

    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX3X3       m_matNormal;
    CFrmMesh           m_Mesh;

    CFrmArcBall        m_ArcBall;
    FRMVECTOR4         m_qMeshRotation;
    FRMVECTOR4         m_qRotationDelta;

    CFrmTexture*       m_pLogoTexture;

    const CHAR*        m_strLightingType;
    FRMVECTOR3         m_vLightPosition;
    INT32              m_nMaterialIndex;
    const CHAR*        m_strMaterialName;

    CFrmVertexBuffer*  m_pQuadVertexBuffer;
    CFrmIndexBuffer*   m_pQuadIndexBuffer;

    FrameBufferObject* m_MSAART;
    FrameBufferObject* m_ResolvedRT;

    std::vector<UINT>  m_SampleCountSupported;
    INT32              m_nSampleCount;
    UINT32             m_nSampleCountIndex;

    BOOL               m_bMSAA;
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

    CFrmShaderProgramD3D           m_PreviewShader;
    
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_RasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_MSAARasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DefaultDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DisabledDepthStencilState;

};


#endif // SCENE_H
