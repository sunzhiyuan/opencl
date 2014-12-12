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
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "NPR" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_fRotateTime = 0.0f;

    m_pLogoTexture         = NULL;
    m_pCelShadingTexture   = NULL;
    m_pCelConstantBuffer   = NULL;
    
    m_pNormalDepthConstantBuffer = NULL;

    m_fOutlineHalfStepSizeX    = 0.0f;
    m_fOutlineHalfStepSizeY    = 0.0f;

    m_fOutlineWidth  = 0.0f;

    m_fAmbient = 0;

    m_vLightPosition.x  = 1.0f;
    m_vLightPosition.y  = 1.0f;
    m_vLightPosition.z  = 1.0f;

    m_fAmbient = 0.2f;

    m_pCelShadingTexture    = NULL;
    m_pLogoTexture 	        = NULL;
    m_pRayGunTexture        = NULL;

    m_pTextures[CEL_SHADED_FBO]            = nullptr;
    m_pRenderTargetViews[CEL_SHADED_FBO]   = nullptr;
    m_pShaderResourceViews[CEL_SHADED_FBO] = nullptr;
    m_pSamplers[CEL_SHADED_FBO]            = nullptr;
    
    m_pTextures[NORMAL_AND_DEPTH_FBO]            = nullptr;
    m_pRenderTargetViews[NORMAL_AND_DEPTH_FBO]   = nullptr;
    m_pShaderResourceViews[NORMAL_AND_DEPTH_FBO] = nullptr;
    m_pSamplers[NORMAL_AND_DEPTH_FBO]            = nullptr;
    
    //m_pTextureIds[OUTLINE_FBO]    = NULL;
    
    m_pDepthTexture = nullptr;
    m_pDepthRenderTargetView = nullptr;
    
    m_pFullScreenQuadVertexBuffer = NULL;
    m_pFullScreenQuadIndexBuffer = NULL;
    m_hFrameBufferNameId = NULL;
    
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the cel-shading shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION",   FRM_VERTEX_POSITION,  DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "NORMAL",     FRM_VERTEX_NORMAL,    DXGI_FORMAT_R32G32B32_FLOAT },
            { "TEXCOORD",   FRM_VERTEX_TEXCOORD0, DXGI_FORMAT_R32G32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_CelShadingShader.Compile( "CelShadingVS.cso", "CelShadingPS.cso",
                                                  pShaderAttributes,
                                                  nNumShaderAttributes ) )
            return FALSE;

        if ( FALSE == FrmCreateConstantBuffer(sizeof(CelConstantBuffer), &m_CelConstantBufferData, &m_pCelConstantBuffer))
            return FALSE;    
    }

    // Create our Blend FBO shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }, 
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_NormalDepthShader.Compile( "BlendVS.cso", "BlendPS.cso",
                                                   pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if ( FALSE == FrmCreateConstantBuffer(sizeof(BlendConstantBuffer), &m_NormalDepthConstantBufferData, &m_pNormalDepthConstantBuffer))
            return FALSE;            
    }

    return TRUE;
}



BOOL CSample::FreeDrawBuffers()
{
	if( m_pTextures[ CEL_SHADED_FBO ] )
	{
        m_pTextures[ CEL_SHADED_FBO ] = nullptr;
        m_pRenderTargetViews[ CEL_SHADED_FBO ] = nullptr;
        m_pShaderResourceViews[ CEL_SHADED_FBO ] = nullptr;
        m_pSamplers[ CEL_SHADED_FBO ] = nullptr;
	}	
		
	if( m_pTextures[ NORMAL_AND_DEPTH_FBO ] )
	{
        m_pTextures[ NORMAL_AND_DEPTH_FBO ] = nullptr;
        m_pShaderResourceViews[ NORMAL_AND_DEPTH_FBO ] = nullptr;
        m_pSamplers[ NORMAL_AND_DEPTH_FBO ] = nullptr;
        m_pRenderTargetViews[ NORMAL_AND_DEPTH_FBO ] = nullptr;
	}	

    if ( m_pDepthTexture )
    {
        m_pDepthTexture = nullptr;
        m_pDepthRenderTargetView = nullptr;
    }
	
	return TRUE;
}

BOOL CSample::CreateDrawBuffers()
{
    FreeDrawBuffers();

    D3D11_TEXTURE2D_DESC textureDesc;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    D3D11_SAMPLER_DESC samplerDesc;
    
    // Texture description
    ZeroMemory( &textureDesc, sizeof(textureDesc) );
    textureDesc.Width = m_nFBOTextureWidth;
    textureDesc.Height = m_nFBOTextureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Render target description
    ZeroMemory( &renderTargetViewDesc, sizeof(renderTargetViewDesc) );
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    
    // Shader resource view description
    ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

    // Sampler description
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
    if ( FAILED(D3DDevice()->CreateTexture2D( &textureDesc, NULL, &m_pTextures[ CEL_SHADED_FBO ] ) ) )
    {
        return FALSE;
    }

    // Create Render Target View 0
    if ( FAILED(D3DDevice()->CreateRenderTargetView( m_pTextures[CEL_SHADED_FBO].Get(), 
                                                     &renderTargetViewDesc, 
                                                     &m_pRenderTargetViews[CEL_SHADED_FBO] ) ) )
    {
        return FALSE;
    }

    // Create Render Shader Resource View 0
    if ( FAILED(D3DDevice()->CreateShaderResourceView( m_pTextures[CEL_SHADED_FBO].Get(),
                                                       &shaderResourceViewDesc,
                                                       &m_pShaderResourceViews[CEL_SHADED_FBO] ) ) )
    {
        return FALSE;
    }

    // Create Sampler State 0
    if ( FAILED(D3DDevice()->CreateSamplerState( &samplerDesc, &m_pSamplers[CEL_SHADED_FBO] ) ) )
    {
        return FALSE;
    }

    // Create Texture 1
    if ( FAILED(D3DDevice()->CreateTexture2D( &textureDesc, NULL, &m_pTextures[ NORMAL_AND_DEPTH_FBO ] ) ) )
    {
        return FALSE;
    }

    // Create Render Target View 1
    if ( FAILED(D3DDevice()->CreateRenderTargetView( m_pTextures[NORMAL_AND_DEPTH_FBO].Get(), 
                                                     &renderTargetViewDesc, 
                                                     &m_pRenderTargetViews[ NORMAL_AND_DEPTH_FBO] ) ) )
    {
        return FALSE;                                                     
    }

    // Create Render Shader Resource View 1
    if ( FAILED(D3DDevice()->CreateShaderResourceView( m_pTextures[NORMAL_AND_DEPTH_FBO].Get(),
                                                       &shaderResourceViewDesc,
                                                       &m_pShaderResourceViews[NORMAL_AND_DEPTH_FBO] ) ) )
    {
        return FALSE;
    }

    // Create Sampler 1
    if ( FAILED(D3DDevice()->CreateSamplerState( &samplerDesc, &m_pSamplers[NORMAL_AND_DEPTH_FBO] ) ) )
    {
        return FALSE;
    }


    // Create a depth/stencil texture for the depth attachment
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    if ( FAILED(D3DDevice()->CreateTexture2D( &textureDesc, NULL, &m_pDepthTexture ) ) )
        return FALSE;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = textureDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    
    if ( FAILED(D3DDevice()->CreateDepthStencilView( m_pDepthTexture.Get(), 
                                                     &depthStencilViewDesc, 
                                                     &m_pDepthRenderTargetView ) ) )
        return FALSE;    

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    m_bShouldRotate = TRUE;

    // Create the font
    if( FALSE == m_Font.Create( "Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceD3D resource;
    if( FALSE == resource.LoadFromFile( "Textures.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Create the cel shading texture
    m_pCelShadingTexture = resource.GetTexture( "Cel-Shading" );

    // Create the ray gun texture
    m_pRayGunTexture = resource.GetTexture( "RayGun" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddFloatVariable( &m_fOutlineWidth , "Outline Width", "%3.2f" );
    m_UserInterface.AddFloatVariable( &m_vLightPosition.x, "Light Pos", "%4.2f" );
    m_UserInterface.AddFloatVariable( &m_fAmbient, "Ambient Light", "%4.2f" );
    m_UserInterface.AddBoolVariable( &m_bShouldRotate, "Should Rotate" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Decrease Outline Width" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Increase Outline Width" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Decrease Ambient Light" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Increase Ambient Light" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Toggle Rotation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_DPAD, "Move Light" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "RayGun.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh.MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // VB/IB for full-screen Quad
    FLOAT32 Quad[] =
    {
        -1.0, -1.0f, 0.0f, 1.0f,
        +1.0, -1.0f, 1.0f, 1.0f,
        -1.0, +1.0f, 0.0f, 0.0f,
        +1.0, +1.0f, 1.0f, 0.0f,
    };
    if( FALSE == FrmCreateVertexBuffer( 4, sizeof(FLOAT32) * 4, &Quad[0], &m_pFullScreenQuadVertexBuffer ) )
        return FALSE;

    UINT32 indices[6] = { 0, 1, 2, 2, 1, 3 };
    if( FALSE == FrmCreateIndexBuffer( 6, sizeof(UINT32), &indices, &m_pFullScreenQuadIndexBuffer ) )
        return FALSE;

    
    // Initial state
    D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    rdesc.FrontCounterClockwise = TRUE; // Change the winding direction to match GL    
    D3DDevice()->CreateRasterizerState(&rdesc, &m_RasterizerState);
    D3DDeviceContext()->RSSetState(m_RasterizerState.Get());

    // State blocks
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
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 0.1f, 10.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);


    // Setup our depth buffer that will be shared by our various offscreen buffers
    m_nFBOTextureWidth  = m_nWidth;
    m_nFBOTextureHeight = m_nHeight;

    m_fOutlineWidth = 0.3f;
    m_fOutlineHalfStepSizeX = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureWidth ) / 2.0f;
    m_fOutlineHalfStepSizeY = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureHeight ) / 2.0f;

    // Setup our FBOs
    CreateDrawBuffers();

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Free constant buffer memory
    if( m_pCelConstantBuffer)         m_pCelConstantBuffer->Release();
    if( m_pNormalDepthConstantBuffer) m_pNormalDepthConstantBuffer->Release();
    
    // Free texture memory
    if( m_pCelShadingTexture ) m_pCelShadingTexture->Release();
    if( m_pLogoTexture )	   m_pLogoTexture->Release();
    if( m_pRayGunTexture )     m_pRayGunTexture->Release();

    // VB/IB
    if( m_pFullScreenQuadVertexBuffer ) m_pFullScreenQuadVertexBuffer->Release();
    if( m_pFullScreenQuadIndexBuffer ) m_pFullScreenQuadIndexBuffer->Release();

    // Free mesh memory
    m_Mesh.Destroy();
    
	FreeDrawBuffers();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();
    if( m_bShouldRotate )
    {
        m_fRotateTime += fElapsedTime;
    }

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    if( nButtons & INPUT_KEY_1 )
    {
        m_fOutlineWidth = max( m_fOutlineWidth - 0.1f * fElapsedTime, 0.0f );
        m_fOutlineHalfStepSizeX = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureWidth  ) / 2.0f;
        m_fOutlineHalfStepSizeY = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureHeight ) / 2.0f;
    }

    if( nButtons & INPUT_KEY_2 )
    {
        m_fOutlineWidth = min( m_fOutlineWidth + 0.1f * fElapsedTime, 0.30f );
        m_fOutlineHalfStepSizeX = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureWidth  ) / 2.0f;
        m_fOutlineHalfStepSizeY = ( m_fOutlineWidth / (FLOAT32)m_nFBOTextureHeight ) / 2.0f;
    }

    if( nButtons & INPUT_KEY_3 )
    {
        if( m_fAmbient > 0.01f )
            m_fAmbient -= 0.2f * fElapsedTime;
    }

    if( nButtons & INPUT_KEY_4 )
    {
        if( m_fAmbient < 0.99f )
            m_fAmbient += 0.2f * fElapsedTime;
    }

    if( nPressedButtons & INPUT_KEY_5 )
    {
        m_bShouldRotate = 1 - m_bShouldRotate;
    }

    if( nButtons & INPUT_DPAD_LEFT )
    {
        // Move the light to the left
        m_vLightPosition.x -= 4.0f * fElapsedTime;
        if( m_vLightPosition.x < -5.0f )
            m_vLightPosition.x = -5.0f;
    }

    if( nButtons & INPUT_DPAD_RIGHT )
    {
        // Move the light to the right
        m_vLightPosition.x += 4.0f * fElapsedTime;
        if( m_vLightPosition.x > 5.0f )
            m_vLightPosition.x = 5.0f;
    }

    if( nButtons & INPUT_DPAD_DOWN )
    {
        // Move the light to the left
        m_vLightPosition.y -= 4.0f * fElapsedTime;
        if( m_vLightPosition.y < -5.0f )
            m_vLightPosition.y = -5.0f;
    }

    if( nButtons & INPUT_DPAD_UP )
    {
        // Move the light to the right
        m_vLightPosition.y += 4.0f * fElapsedTime;
        if( m_vLightPosition.y > 5.0f )
            m_vLightPosition.y = 5.0f;
    }

    // Rotate the object
    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.30f, 0.5f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.10f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    FRMMATRIX4X4 matModel = FrmMatrixRotate( m_fRotateTime, FRMVECTOR3( 0.0f, 1.0f, 0.0f ) );
    // Build the matrices
    m_matModelView     = FrmMatrixMultiply( matModel,       m_matView );
    m_matModelViewProj = FrmMatrixMultiply( m_matModelView, m_matProj );

    m_matNormalMatrix = FrmMatrixNormal( m_matModelView );
}



//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderScreenAlignedQuad()
{
    // Disable depth test
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 1 );

    
    m_pFullScreenQuadVertexBuffer->Bind( 0 );
    m_pFullScreenQuadIndexBuffer->Bind( 0 );


    FrmDrawIndexedVertices( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 6, sizeof(UINT32), 0 );
        

    // Re-enable depth test
    D3DDeviceContext()->OMSetDepthStencilState( m_DefaultDepthStencilState.Get(), 1 );
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::RenderSceneCelShadedDepthAndNormalsToFBO()
{
    // Clear the Render target views
    const float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    D3DDeviceContext()->ClearRenderTargetView( m_pRenderTargetViews[0].Get(), clearColor );
    D3DDeviceContext()->ClearRenderTargetView( m_pRenderTargetViews[1].Get(), clearColor );
    D3DDeviceContext()->ClearDepthStencilView( m_pDepthRenderTargetView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
    
    // Set the render targets current
    ID3D11RenderTargetView* pRTViews[2] = { m_pRenderTargetViews[0].Get(), m_pRenderTargetViews[1].Get() };
    D3DDeviceContext()->OMSetRenderTargets(
        2, 
        pRTViews,
        m_pDepthRenderTargetView.Get() );
        
    // Set the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);
    
    m_CelShadingShader.Bind();
    m_pCelShadingTexture->Bind( 0 );
    m_pRayGunTexture->Bind( 1 );

    // Bind the constants
    m_CelConstantBufferData.matNormal = FRMMATRIX4X4( m_matNormalMatrix );
    m_CelConstantBufferData.matModelViewProj = m_matModelViewProj;
    m_CelConstantBufferData.vLightPos = FRMVECTOR4( m_vLightPosition, 1.0f );
    m_CelConstantBufferData.vAmbient = FRMVECTOR4( m_fAmbient, m_fAmbient, m_fAmbient, 1.0f );
    m_pCelConstantBuffer->Update( &m_CelConstantBufferData );
    m_pCelConstantBuffer->Bind ( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );

    m_Mesh.Render( &m_CelShadingShader );

    // Restore the render target
    pRTViews[0] = m_windowRenderTargetView.Get();
    pRTViews[1] = NULL;
    D3DDeviceContext()->OMSetRenderTargets( 2, pRTViews, m_windowDepthStencilView.Get() );
}



//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------

VOID CSample::RenderOutline()
{
    // Set the viewport
    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_nWidth);
    viewport.Height = static_cast<float>(m_nHeight);
    viewport.MinDepth = D3D11_MIN_DEPTH;
    viewport.MaxDepth = D3D11_MAX_DEPTH;
    D3DDeviceContext()->RSSetViewports(1, &viewport);

    // Bind the backbuffer as a render target, no depth buffer needed since we are just drawing a full screen quad
    D3DDeviceContext()->OMSetRenderTargets( 1, m_windowRenderTargetView.GetAddressOf(), NULL );

    // Discard the contents of the color buffer since we will strike every pixel and it is more expensive to clear
    D3DDeviceContext()->DiscardView( m_windowRenderTargetView.Get() );

    m_NormalDepthShader.Bind();

    // Bind color attachment 0 texture/sampler
    D3DDeviceContext()->PSSetShaderResources( 0, 1, m_pShaderResourceViews[CEL_SHADED_FBO].GetAddressOf());
    D3DDeviceContext()->PSSetSamplers( 0, 1, m_pSamplers[CEL_SHADED_FBO].GetAddressOf());

    // Bind color attachment 1 texture/sampler
    D3DDeviceContext()->PSSetShaderResources( 1, 1, m_pShaderResourceViews[NORMAL_AND_DEPTH_FBO].GetAddressOf());
    D3DDeviceContext()->PSSetSamplers( 1, 1, m_pSamplers[NORMAL_AND_DEPTH_FBO].GetAddressOf());

    // Update constans
    m_NormalDepthConstantBufferData.fHalfStepSizeX = m_fOutlineHalfStepSizeX * 4.0f;
	m_NormalDepthConstantBufferData.fHalfStepSizeY = m_fOutlineHalfStepSizeY * 4.0f;
    m_pNormalDepthConstantBuffer->Update( &m_NormalDepthConstantBufferData );
    m_pNormalDepthConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );

    RenderScreenAlignedQuad();

    // Unbind the SRV's
    ID3D11ShaderResourceView* unbind[1] = { NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 1, unbind );
    D3DDeviceContext()->PSSetShaderResources( 1, 1, unbind );
    
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    RenderSceneCelShadedDepthAndNormalsToFBO();
    RenderOutline();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

