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


enum
{
	CEL_SHADED_FBO = 0,
    NORMAL_AND_DEPTH_FBO,
    OUTLINE_FBO,
    NUM_FBOS 
};


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

    BOOL CreateDrawBuffers();
	BOOL FreeDrawBuffers();


    VOID RenderScreenAlignedQuad();
    VOID RenderSceneCelShadedDepthAndNormalsToFBO();
    VOID RenderOutlineToFBO();
    VOID RenderOutline();

    CFrmFontD3D            m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceD3D   m_UserInterface;

    CFrmMesh           m_Mesh;

    BOOL               m_bShouldRotate;
    FLOAT              m_fRotateTime;

    // FBO related
    UINT32             m_nFBOTextureWidth;
    UINT32             m_nFBOTextureHeight;
    
    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX3X3       m_matNormalMatrix;

    // Shader related data
    CFrmTexture*       m_pLogoTexture;

    struct CelConstantBuffer
    {
        FRMVECTOR4 vLightPos;
        FRMVECTOR4 vAmbient;
        FRMMATRIX4X4 matModelViewProj;
        FRMMATRIX4X4 matNormal;
    };

    CFrmShaderProgramD3D   m_CelShadingShader;
    CFrmConstantBuffer* m_pCelConstantBuffer;
    CelConstantBuffer   m_CelConstantBufferData;
    
    CFrmTexture*       m_pCelShadingTexture;
    CFrmTexture*       m_pRayGunTexture;

    struct BlendConstantBuffer
    {
        FLOAT32 fHalfStepSizeX;
        FLOAT32 fHalfStepSizeY;
        FRMVECTOR2 Padding;
    };
    CFrmShaderProgramD3D   m_NormalDepthShader;
    CFrmConstantBuffer* m_pNormalDepthConstantBuffer;
    BlendConstantBuffer m_NormalDepthConstantBufferData;    
    
    FLOAT32            m_fOutlineHalfStepSizeX;
    FLOAT32            m_fOutlineHalfStepSizeY;

    FLOAT32            m_fOutlineWidth;

    FRMVECTOR3         m_vLightPosition;

    FLOAT32            m_fAmbient;
    
	UINT32			   m_hFrameBufferNameId;

    CFrmVertexBuffer*  m_pFullScreenQuadVertexBuffer;
    CFrmIndexBuffer*   m_pFullScreenQuadIndexBuffer;

    Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_pTextures[ NUM_FBOS ];
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   m_pRenderTargetViews[ NUM_FBOS ];
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceViews[ NUM_FBOS ];
    Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_pSamplers[ NUM_FBOS ];

    Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_pDepthTexture;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_pDepthRenderTargetView;    
    
    
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_RasterizerState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DefaultDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DisabledDepthStencilState;
};


#endif // SCENE_H
