//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include <FrmApplication.h>
#include <Direct3D/FrmFontD3D.h>
#include <Direct3D/FrmMesh.h>
#include <Direct3D/FrmPackedResourceD3D.h>
#include <Direct3D/FrmShader.h>
#include <Direct3D/FrmUserInterfaceD3D.h>
#include "Scene.h"

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader::MaterialShader()
{
    m_pMaterialConstantBuffer = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader::Destroy()
{
    if ( m_pMaterialConstantBuffer ) m_pMaterialConstantBuffer->Release();
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
DownsizeShader::DownsizeShader()
{
    m_pDownsizeShaderConstantBuffer = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID DownsizeShader::Destroy()
{
    if ( m_pDownsizeShaderConstantBuffer ) m_pDownsizeShaderConstantBuffer->Release();
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID AdaptLuminanceShader::Destroy()
{
    if( m_pAdaptLuminanceConstantBuffer ) m_pAdaptLuminanceConstantBuffer->Release();
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
AdaptLuminanceShader::AdaptLuminanceShader()
{
    m_pAdaptLuminanceConstantBuffer = NULL;
    //ElapsedTime = 1.0f;
    //Lambda = 0.1f;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
LuminanceShader::LuminanceShader()
{
    m_pLuminanceShaderConstantBuffer = NULL;	
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID LuminanceShader::Destroy()
{
    if ( m_pLuminanceShaderConstantBuffer ) m_pLuminanceShaderConstantBuffer->Release();
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BrightPassShader::BrightPassShader()
{
    m_pBrightPassConstantBuffer	= 0;
    MiddleGray = 0.18f;
    BrightPassWhite = 1.0f;
    BrightPassThreshold = 0.7f;
    BrightPassOffset = 0.4f;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BrightPassShader::Destroy()
{
    if ( m_pBrightPassConstantBuffer ) m_pBrightPassConstantBuffer->Release();
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CombineShader::CombineShader()
{
    m_pCombineShaderConstantBuffer = NULL;
    GaussSpread = 1.0f;
    GaussRho = 1.0f;
    MiddleGray = 0.18f;
    White = 1.0f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CombineShader::Destroy()
{
    if ( m_pCombineShaderConstantBuffer ) m_pCombineShaderConstantBuffer->Release();
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BlurShader::BlurShader()
{
    m_pBlurConstantBuffer = NULL;
    StepSize = FRMVECTOR2( 1.0f, 1.0f );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BlurShader::Destroy()
{
    if ( m_pBlurConstantBuffer ) m_pBlurConstantBuffer->Release();
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
PreviewShader::PreviewShader()
{
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID PreviewShader::Destroy()
{
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject::SimpleObject()
{
    ModelScale = 1.0f;
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
    BumpTexture = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject::Destroy()
{
}

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Tone Mapping" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_pLogoTexture = NULL;

    m_SharpRT               = NULL;
    m_BlurredRT             = NULL;
    m_ScratchRT             = NULL;
    m_64x64RT               = NULL;
    m_16x16RT               = NULL;
    m_4x4RT                 = NULL;
    m_1x1RT                 = NULL;
    m_LastAverageLumRT      = NULL;
    m_PreviousAverageLumRT  = NULL;
    m_BrightPassRT          = NULL;
    m_QuarterRT             = NULL;

    m_pQuadVertexBuffer     = NULL;
    m_pQuadIndexBuffer      = NULL;
    m_pDynamicQuadVertexBuffer = NULL;
    
    m_ShouldRotate		= TRUE;

    m_fDownsizeRatio    = 0.4f;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the material shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION,   DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD", FRM_VERTEX_TEXCOORD0,  DXGI_FORMAT_R32G32_FLOAT },
            { "NORMAL",   FRM_VERTEX_NORMAL,     DXGI_FORMAT_R32G32B32_FLOAT },
            { "TANGENT",  FRM_VERTEX_TANGENT,    DXGI_FORMAT_R32G32B32_FLOAT },
            { "BINORMAL", FRM_VERTEX_BINORMAL,   DXGI_FORMAT_R32G32B32_FLOAT }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_MaterialShader.Shader.Compile( "MaterialShaderVS.cso", "MaterialShaderPS.cso",
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Create constant buffer
        if( FALSE == FrmCreateConstantBuffer( sizeof(m_MaterialShader.m_MaterialConstantBufferData), 
                                              &m_MaterialShader.m_MaterialConstantBufferData,
                                              &m_MaterialShader.m_pMaterialConstantBuffer ) )
            return FALSE;                                              
    }

    // downsize shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_DownsizeShader.Shader.Compile( "DownsizeShaderVS.cso", "DownsizeShaderToneMappingPS.cso",
                                                       pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCreateConstantBuffer( sizeof(m_DownsizeShader.m_DownsizeShaderConstantBufferData),
                                              &m_DownsizeShader.m_DownsizeShaderConstantBufferData,
                                              &m_DownsizeShader.m_pDownsizeShaderConstantBuffer ) )
            return FALSE;                                              
    }

    // luminance shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_LuminanceShader.Shader.Compile( "GenerateLuminanceVS.cso", "GenerateLuminancePS.cso",
                                                       pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
    
        if( FALSE == FrmCreateConstantBuffer( sizeof(m_LuminanceShader.m_LuminanceShaderConstantBufferData),
                                              &m_LuminanceShader.m_LuminanceShaderConstantBufferData,
                                              &m_LuminanceShader.m_pLuminanceShaderConstantBuffer ) )
            return FALSE;                                              

    }

    // average luminance shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_AvgLuminanceShader.Shader.Compile( "AverageLuminanceVS.cso", "AverageLuminancePS.cso",
                                                       pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
    
        if( FALSE == FrmCreateConstantBuffer( sizeof(m_AvgLuminanceShader.m_LuminanceShaderConstantBufferData),
                                              &m_AvgLuminanceShader.m_LuminanceShaderConstantBufferData,
                                              &m_AvgLuminanceShader.m_pLuminanceShaderConstantBuffer ) )
            return FALSE;           
    }

    // adapt luminance shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_AdaptLuminanceShader.Shader.Compile( "AdaptLuminanceVS.cso", "AdaptLuminancePS.cso",
                                                            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCreateConstantBuffer( sizeof(m_AdaptLuminanceShader.m_AdaptLuminanceConstantBufferData),
                                              &m_AdaptLuminanceShader.m_AdaptLuminanceConstantBufferData,
                                              &m_AdaptLuminanceShader.m_pAdaptLuminanceConstantBuffer ) )
            return FALSE;           

    }

    // bright pass shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);
        
        if( FALSE == m_BrightPassShader.Shader.Compile( "BrightPassVS.cso", "BrightPassPS.cso",
                                                        pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCreateConstantBuffer( sizeof(m_BrightPassShader.m_BrightPassConstantBufferData),
                                              &m_BrightPassShader.m_BrightPassConstantBufferData,
                                              &m_BrightPassShader.m_pBrightPassConstantBuffer ) )
            return FALSE;           	
    }

    // copy render target shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_CopyRTShader.Shader.Compile( "CopyRTVS.cso", "CopyRTPS.cso",
                                                    pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

    }

    // blur shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_BlurShader.Shader.Compile( "BlurShaderVS.cso", "BlurShaderPS.cso",
                                                  pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Create constant buffer
        if( FALSE == FrmCreateConstantBuffer( sizeof(m_BlurShader.m_BlurConstantBufferData), 
                                              &m_BlurShader.m_BlurConstantBufferData,
                                              &m_BlurShader.m_pBlurConstantBuffer ) )
            return FALSE;                                              

        SetupGaussWeights();
    }

    // combine shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_CombineShader.Shader.Compile( "CombineShaderVS.cso", "CombineShaderToneMappingPS.cso",
                                                     pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Create constant buffer
        if( FALSE == FrmCreateConstantBuffer( sizeof(m_CombineShader.m_CombineShaderConstantBufferData), 
                                              &m_CombineShader.m_CombineShaderConstantBufferData,
                                              &m_CombineShader.m_pCombineShaderConstantBuffer ) )
            return FALSE;                                              
    }

    // preview shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_PreviewShader.Shader.Compile( "PreviewShaderVS.cso", "PreviewShaderPS.cso",
                                                     pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;		
    }
    return TRUE;
}



//--------------------------------------------------------------------------------------
// Name: CreateFBO()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateFBO( UINT32 nWidth, UINT32 nHeight, DXGI_FORMAT nFormat, D3D11_FILTER nFilter, BOOL bDepth, FrameBufferObject** ppFBO )
{
    if (*ppFBO)
    {
        DestroyFBO( *ppFBO );
        *ppFBO = NULL;
    }


    (*ppFBO) = new FrameBufferObject;
    (*ppFBO)->m_nWidth  = nWidth;
    (*ppFBO)->m_nHeight = nHeight;

    D3D11_TEXTURE2D_DESC textureDesc;        
    ZeroMemory( &textureDesc, sizeof(textureDesc) );
    textureDesc.Width = nWidth;
    textureDesc.Height = nHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = nFormat;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Render target view
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    // Shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        
    // Sampler description
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = nFilter;
    samplerDesc.MaxAnisotropy = 0;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.BorderColor[0] = 0.0f;
    samplerDesc.BorderColor[1] = 0.0f;
    samplerDesc.BorderColor[2] = 0.0f;
    samplerDesc.BorderColor[3] = 0.0f;

    // Create Texture 0
    if ( FAILED(D3DDevice()->CreateTexture2D( &textureDesc, NULL, &(*ppFBO)->m_pTexture0 ) ) )
    {
        return FALSE;
    }

    // Create Render Target View 0
    if ( FAILED(D3DDevice()->CreateRenderTargetView( (*ppFBO)->m_pTexture0.Get(), 
                                                     &renderTargetViewDesc, 
                                                     &(*ppFBO)->m_pTexture0RenderTargetView ) ) )
    {
        return FALSE;
    }

    // Create Render Shader Resource View 0
    if ( FAILED(D3DDevice()->CreateShaderResourceView( (*ppFBO)->m_pTexture0.Get(),
                                                       &shaderResourceViewDesc,
                                                       &(*ppFBO)->m_pTexture0ShaderResourceView ) ) )
    {
        return FALSE;
    }

    // Create Sampler State 0
    if ( FAILED(D3DDevice()->CreateSamplerState( &samplerDesc, &(*ppFBO)->m_pTexture0Sampler ) ) )
    {
        return FALSE;
    }
    
    // Create a depth/stencil texture for the depth attachment
    if ( bDepth )
    {
        textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        if ( FAILED(D3DDevice()->CreateTexture2D( &textureDesc, NULL, &(*ppFBO)->m_pDepthTexture ) ) )
            return FALSE;

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
        depthStencilViewDesc.Format = textureDesc.Format;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;
    
        if ( FAILED(D3DDevice()->CreateDepthStencilView( (*ppFBO)->m_pDepthTexture.Get(), 
                                                         &depthStencilViewDesc, 
                                                         &(*ppFBO)->m_pDepthStencilView ) ) )
            return FALSE;    
    }

    return TRUE;	
}

//--------------------------------------------------------------------------------------
// Name: DestroyFBO()
// Desc: Destroy the FBO
//--------------------------------------------------------------------------------------
VOID CSample::DestroyFBO( FrameBufferObject* pFBO )
{
    delete pFBO;
}


//--------------------------------------------------------------------------------------
// Name: BeginFBO()
// Desc: Bind objects for rendering to the frame buffer object
//--------------------------------------------------------------------------------------
VOID CSample::BeginFBO( FrameBufferObject* pFBO )
{
    ID3D11RenderTargetView* pRTViews[1] = { pFBO->m_pTexture0RenderTargetView.Get() };                                             
    D3DDeviceContext()->OMSetRenderTargets( 1, pRTViews, pFBO->m_pDepthStencilView.Get() );
    
    // Set the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(pFBO->m_nWidth);
    viewport.Height = static_cast<float>(pFBO->m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);
}


//--------------------------------------------------------------------------------------
// Name: EndFBO()
// Desc: Restore rendering back to the primary frame buffer
//--------------------------------------------------------------------------------------
VOID CSample::EndFBO( FrameBufferObject* pFBO )
{
    // Reset to default window framebuffer
    ID3D11RenderTargetView* pRTViews[1] = { m_windowRenderTargetView.Get() };
    D3DDeviceContext()->OMSetRenderTargets( 1, pRTViews, m_windowDepthStencilView.Get() );

    // Reset the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);
}

//--------------------------------------------------------------------------------------
// Name: RenderScreenAlignedQuad
// Desc: Draws a full-screen rectangle
//--------------------------------------------------------------------------------------
VOID CSample::RenderScreenAlignedQuad()
{

    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );

    m_pQuadVertexBuffer->Bind( 0 );
    m_pQuadIndexBuffer->Bind( 0 );
    
    FrmDrawIndexedVertices( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6, sizeof(UINT32), 0 );

    D3DDeviceContext()->OMSetDepthStencilState( m_DefaultDepthStencilState.Get(), 0 );
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    m_ShouldRotate = TRUE;
    m_Preview = false;

    // Create the font
    if( FALSE == m_Font.Create( "Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    if( FALSE == g_Resource.LoadFromFile( "Textures.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = g_Resource.GetTexture( "Logo" );
    
    // Load the packed resources
    if( FALSE == g_RoomResources.LoadFromFile( "Room.pak" ) )
        return FALSE;

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5,
        m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_BrightPassShader.BrightPassThreshold, (char *)"Bright pass threshold" );
    m_UserInterface.AddFloatVariable( &m_BrightPassShader.BrightPassOffset, (char *)"Bright pass offset" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.GaussSpread, (char *)"Gauss filter spread" );
    m_UserInterface.AddFloatVariable( &m_fDownsizeRatio, (char *)"Downsize ratio" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Decrease bright pass threshold" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Increase bright pass threshold" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Decrease bright pass offset" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Increase bright pass offset" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Decrease downsize ratio" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Increase downsize ratio" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    if( FALSE == m_RoomMesh.Load( "Map3.mesh" ) )
        return FALSE;

    m_CameraPos = FRMVECTOR3( 6.2f, 2.0f, 0.0f );
    float Split = 1.1f;
    float yAdjust = -0.05f;
    m_Object.ModelScale = 1.0f;
        
    if( FALSE == m_RoomMesh.MakeDrawable( &g_RoomResources ) )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
        
    m_AmbientLight = FRMVECTOR4( 0.02f, 0.02f, 0.02f, 0.02f );
        
    // Set up the objects
    m_Object.Drawable = &m_RoomMesh;
    m_Object.Position.y += yAdjust;
 
    // State blocks
    D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    D3DDevice()->CreateRasterizerState(&rdesc, &m_DefaultRasterizerState);
    
    rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    rdesc.CullMode = D3D11_CULL_NONE;
    D3DDevice()->CreateRasterizerState(&rdesc, &m_CullDisabledRasterizerState);
    
    rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    rdesc.FrontCounterClockwise = TRUE; // Change the winding direction to match GL    
    D3DDevice()->CreateRasterizerState(&rdesc, &m_RasterizerState);
    D3DDeviceContext()->RSSetState(m_RasterizerState.Get());

    D3D11_DEPTH_STENCIL_DESC dsdesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DefaultDepthStencilState);
    dsdesc.DepthEnable = false;
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DisabledDepthStencilState);

    D3D11_BLEND_DESC bdesc = CD3D11_BLEND_DESC(D3D11_DEFAULT);
    D3DDevice()->CreateBlendState(&bdesc, &m_DisabledBlendState);
    bdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bdesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bdesc.RenderTarget[0].BlendEnable = TRUE;
    D3DDevice()->CreateBlendState(&bdesc, &m_EnabledBlendState);

    // Quad VB/IB
    UINT32 indices[6] = { 0, 1, 2, 2, 1, 3 };
    FLOAT32 Quad[] =
    {
        +1.0, +1.0f, 1.0f, 0.0f,
        -1.0, +1.0f, 0.0f, 0.0f,
        +1.0, -1.0f, 1.0f, 1.0f,
        -1.0, -1.0f, 0.0f, 1.0f,
    };

    if( FALSE == FrmCreateVertexBuffer( 4, sizeof(FLOAT32) * 4, &Quad[0], &m_pQuadVertexBuffer ) )
        return FALSE;

    if( FALSE == FrmCreateIndexBuffer( 6, sizeof(UINT32), &indices, &m_pQuadIndexBuffer ) )
        return FALSE;

    if( FALSE == FrmCreateVertexBuffer( 4, sizeof(FLOAT32) * 4, NULL, &m_pDynamicQuadVertexBuffer ) )
        return FALSE;


    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    FLOAT32 FOVy = FrmRadians( 86 );
    FLOAT32 zNear = 0.25f;
    FLOAT32 zFar = 20.0f;
    m_matProj = FrmMatrixPerspectiveFovRH( FOVy, fAspect, zNear, zFar );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    UINT32 downsizeWidth = (UINT32) (((FLOAT32)m_nWidth) * m_fDownsizeRatio);
    UINT32 downsizeHeight = (UINT32) (((FLOAT32)m_nHeight) * m_fDownsizeRatio);
    
    if( FALSE == CreateFBO( m_nWidth, m_nHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D11_FILTER_MIN_MAG_MIP_POINT, TRUE, &m_SharpRT ) )
        return FALSE;
    if( FALSE == CreateFBO( downsizeWidth, downsizeHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D11_FILTER_MIN_MAG_MIP_POINT, FALSE, &m_QuarterRT ) )
        return FALSE;
    if( FALSE == CreateFBO( downsizeWidth, downsizeHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D11_FILTER_MIN_MAG_MIP_POINT, FALSE, &m_BlurredRT ) )
        return FALSE;
    if( FALSE == CreateFBO( downsizeWidth, downsizeHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D11_FILTER_MIN_MAG_MIP_POINT, FALSE, &m_ScratchRT ) )
        return FALSE;
    if( FALSE == CreateFBO( 64, 64, DXGI_FORMAT_R32_FLOAT, D3D11_FILTER_MIN_MAG_MIP_POINT, FALSE, &m_64x64RT ) )
        return FALSE;
    if( FALSE == CreateFBO( 16, 16, DXGI_FORMAT_R32_FLOAT, D3D11_FILTER_MIN_MAG_MIP_POINT, FALSE, &m_16x16RT ) )
        return FALSE;
    if( FALSE == CreateFBO( 4, 4, DXGI_FORMAT_R32_FLOAT, D3D11_FILTER_MIN_MAG_MIP_POINT, FALSE, &m_4x4RT ) )
        return FALSE;
    if( FALSE == CreateFBO( 1, 1, DXGI_FORMAT_R32_FLOAT, D3D11_FILTER_MIN_MAG_MIP_POINT, FALSE, &m_1x1RT ) )
        return FALSE;
    if( FALSE == CreateFBO( 1, 1, DXGI_FORMAT_R32_FLOAT, D3D11_FILTER_MIN_MAG_MIP_POINT, FALSE, &m_PreviousAverageLumRT ) )
        return FALSE;
    if( FALSE == CreateFBO( 1, 1, DXGI_FORMAT_R32_FLOAT, D3D11_FILTER_MIN_MAG_MIP_POINT, FALSE, &m_LastAverageLumRT ) )
        return FALSE;
    if( FALSE == CreateFBO( downsizeWidth, downsizeHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, D3D11_FILTER_MIN_MAG_MIP_POINT, FALSE, &m_BrightPassRT ) )
        return FALSE;
        

    m_BlurShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_BlurredRT->m_nWidth, 1.0f / (FLOAT32)m_BlurredRT->m_nHeight );

    BeginFBO(m_LastAverageLumRT);

    // Clear the last average lum RT
    const FLOAT clearColor[4] = { 0.04f, 0.04f, 0.04f, 1.0f };
    D3DDeviceContext()->ClearRenderTargetView( m_LastAverageLumRT->m_pTexture0RenderTargetView.Get(), clearColor );
    
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_pLogoTexture ) m_pLogoTexture->Release();

    if( m_pQuadVertexBuffer ) m_pQuadVertexBuffer->Release();
    if( m_pQuadIndexBuffer ) m_pQuadIndexBuffer->Release();
    if( m_pDynamicQuadVertexBuffer ) m_pDynamicQuadVertexBuffer->Release();


    DestroyFBO( m_SharpRT);
    DestroyFBO( m_BlurredRT);
    DestroyFBO( m_ScratchRT);
    DestroyFBO(m_64x64RT);
    DestroyFBO(m_16x16RT);
    DestroyFBO(m_4x4RT);
    DestroyFBO(m_1x1RT);
    DestroyFBO(m_LastAverageLumRT);
    DestroyFBO(m_PreviousAverageLumRT);
    DestroyFBO(m_BrightPassRT);
    DestroyFBO(m_QuarterRT);
    
    // release resource
    m_RoomMesh.Destroy();
    g_Resource.Destroy();
    g_RoomResources.Destroy();
    
    // Free objects
    m_Object.Destroy();
    
    // Free shader memory
    m_MaterialShader.Destroy();
    m_CombineShader.Destroy();
    m_DownsizeShader.Destroy();
    m_BlurShader.Destroy();
    m_LuminanceShader.Destroy();
    m_AvgLuminanceShader.Destroy();
    m_AdaptLuminanceShader.Destroy();
    m_BrightPassShader.Destroy();
    m_CopyRTShader.Destroy();

    m_PreviewShader.Destroy();
}

//--------------------------------------------------------------------------------------
// Name: GetCameraPos27()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos( FLOAT32 fTime )
{
    FRMVECTOR3 BaseOffset = FRMVECTOR3( -0.5f, 2.0f, 0.0f );
    FRMVECTOR3 CurPosition = FRMVECTOR3( 3.0f * FrmCos( 0.5f * fTime ), 0.0f, 0.0f );
    return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // get the current time
    FLOAT32 ElapsedTime = m_Timer.GetFrameElapsedTime();

    // process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    // toggle rotation
    if( nPressedButtons & INPUT_KEY_8 )
    {
        m_ShouldRotate = 1 - m_ShouldRotate;
    }

    // toggle previews
    if( nPressedButtons & INPUT_KEY_9 )
    {
        m_Preview = 1 - m_Preview;
    }

    if( nButtons & INPUT_KEY_1 )
    {
        m_BrightPassShader.BrightPassThreshold -= 2.0f * ElapsedTime;
        m_BrightPassShader.BrightPassThreshold = max( 0.0f, m_BrightPassShader.BrightPassThreshold );
    }

    if( nButtons & INPUT_KEY_2 )
    {
        m_BrightPassShader.BrightPassThreshold += 2.0f * ElapsedTime;
        m_BrightPassShader.BrightPassThreshold = min( 100.0f, m_BrightPassShader.BrightPassThreshold );
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_BrightPassShader.BrightPassOffset -= 2.0f * ElapsedTime;
        m_BrightPassShader.BrightPassOffset = max( 0.0f, m_BrightPassShader.BrightPassOffset );
    }

    if( nButtons & INPUT_KEY_4 )
    {
        m_BrightPassShader.BrightPassOffset += 2.0f * ElapsedTime;
        m_BrightPassShader.BrightPassOffset = min( 100.0f, m_BrightPassShader.BrightPassOffset );
    }

    if( nButtons & INPUT_KEY_5 )
    {
        m_fDownsizeRatio -= 2.0f * ElapsedTime;
        m_fDownsizeRatio = max( 0.1f, m_fDownsizeRatio );
        Resize();
    }

    if( nButtons & INPUT_KEY_6 )
    {
        m_fDownsizeRatio += 2.0f * ElapsedTime;
        m_fDownsizeRatio = min( 0.9f, m_fDownsizeRatio );
        Resize();
    }


    // update light position
    m_LightPos = FRMVECTOR3( 5.3f, 2.0f, 0.0f );

    // animate the camera
    static FLOAT32 TotalTime = 0.0f;
    if( m_ShouldRotate )
    {
        static FLOAT32 CameraSpeed = 1.0f;
        TotalTime += ElapsedTime * CameraSpeed;
    }

    // build camera transforms
    FRMVECTOR3 UpDir = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 LookPos;

    m_CameraPos   = GetCameraPos( TotalTime );
    LookPos       = m_CameraPos + FRMVECTOR3( 1.0f, 0.0f, 0.0f );
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    m_AdaptLuminanceShader.ElapsedTime = ElapsedTime;
    m_AdaptLuminanceShader.Lambda = 17000.0f;

    // update objects
    m_Object.Update( ElapsedTime, m_ShouldRotate );
}

//--------------------------------------------------------------------------------------
// Name: GaussianDistribution()
// Desc: 
//--------------------------------------------------------------------------------------
FLOAT32 GaussianDistribution( FLOAT32 X, FLOAT32 Y, FLOAT32 Rho )
{
    FLOAT32 Gauss = 1.0f / sqrtf( 2.0f * FRM_PI * Rho * Rho );
    Gauss *= expf( -( X * X + Y * Y ) / ( 2.0f * Rho * Rho ) );

    return Gauss;
}

//--------------------------------------------------------------------------------------
// Name: SetupGaussWeights()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::SetupGaussWeights()
{
    FLOAT32 GaussSum = 0.0f;
    for( int i = 0; i < 4; i++ )
    {
        // normalize to -1..1
        FLOAT32 X = ( FLOAT32( i ) / 3.0f );
        X = ( X - 0.5f ) * 2.0f;

        // spread is tunable
        X *= m_CombineShader.GaussSpread;

        m_BlurShader.m_BlurConstantBufferData.GaussWeight[i] = GaussianDistribution( X, 0.0f, m_CombineShader.GaussRho );

        GaussSum += m_BlurShader.m_BlurConstantBufferData.GaussWeight[i];
    }

    m_BlurShader.m_BlurConstantBufferData.GaussInvSum = ( 1.0f / GaussSum );
}

//--------------------------------------------------------------------------------------
// Name: GaussBlur()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GaussBlur( FrameBufferObject* InputTargetRT, FrameBufferObject* BlurredTargetRT )
{
    // Blur in the X direction
    {
        BeginFBO( m_ScratchRT );

        // Discard color contents
        D3DDeviceContext()->DiscardView( m_ScratchRT->m_pTexture0RenderTargetView.Get() );

        D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
        float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT32 sampleMask = 0xffffffff;
        D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        
        D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );		

        m_BlurShader.Shader.Bind();
     
        FRMVECTOR2 StepSizeX = FRMVECTOR2( m_BlurShader.StepSize.x * m_CombineShader.GaussSpread, 0.0f );     
        m_BlurShader.m_BlurConstantBufferData.StepSize = StepSizeX;
        m_BlurShader.m_pBlurConstantBuffer->Update( &m_BlurShader.m_BlurConstantBufferData );
        m_BlurShader.m_pBlurConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );

        // Bind textures
        D3DDeviceContext()->PSSetShaderResources( 0, 1, InputTargetRT->m_pTexture0ShaderResourceView.GetAddressOf() );
        D3DDeviceContext()->PSSetSamplers( 0, 1, InputTargetRT->m_pTexture0Sampler.GetAddressOf() );
        
        RenderScreenAlignedQuad();

        // Unbind textures
        ID3D11ShaderResourceView* unused[2] = { NULL, NULL };
        D3DDeviceContext()->PSSetShaderResources( 0, 2, unused );
    }

    // Then blur in the Y direction
    {
        BeginFBO( BlurredTargetRT );

        // Discard color contents
        D3DDeviceContext()->DiscardView( BlurredTargetRT->m_pTexture0RenderTargetView.Get() );

        m_BlurShader.Shader.Bind();

        FRMVECTOR2 StepSizeY = FRMVECTOR2( 0.0f, m_BlurShader.StepSize.y * m_CombineShader.GaussSpread );
        m_BlurShader.m_BlurConstantBufferData.StepSize = StepSizeY;
        m_BlurShader.m_pBlurConstantBuffer->Update( &m_BlurShader.m_BlurConstantBufferData );
        m_BlurShader.m_pBlurConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );

        // Bind textures
        D3DDeviceContext()->PSSetShaderResources( 0, 1, m_ScratchRT->m_pTexture0ShaderResourceView.GetAddressOf() );
        D3DDeviceContext()->PSSetSamplers( 0, 1, m_ScratchRT->m_pTexture0Sampler.GetAddressOf() );

        RenderScreenAlignedQuad();

        // Unbind textures
        ID3D11ShaderResourceView* unused[2] = { NULL, NULL };
        D3DDeviceContext()->PSSetShaderResources( 0, 2, unused );
    }
}

//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::DrawObject( SimpleObject* Object )
{
    FRMMATRIX4X4    MatModel;
    FRMMATRIX4X4    MatModelView;
    FRMMATRIX4X4    MatModelViewProj;
    FRMMATRIX4X4    MatNormal;
    FRMVECTOR3      Eye = m_CameraPos;

    if ( !Object || !Object->Drawable )
        return;

    MatModel         = Object->ModelMatrix;
    MatModelView     = FrmMatrixMultiply( MatModel, m_matView );
    MatModelViewProj = FrmMatrixMultiply( MatModelView, m_matProj );
    MatNormal        = FrmMatrixInverse( MatModel );

    m_MaterialShader.Shader.Bind();

    // Update constants
    m_MaterialShader.m_MaterialConstantBufferData.MatModelViewProj = MatModelViewProj;
    m_MaterialShader.m_MaterialConstantBufferData.MatModel = MatModel;
    m_MaterialShader.m_MaterialConstantBufferData.EyePos = FRMVECTOR4( Eye, 1.0 );
    m_MaterialShader.m_MaterialConstantBufferData.LightPos = FRMVECTOR4( m_LightPos, 1.0 );
    m_MaterialShader.m_MaterialConstantBufferData.AmbientColor = FRMVECTOR4( m_AmbientLight );

    m_MaterialShader.m_pMaterialConstantBuffer->Update( &m_MaterialShader.m_MaterialConstantBufferData );
    m_MaterialShader.m_pMaterialConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );
    
    Object->Drawable->Render( &m_MaterialShader.Shader );
}

//--------------------------------------------------------------------------------------
// Name: RenderScreenAlignedQuad
// Desc: Draws a full-screen rectangle
//--------------------------------------------------------------------------------------
VOID CSample::RenderScreenAlignedQuad( FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight )
{
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );

    const FLOAT32 Quad[] =
    {
        TopLeft.x, BottomRight.y,		0.0f, 0.0f,
        BottomRight.x, BottomRight.y,	1.0f, 0.0f,
        TopLeft.x, TopLeft.y,			0.0f, 1.0f,
        BottomRight.x, TopLeft.y,		1.0f, 1.0f,
        
    };

    VOID* pData = m_pDynamicQuadVertexBuffer->Map( D3D11_MAP_WRITE_DISCARD );
    FrmMemcpy( pData, Quad, sizeof(Quad) );
    m_pDynamicQuadVertexBuffer->Unmap();

    m_pDynamicQuadVertexBuffer->Bind( 0 );
    m_pQuadIndexBuffer->Bind( 0 );
    
    FrmDrawIndexedVertices( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6, sizeof(UINT32), 0 );

    D3DDeviceContext()->OMSetDepthStencilState( m_DefaultDepthStencilState.Get(), 0 );

}

VOID CSample::PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, FrameBufferObject* pTexture )
{
    TopLeft.y = -TopLeft.y;
    Dims.y = -Dims.y;

    // Reset to default window framebuffer
    ID3D11RenderTargetView* pRTViews[1] = { m_windowRenderTargetView.Get() };
    D3DDeviceContext()->OMSetRenderTargets( 1, pRTViews, m_windowDepthStencilView.Get() );

    // Reset the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);

    m_PreviewShader.Shader.Bind();

    D3DDeviceContext()->PSSetShaderResources( 0, 1, pTexture->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, pTexture->m_pTexture0Sampler.GetAddressOf() );    

    RenderScreenAlignedQuad( TopLeft, TopLeft + Dims );
}

//--------------------------------------------------------------------------------------
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::DrawScene()
{
    BeginFBO( m_SharpRT );

    // Discard color and clear depth
    D3DDeviceContext()->DiscardView( m_SharpRT->m_pTexture0RenderTargetView.Get() );
    D3DDeviceContext()->ClearDepthStencilView( m_SharpRT->m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

    D3DDeviceContext()->RSSetState( m_DefaultRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DefaultDepthStencilState.Get(), 0 );

    DrawObject( &m_Object );
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Generate64x64Lum( FrameBufferObject* SourceRT )
{
    BeginFBO( m_64x64RT );

    // Discard color contents
    D3DDeviceContext()->DiscardView( m_64x64RT->m_pTexture0RenderTargetView.Get() );

    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );
    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    UINT32 sampleMask = 0xffffffff;
    D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        
            
    m_LuminanceShader.Shader.Bind();
    m_LuminanceShader.m_LuminanceShaderConstantBufferData.g_StepSize = FRMVECTOR2(1.0f / SourceRT->m_nWidth, 1.0f / SourceRT->m_nHeight);
    m_LuminanceShader.m_pLuminanceShaderConstantBuffer->Update( &m_LuminanceShader.m_LuminanceShaderConstantBufferData );
    m_LuminanceShader.m_pLuminanceShaderConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );

    // Bind textures
    D3DDeviceContext()->PSSetShaderResources( 0, 1, SourceRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, SourceRT->m_pTexture0Sampler.GetAddressOf() );
    
    RenderScreenAlignedQuad();
    
    // Unbind textures
    ID3D11ShaderResourceView* unused[2] = { NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 2, unused );
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Generate16x16Lum( FrameBufferObject* SourceRT )
{
    BeginFBO( m_16x16RT );

    // Discard color contents
    D3DDeviceContext()->DiscardView( m_16x16RT->m_pTexture0RenderTargetView.Get() );

    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );
    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    UINT32 sampleMask = 0xffffffff;
    D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        

    m_AvgLuminanceShader.Shader.Bind();
    m_AvgLuminanceShader.m_LuminanceShaderConstantBufferData.g_StepSize = FRMVECTOR2(1.0f / SourceRT->m_nWidth, 1.0f / SourceRT->m_nHeight);
    m_AvgLuminanceShader.m_pLuminanceShaderConstantBuffer->Update( &m_AvgLuminanceShader.m_LuminanceShaderConstantBufferData );
    m_AvgLuminanceShader.m_pLuminanceShaderConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );

    // Bind textures
    D3DDeviceContext()->PSSetShaderResources( 0, 1, SourceRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, SourceRT->m_pTexture0Sampler.GetAddressOf() );

    RenderScreenAlignedQuad();

    // Unbind textures
    ID3D11ShaderResourceView* unused[2] = { NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 2, unused );
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Generate4x4Lum( FrameBufferObject* SourceRT )
{
    BeginFBO( m_4x4RT );

    // Discard color contents
    D3DDeviceContext()->DiscardView( m_4x4RT->m_pTexture0RenderTargetView.Get() );
    
    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );
    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    UINT32 sampleMask = 0xffffffff;
    D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        

    m_AvgLuminanceShader.Shader.Bind();
    m_AvgLuminanceShader.m_LuminanceShaderConstantBufferData.g_StepSize = FRMVECTOR2(1.0f / SourceRT->m_nWidth, 1.0f / SourceRT->m_nHeight);
    m_AvgLuminanceShader.m_pLuminanceShaderConstantBuffer->Update( &m_AvgLuminanceShader.m_LuminanceShaderConstantBufferData );
    m_AvgLuminanceShader.m_pLuminanceShaderConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );

    // Bind textures
    D3DDeviceContext()->PSSetShaderResources( 0, 1, SourceRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, SourceRT->m_pTexture0Sampler.GetAddressOf() );

    RenderScreenAlignedQuad();

    // Unbind textures
    ID3D11ShaderResourceView* unused[2] = { NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 2, unused );
}

//--------------------------------------------------------------------------------------
// Name: Generate64x64Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Generate1x1Lum( FrameBufferObject* SourceRT )
{
    BeginFBO( m_1x1RT );

    // Discard color contents
    D3DDeviceContext()->DiscardView( m_1x1RT->m_pTexture0RenderTargetView.Get() );

    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );
    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    UINT32 sampleMask = 0xffffffff;
    D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        

    m_AvgLuminanceShader.Shader.Bind();
    m_AvgLuminanceShader.m_LuminanceShaderConstantBufferData.g_StepSize = FRMVECTOR2(1.0f / SourceRT->m_nWidth, 1.0f / SourceRT->m_nHeight);
    m_AvgLuminanceShader.m_pLuminanceShaderConstantBuffer->Update( &m_AvgLuminanceShader.m_LuminanceShaderConstantBufferData );
    m_AvgLuminanceShader.m_pLuminanceShaderConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );

    // Bind textures
    D3DDeviceContext()->PSSetShaderResources( 0, 1, SourceRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, SourceRT->m_pTexture0Sampler.GetAddressOf() );

    RenderScreenAlignedQuad();

    // Unbind textures
    ID3D11ShaderResourceView* unused[2] = { NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 2, unused );
}

//--------------------------------------------------------------------------------------
// Name: GenerateAdaptLum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::GenerateAdaptLum( FrameBufferObject* CurrLumRT, FrameBufferObject* PrevLumRT )
{
    BeginFBO(m_LastAverageLumRT);

    // Discard color contents
    D3DDeviceContext()->DiscardView( m_LastAverageLumRT->m_pTexture0RenderTargetView.Get() );

    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );
    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    UINT32 sampleMask = 0xffffffff;
    D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        


    m_AdaptLuminanceShader.Shader.Bind();

    m_AdaptLuminanceShader.m_AdaptLuminanceConstantBufferData.ElapsedTime = m_AdaptLuminanceShader.ElapsedTime;
    m_AdaptLuminanceShader.m_AdaptLuminanceConstantBufferData.Lambda = m_AdaptLuminanceShader.Lambda;
    m_AdaptLuminanceShader.m_pAdaptLuminanceConstantBuffer->Update( &m_AdaptLuminanceShader.m_AdaptLuminanceConstantBufferData );
    m_AdaptLuminanceShader.m_pAdaptLuminanceConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );

    // Bind textures
    D3DDeviceContext()->PSSetShaderResources( 0, 1, CurrLumRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, CurrLumRT->m_pTexture0Sampler.GetAddressOf() );

    D3DDeviceContext()->PSSetShaderResources( 1, 1, PrevLumRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 1, 1, PrevLumRT->m_pTexture0Sampler.GetAddressOf() );

    RenderScreenAlignedQuad();

    // Unbind textures
    ID3D11ShaderResourceView* unused[2] = { NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 2, unused );
}

//--------------------------------------------------------------------------------------
// Name: Copy1x1Lum()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Copy1x1Lum( FrameBufferObject* CopyToLumRT, FrameBufferObject* CopyFromLumRT )
{
    BeginFBO(CopyToLumRT);

    D3DDeviceContext()->DiscardView( CopyToLumRT->m_pTexture0RenderTargetView.Get() );

    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );
    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    UINT32 sampleMask = 0xffffffff;
    D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        

    m_CopyRTShader.Shader.Bind();

    // Bind textures
    D3DDeviceContext()->PSSetShaderResources( 0, 1, CopyFromLumRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, CopyFromLumRT->m_pTexture0Sampler.GetAddressOf() );
    
    RenderScreenAlignedQuad();

    // Unbind textures
    ID3D11ShaderResourceView* unused[2] = { NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 2, unused );
}

//--------------------------------------------------------------------------------------
// Name: BrightPass()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::BrightPass( FrameBufferObject* CopyToLumRT, FrameBufferObject* ScratchRT, FrameBufferObject* currLumRT )
{
    BeginFBO(CopyToLumRT);

    D3DDeviceContext()->DiscardView( CopyToLumRT->m_pTexture0RenderTargetView.Get() );

    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );
    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    UINT32 sampleMask = 0xffffffff;
    D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        

    m_BrightPassShader.Shader.Bind();

    m_BrightPassShader.m_BrightPassConstantBufferData.MiddleGray = m_BrightPassShader.MiddleGray;
    m_BrightPassShader.m_BrightPassConstantBufferData.BrightPassWhite = m_BrightPassShader.BrightPassWhite;
    m_BrightPassShader.m_BrightPassConstantBufferData.BrightPassThreshold = m_BrightPassShader.BrightPassThreshold;
    m_BrightPassShader.m_BrightPassConstantBufferData.BrightPassOffset = m_BrightPassShader.BrightPassOffset;
    m_BrightPassShader.m_pBrightPassConstantBuffer->Update( &m_BrightPassShader.m_BrightPassConstantBufferData );
    m_BrightPassShader.m_pBrightPassConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );

    // Bind textures
    D3DDeviceContext()->PSSetShaderResources( 0, 1, ScratchRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, ScratchRT->m_pTexture0Sampler.GetAddressOf() );

    D3DDeviceContext()->PSSetShaderResources( 1, 1, currLumRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 1, 1, currLumRT->m_pTexture0Sampler.GetAddressOf() );	

    RenderScreenAlignedQuad();

    // Unbind textures
    ID3D11ShaderResourceView* unused[2] = { NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 2, unused );
}


//--------------------------------------------------------------------------------------
// Name: CreateBlurredScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::CreateBlurredImage()
{
    // Part 1: downsize the framebuffer to a smaller render target
    {
        BeginFBO(m_QuarterRT);

        // Discard color contents
        D3DDeviceContext()->DiscardView( m_QuarterRT->m_pTexture0RenderTargetView.Get() );

        D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
        D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );
        float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT32 sampleMask = 0xffffffff;
        D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        

        m_DownsizeShader.Shader.Bind();
        m_DownsizeShader.m_DownsizeShaderConstantBufferData.g_StepSize = FRMVECTOR2(1.0f / m_SharpRT->m_nWidth, 1.0f / m_SharpRT->m_nHeight);
        m_DownsizeShader.m_pDownsizeShaderConstantBuffer->Update( &m_DownsizeShader.m_DownsizeShaderConstantBufferData );
        m_DownsizeShader.m_pDownsizeShaderConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );
        
        // Bind textures
        D3DDeviceContext()->PSSetShaderResources( 0, 1, m_SharpRT->m_pTexture0ShaderResourceView.GetAddressOf() );
        D3DDeviceContext()->PSSetSamplers( 0, 1, m_SharpRT->m_pTexture0Sampler.GetAddressOf() );

        RenderScreenAlignedQuad();

        // Unbind textures
        ID3D11ShaderResourceView* unused[2] = { NULL, NULL };
        D3DDeviceContext()->PSSetShaderResources( 0, 2, unused );
    }

    // Part 2: blur
    GaussBlur( m_QuarterRT, m_BlurredRT );
    
    // generate 64x64 luminance render target
    Generate64x64Lum( m_BlurredRT );
    
    // generate 16x16 luminance render target
    Generate16x16Lum( m_64x64RT );

    // generate 4x4 luminance render target
    Generate4x4Lum( m_16x16RT );

    // generate 1x1 luminance render target
    Generate1x1Lum( m_4x4RT );

    // adapt luminance
    GenerateAdaptLum( m_1x1RT, m_PreviousAverageLumRT );

    // copy the result of this operation into another 1x1 render target to store it for the next frame
    Copy1x1Lum( m_PreviousAverageLumRT, m_LastAverageLumRT );

    // bright pass filter
    BrightPass( m_BrightPassRT, m_QuarterRT, m_LastAverageLumRT );

    // bloom
    GaussBlur( m_BrightPassRT, m_QuarterRT );
    GaussBlur( m_QuarterRT, m_BlurredRT ); 
}

//--------------------------------------------------------------------------------------
// Name: DrawCombinedScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::DrawCombinedScene()
{
    // Reset to default window framebuffer - no depth buffer since we are rendering a full screen quad
    ID3D11RenderTargetView* pRTViews[1] = { m_windowRenderTargetView.Get() };
    D3DDeviceContext()->OMSetRenderTargets( 1, pRTViews, NULL );

    // Discard the color buffer since we will hit every pixel - this is faster than a clear
    D3DDeviceContext()->DiscardView( m_windowRenderTargetView.Get() );

    // Reset the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);


    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );
    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    UINT32 sampleMask = 0xffffffff;
    D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        

    m_CombineShader.Shader.Bind();

    m_CombineShader.m_CombineShaderConstantBufferData.MiddleGray = m_CombineShader.MiddleGray;
    m_CombineShader.m_CombineShaderConstantBufferData.White = m_CombineShader.White;
    m_CombineShader.m_pCombineShaderConstantBuffer->Update( &m_CombineShader.m_CombineShaderConstantBufferData );
    m_CombineShader.m_pCombineShaderConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );

    // Bind textures
    D3DDeviceContext()->PSSetShaderResources( 0, 1, m_SharpRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, m_SharpRT->m_pTexture0Sampler.GetAddressOf() );

    D3DDeviceContext()->PSSetShaderResources( 1, 1, m_LastAverageLumRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 1, 1, m_LastAverageLumRT->m_pTexture0Sampler.GetAddressOf() );	

    D3DDeviceContext()->PSSetShaderResources( 2, 1, m_BlurredRT->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 2, 1, m_BlurredRT->m_pTexture0Sampler.GetAddressOf() );	
    
    RenderScreenAlignedQuad();

    // Unbind textures
    ID3D11ShaderResourceView* unused[3] = { NULL, NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 3, unused );
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    // Draw regular stuff to the framebuffer
    DrawScene();

    // Downsize framebuffer and blur
    CreateBlurredImage();

    // Recombine scene with depth of field
    DrawCombinedScene();

    // Show render target previews
    if( m_Preview )
    {
        FLOAT32 xPos = -0.95f;
        FLOAT32 yPos = -0.95f;
        FLOAT32 Width = 0.2f;
        FLOAT32 Height = 0.2f;

        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_BrightPassRT );
        xPos += ( Width + 0.1f );

        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_64x64RT );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_16x16RT );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_4x4RT );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_1x1RT );
        xPos += ( Width + 0.1f );


        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_LastAverageLumRT );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_PreviousAverageLumRT );
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
