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
    enum ColorSpace
    {
        COLORSPACE_RGB_PASSTHROUGH,
        COLORSPACE_CIE,
        COLORSPACE_HSV,
        COLORSPACE_L16LUV,
        MAX_COLORSPACES
    };

public:
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample( const CHAR* strName );
    
    VOID RenderScreenAlignedQuad( FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight );

private:    
    BOOL InitShaders();
    VOID DrawScene();

    BOOL CreateFBO( UINT32 nWidth, UINT32 nHeight, DXGI_FORMAT nFormat, FrameBufferObject** ppFBO );	
    VOID DestroyFBO( FrameBufferObject* pFBO );
    VOID BeginFBO( FrameBufferObject* pFBO );
    VOID EndFBO( FrameBufferObject* pFBO );

    
    CFrmPackedResourceD3D   resource;

    CFrmFontD3D             m_Font;
    CFrmTimer               m_Timer;
    CFrmUserInterfaceD3D    m_UserInterface;
    CFrmTexture*            m_pLogoTexture;
    BOOL                    m_bsRGBRenderTarget;
    FrameBufferObject*      m_sRGBRenderTarget;


    CFrmTexture*            m_psRGBTexture;
    CFrmTexture*            m_pRGBTexture;    
    CFrmVertexBuffer*       m_pQuadVertexBuffer;
    CFrmIndexBuffer*        m_pQuadIndexBuffer;
    
    CFrmShaderProgramD3D    m_sRGBShader;
    CFrmShaderProgramD3D    m_sRGBFinalGammaCorrectShader;

    Microsoft::WRL::ComPtr<ID3D11RasterizerState>    m_RasterizerState;    
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DefaultDepthStencilState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  m_DisabledDepthStencilState;

};


#endif // SCENE_H
