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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>  

//--------------------------------------------------------------------------------------
// Name: LoadTGA()
// Desc: Helper function to load a 32-bit TGA image file
//--------------------------------------------------------------------------------------
BYTE* LoadTGA( const CHAR* strFileName, UINT32* pWidth, UINT32* pHeight, UINT32 nOutputPixelSize)
{
    struct TARGA_HEADER
    {
        BYTE   IDLength, ColormapType, ImageType;
        BYTE   ColormapSpecification[5];
        UINT16 XOrigin, YOrigin;
        UINT16 ImageWidth, ImageHeight;
        BYTE   PixelDepth;
        BYTE   ImageDescriptor;
    };
    
    static TARGA_HEADER header;
    
    // Read the TGA file
    FILE *file = NULL;
    fopen_s( &file, strFileName, "rb" );
    if( NULL == file )
        return NULL;

    fread( &header, sizeof(header), 1, file );
    UINT32 nPixelSize = header.PixelDepth / 8;
    (*pWidth)  = header.ImageWidth;
    (*pHeight) = header.ImageHeight;

    BYTE* pBits = new BYTE[ nPixelSize * header.ImageWidth * header.ImageHeight ];
    BYTE* pRGBABits = new BYTE[ 4 * header.ImageWidth * header.ImageHeight ];
    fread( pBits, nPixelSize, header.ImageWidth * header.ImageHeight, file );
    fclose( file );

    // Convert the image from BGRA to RGBA
    BYTE* p = pBits;
    BYTE* p2 = pRGBABits;
    for( UINT32 y=0; y<header.ImageHeight; y++ )
    {
        for( UINT32 x=0; x<header.ImageWidth; x++ )
        {
            BYTE temp = p[2]; p[2] = p[0]; p[0] = temp;
            
            p2[0] = p[0];
            p2[1] = p[1];
            p2[2] = p[2];
            p2[3] = 255;

            p += nPixelSize;
            p2 += 4;
        }
    }

    if (nOutputPixelSize == 3)
    {
        delete [] pRGBABits;
        return pBits;
    }
    else
    {
        delete [] pBits;
        return pRGBABits;
    }
}
//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "sRGB Texture" );
}

//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_pLogoTexture = NULL;
    m_psRGBTexture = NULL;
    m_pRGBTexture  = NULL;

    m_sRGBRenderTarget = NULL;

    m_pQuadIndexBuffer = NULL;
    m_pQuadVertexBuffer = NULL;

    m_nWidth = 800;
    m_nHeight = 480;

    m_bsRGBRenderTarget = TRUE;
}

//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    { 
        { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
    };
    const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

    // Create shader for sRGB
    {
        if( FALSE == m_sRGBShader.Compile( "sRGBTextureVS.cso", "sRGBTexturePS.cso",
                                           pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
    }

    // Create shader for sRGB final gamma correct step
    {
        if( FALSE == m_sRGBFinalGammaCorrectShader.Compile( "sRGBTextureVS.cso", "sRGBFinalGammaCorrectPS.cso",
                                                            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Load the packed resources
    if( FALSE == resource.LoadFromFile( "sRGB.pak" ) )
        return FALSE;

    // Load texture image data
	UINT32 nWidth, nHeight;
    BYTE*  pTextureData = LoadTGA( "Bridge_SRGB.tga", &nWidth, &nHeight, 4 );
    if( NULL == pTextureData )
          return FALSE;

    // Create sRGB version of texture
    if (FALSE == FrmCreateTexture(nWidth, nHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 
                                  0, pTextureData, nWidth * nHeight * 4, &m_psRGBTexture))
    {
        return FALSE;
    }

    // Create regular version of texture (non-sRGB)
    if (FALSE == FrmCreateTexture(nWidth, nHeight, 1, DXGI_FORMAT_R8G8B8A8_UNORM, 
                                  0, pTextureData, nWidth * nHeight * 4, &m_pRGBTexture))
    {
        return FALSE;
    }

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );
    
    // Create the font
    if( FALSE == m_Font.Create( "Tuffy12.pak" ) )
        return FALSE;

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, "HDR" ) )
        return FALSE;
        
    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5, 
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Toggle sRGB Render Target" );
    m_UserInterface.AddBoolVariable( &m_bsRGBRenderTarget, "sRGB Render Target");

    m_UserInterface.SetHeading("sRGB Texture (left) and RGB Texture (right) to sRGB Render Target");

    UINT32 indices[6] = { 0, 1, 2, 2, 1, 3 };
    if( FALSE == FrmCreateVertexBuffer( 4, sizeof(FLOAT32) * 4, NULL, &m_pQuadVertexBuffer ) )
        return FALSE;

    if( FALSE == FrmCreateIndexBuffer( 6, sizeof(UINT32), &indices, &m_pQuadIndexBuffer ) )
        return FALSE;


    // Initialize the shaders
    if( FALSE == InitShaders() )
    {
        return FALSE;
    }

    // Render state
    D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    rdesc.CullMode = D3D11_CULL_NONE;
    D3DDevice()->CreateRasterizerState(&rdesc, &m_RasterizerState);
    D3DDeviceContext()->RSSetState(m_RasterizerState.Get());

    D3D11_DEPTH_STENCIL_DESC dsdesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DefaultDepthStencilState);
    dsdesc.DepthEnable = false;
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DisabledDepthStencilState);


    return TRUE;

}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    // Create sRGB render target
    if( FALSE == CreateFBO( m_nWidth, m_nHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, &m_sRGBRenderTarget ) )
        return FALSE;
    
    // Initialize the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Free texture memory
    if( m_pLogoTexture ) m_pLogoTexture->Release();
    if( m_psRGBTexture ) m_psRGBTexture->Release();
    if( m_pRGBTexture )  m_pRGBTexture->Release();

    if ( m_pQuadVertexBuffer ) m_pQuadVertexBuffer->Release();
    if ( m_pQuadIndexBuffer ) m_pQuadIndexBuffer->Release();
    
    resource.Destroy();
    
    m_Font.Destroy();

    DestroyFBO( m_sRGBRenderTarget );
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

    if ( nPressedButtons & INPUT_KEY_1 )
    {
        m_bsRGBRenderTarget = !m_bsRGBRenderTarget;
            
        if ( m_bsRGBRenderTarget )
        {
            m_UserInterface.SetHeading("sRGB Texture (left) and RGB Texture (right) to sRGB Render Target");
        }
        else
        {
            m_UserInterface.SetHeading("sRGB Texture (left) and RGB Texture (right)");
        }

    }
}

//--------------------------------------------------------------------------------------
// Name: CreateFBO()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateFBO( UINT32 nWidth, UINT32 nHeight, DXGI_FORMAT nFormat, FrameBufferObject** ppFBO )
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
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
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
VOID CSample::RenderScreenAlignedQuad( FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight )
{   
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );		

    const FLOAT32 Quad[] =
    {
        TopLeft.x, BottomRight.y,		0.0f, 1.0f,
        BottomRight.x, BottomRight.y,	1.0f, 1.0f,
        TopLeft.x, TopLeft.y,			0.0f, 0.0f,
        BottomRight.x, TopLeft.y,		1.0f, 0.0f,
        
    };

    VOID* pData = m_pQuadVertexBuffer->Map( D3D11_MAP_WRITE_DISCARD );
    FrmMemcpy( pData, Quad, sizeof(Quad));
    m_pQuadVertexBuffer->Unmap();

    m_pQuadVertexBuffer->Bind( 0 );
    m_pQuadIndexBuffer->Bind( 0 );
    
    FrmDrawIndexedVertices( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6, sizeof(UINT32), 0 );

    D3DDeviceContext()->OMSetDepthStencilState( m_DefaultDepthStencilState.Get(), 0 );		
}

//--------------------------------------------------------------------------------------
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::DrawScene()
{
    // EXPLANATION:
    //
    // The values in the texture map are stored in gamma-corrected space, which
    // means each of the original linear values were raised to the (1/2.2) power.
    //
    // When we fetch from m_psRGBTexture, the HW automatically de-gammas to convert
    // to linear (pow(x, 2.2)). When we fetch from m_pRGBTexture, the value is unchanged, 
    // so it remains in gamma-corrected space.
    //
    // The rendering steps are:
    //
    // 1.
    // After fetching the texture m_psRGBTexture (on the left) fetch we are in LINEAR space
    // After fetching the texture m_pRGBTexture (on the right) we are in GAMMA-CORRECTED space
    //
    // We want to do all shader math in linear space, so the left case is the correct one.
    //
    // 2.
    // Now we are rendering to m_sRGBRenderTarget, an sRGB render target which on write
    // means that we have:
    // 
    // For the texture on the left, we are now back in GAMMA-CORRECTED space
    // For the texture on the right, we are now in GAMMA-CORRECTED^(1/2.2) (not a sensible space)
    //
    // 3. 
    // We draw the final sRGB quad to the screen, which because it is an sRGB ShaderResourceView,
    // will again de-gamma on fetch (raise to the 2.2 power).  Since we do want the image on the
    // left to remain in gamma-correct space, the sRGBFinalGammaCorrect.hlsl shader raises
    // to the (1/2.2) power to put us back in the original space.  Finally rendered to the screen is:
    //
    // For the texture on the left, we remain in GAMMA-CORRECTED space
    // For the texture on the right, we remain in GAMMA-CORRECTED^(1/2.2) space (not correct)

    if ( m_bsRGBRenderTarget )
    {
        BeginFBO( m_sRGBRenderTarget );

        // Clear the render target to a solid color, and reset the depth stencil.
        float clearColor[4] = { 0.071f, 0.04f, 0.561f, 1.0f };
        for (int i = 0; i < 4; i++)
        {
            clearColor[i] = pow(clearColor[i], 2.2f);
        }
        D3DDeviceContext()->ClearRenderTargetView( m_sRGBRenderTarget->m_pTexture0RenderTargetView.Get(), clearColor);
        D3DDeviceContext()->ClearDepthStencilView( m_sRGBRenderTarget->m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0, 0 );
    }
    else
    {
        // Bind the backbuffer as a render target
        D3DDeviceContext()->OMSetRenderTargets( 1, m_windowRenderTargetView.GetAddressOf(), m_windowDepthStencilView.Get() );

        // Clear the backbuffer to a solid color, and reset the depth stencil.
        const float clearColor[4] = { 0.071f, 0.04f, 0.561f, 1.0f };
        D3DDeviceContext()->ClearRenderTargetView( m_windowRenderTargetView.Get(), clearColor );
        D3DDeviceContext()->ClearDepthStencilView( m_windowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
    }

    m_sRGBShader.Bind();
    {

        FRMVECTOR2 CardSize = FRMVECTOR2( 0.4f, 0.8f );
        FRMVECTOR2 CardPos1 = FRMVECTOR2( -0.475f, 0.0f );

        m_psRGBTexture->Bind( 0 );
        RenderScreenAlignedQuad( CardPos1 - CardSize, CardPos1 + CardSize );

        FRMVECTOR2 CardPos2 = FRMVECTOR2( 0.475f, 0.0f );
        m_pRGBTexture->Bind( 0 );
        RenderScreenAlignedQuad( CardPos2 - CardSize, CardPos2 + CardSize );
    }

    if ( m_bsRGBRenderTarget )
    {
        EndFBO( m_sRGBRenderTarget );

        // Bind the backbuffer as a render target, don't use the depth buffer since we are drawing a full screen quad
        D3DDeviceContext()->OMSetRenderTargets( 1, m_windowRenderTargetView.GetAddressOf(), NULL );

        // Discard the color buffer since we will write to each pixel and this is faster than a clear
        // Clear the backbuffer to a solid color, and reset the depth stencil.
        D3DDeviceContext()->DiscardView( m_windowRenderTargetView.Get() );
        
        m_sRGBFinalGammaCorrectShader.Bind();
        {
            // Set the sRBB render target as a texture to draw to the screen
            D3DDeviceContext()->PSSetShaderResources( 0, 1, m_sRGBRenderTarget->m_pTexture0ShaderResourceView.GetAddressOf() );
            RenderScreenAlignedQuad( FRMVECTOR2(-1.0, 1.0) , FRMVECTOR2(1.0, -1.0) );
        }
    }
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    DrawScene();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
