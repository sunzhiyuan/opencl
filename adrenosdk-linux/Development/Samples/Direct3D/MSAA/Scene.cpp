//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
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
#include <FrmUtils.h>
#include "Scene.h"


//--------------------------------------------------------------------------------------
// Name: MaterialList[]
// Desc: Global list of materials
//--------------------------------------------------------------------------------------
struct MATERIAL
{
    const CHAR* strMaterial;
    FRMVECTOR4  vAmbient;
    FRMVECTOR4  vDiffuse;
    FRMVECTOR3  vSpecular;
    FLOAT32     fShininess;
};

static const MATERIAL MaterialList[] =
{
    {   "Default Material",
        FRMVECTOR4( 0.0f,      0.1f,      0.25f,     1.0f ),
        FRMVECTOR4( 0.0f,      0.4f,      1.0f,      1.0f ),
        FRMVECTOR3( 1.0f,      1.0f,      1.0f ),
        20.0f
    },
    {   "Emerald",
        FRMVECTOR4( 0.0215f,   0.1745f,   0.0215f,   1.0f ),
        FRMVECTOR4( 0.07568f,  0.61424f,  0.07568f,  1.0f ),
        FRMVECTOR3( 0.633f,    0.727811f, 0.633f ),
        76.8f,
    },
    {   "Jade",
        FRMVECTOR4( 0.135f,    0.2225f,   0.1575f,   1.0f ),
        FRMVECTOR4( 0.54f,     0.89f,     0.63f,     1.0f ),
        FRMVECTOR3( 0.316228f, 0.316228f, 0.316228f ),
        12.8f,
    },
    {   "Pearl",
        FRMVECTOR4( 0.25f,     0.20725f,  0.20725f,  1.0f ),
        FRMVECTOR4( 1.0f,      0.829f,    0.829f,    1.0f ),
        FRMVECTOR3( 0.296648f, 0.296648f, 0.2966f ),
        11.264f,
    },
    {   "Ruby",
        FRMVECTOR4( 0.1745f,   0.01175f,  0.01175f,  1.0f ),
        FRMVECTOR4( 0.61424f,  0.04136f,  0.04136f,  1.0f ),
        FRMVECTOR3( 0.727811f, 0.626959f, 0.626959f ),
        76.8f,
    },
    {   "Turquoise",
        FRMVECTOR4( 0.1f,      0.18725f,  0.1745f,   1.0f ),
        FRMVECTOR4( 0.396f,    0.74151f,  0.69102f,  1.0f ),
        FRMVECTOR3( 0.297254f, 0.30829f,  0.306678f ),
        12.8f,
    },
    {   "Brass",
        FRMVECTOR4( 0.329412f, 0.223529f, 0.027451f, 1.0f ),
        FRMVECTOR4( 0.780382f, 0.568627f, 0.113725f, 1.0f ),
        FRMVECTOR3( 0.992157f, 0.941176f, 0.807843f ),
        27.897f,
    },
    {   "Bronze",
        FRMVECTOR4( 0.2125f,   0.1275f,   0.054f,    1.0f ),
        FRMVECTOR4( 0.714f,    0.4284f,   0.18144f,  1.0f ),
        FRMVECTOR3( 0.393548f, 0.271906f, 0.166721f ),
        25.6f,
    },
    {   "Copper",
        FRMVECTOR4( 0.19125f,  0.0735f,   0.0225f,   1.0f ),
        FRMVECTOR4( 0.7038f,   0.27048f,  0.0828f,   1.0f ),
        FRMVECTOR3( 0.256777f, 0.137622f, 0.086014f ),
        12.8f,
    },
    {   "Gold",
        FRMVECTOR4( 0.24725f,  0.1995f,   0.0745f,   1.0f ),
        FRMVECTOR4( 0.75164f,  0.60648f,  0.22648f,  1.0f ),
        FRMVECTOR3( 0.628281f, 0.555802f, 0.366065f ),
        51.2f,
    },
    {   "Silver",
        FRMVECTOR4( 0.19225f,  0.19225f,  0.19225f,  1.0f ),
        FRMVECTOR4( 0.50754f,  0.50754f,  0.50754f,  1.0f ),
        FRMVECTOR3( 0.508273f, 0.508273f, 0.508273f ),
        51.2f,
    },
    {
        "Yellow Plastic",
        FRMVECTOR4( 0.0f,      0.0f,      0.0f,      1.0f ),
        FRMVECTOR4( 0.5f,      0.5f,      0.0f,      1.0f ),
        FRMVECTOR3( 0.6f,      0.6f,      0.5f ),
        32.0f,
    },
    {
        "Cyan Rubber",
        FRMVECTOR4( 0.0f,      0.05f,     0.05f,     1.0f ), 
        FRMVECTOR4( 0.4f,      0.5f,      0.5f,      1.0f ),
        FRMVECTOR3( 0.04f,     0.7f,      0.7f ),
        10.0f,
    }
};

const int NUM_MATERIALS = sizeof(MaterialList) / sizeof(MaterialList[0]);


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "MSAA" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_bMSAA             = TRUE;

    m_pLogoTexture      = NULL;
    m_qMeshRotation     = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
    m_qRotationDelta    = FRMVECTOR4( 0.0f, 0.01f, 0.0f, 1.0f );
    m_strLightingType   = "Per Pixel";
    m_nMaterialIndex    = 0;
    m_strMaterialName   = MaterialList[ m_nMaterialIndex ].strMaterial;
    m_vLightPosition.x  = 2.0f;
    m_vLightPosition.y  = 1.0f;
    m_vLightPosition.z  = 1.0f;
    m_MSAART            = NULL;
    m_ResolvedRT        = NULL;
    m_pQuadVertexBuffer = NULL;
    m_pQuadIndexBuffer  = NULL;
	
    m_pPerPixelLightingConstantBuffer = NULL;
    
}

//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the PerPixelLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "NORMAL",   FRM_VERTEX_NORMAL, DXGI_FORMAT_R32G32B32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_PerPixelLightingShader.Compile( "PerPixelLightingVS.cso", "PerPixelLightingPS.cso",
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        if( FALSE == FrmCreateConstantBuffer( sizeof(m_PerPixelLightingConstantBufferData), &m_PerPixelLightingConstantBufferData,
                                              &m_pPerPixelLightingConstantBuffer ) )
            return FALSE;        
    }

    	// preview shader
	{
		FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
		{ 
			{ "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32A32_FLOAT }
		};
		const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

		if( FALSE == m_PreviewShader.Compile( "PreviewShaderVS.cso", "PreviewShaderPS.cso",
                                              pShaderAttributes, nNumShaderAttributes ) )
			return FALSE;		
	}



    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: CreateFBO()
// Desc: Create a render target (with or without MSAA depending on nSampleCount)
//--------------------------------------------------------------------------------------
BOOL CSample::CreateFBO( UINT32 nWidth, UINT32 nHeight, DXGI_FORMAT nFormat, UINT32 nSampleCount, FrameBufferObject** ppFBO )
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
    textureDesc.SampleDesc.Count = nSampleCount; // If this is > 1, MSAA is enabled
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;    
    if (nSampleCount == 1)
    {
        // Non-MSAA
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    }
    else
    {
        // MSAA - just use as a render target.  We will use resolve using ResolveSubresource 
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
    }
    
    // Render target view
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
    renderTargetViewDesc.Format = textureDesc.Format;
    if (nSampleCount == 1)
    {
        // Non-MSAA
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    }
    else
    {
        // MSAA
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    }    
    renderTargetViewDesc.Texture2D.MipSlice = 0;
          

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

    // Only create SRV and Sampler for non-MSAA
    if (nSampleCount == 1)
    {
        // Shader resource view
        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
        ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
        shaderResourceViewDesc.Format = textureDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    
        // Create Render Shader Resource View 0
        if ( FAILED(D3DDevice()->CreateShaderResourceView( (*ppFBO)->m_pTexture0.Get(),
                                                           &shaderResourceViewDesc,
                                                           &(*ppFBO)->m_pTexture0ShaderResourceView ) ) )
        {
            return FALSE;
        }

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

        // Create Sampler State 0
        if ( FAILED(D3DDevice()->CreateSamplerState( &samplerDesc, &(*ppFBO)->m_pTexture0Sampler ) ) )
        {
            return FALSE;
        }
    }
    
     // Create a depth/stencil texture for the depth attachment
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    if ( FAILED(D3DDevice()->CreateTexture2D( &textureDesc, NULL, &(*ppFBO)->m_pDepthTexture ) ) )
        return FALSE;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = textureDesc.Format;
    if (nSampleCount == 1)
    {
        // Non-MSAA    
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    }
    else
    {
        // MSAA    
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    }
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
//--------------------------------------------------------------------------------------
// Name: RenderScreenAlignedQuad
// Desc: Draws a full-screen rectangle
//--------------------------------------------------------------------------------------
VOID CSample::RenderScreenAlignedQuad()
{
    D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );

    const FLOAT32 Quad[] =
	{
		+1.0, +1.0f, 1.0f, 0.0f,
		-1.0, +1.0f, 0.0f, 0.0f,
		+1.0, -1.0f, 1.0f, 1.0f,
		-1.0, -1.0f, 0.0f, 1.0f,
	};

    VOID* pData = m_pQuadVertexBuffer->Map( D3D11_MAP_WRITE_DISCARD );
    FrmMemcpy( pData, Quad, sizeof(Quad) );
    m_pQuadVertexBuffer->Unmap();

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
    // Create the font
    if( FALSE == m_Font.Create( "Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceD3D resource;
    if( FALSE == resource.LoadFromFile( "Textures.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddStringVariable( &m_strMaterialName, "Material" );
    m_UserInterface.AddStringVariable( &m_strLightingType, "Type" );
    m_UserInterface.AddFloatVariable( &m_vLightPosition.x, "Light Pos" );
    m_UserInterface.AddIntVariable( &m_nSampleCount, "MSAA Sample Count" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Material" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Toggle MSAA" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Increase MSAA Sample Count" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Decrease MSAA Sample Count" );

    m_UserInterface.AddHelpKey( FRM_FONT_KEY_DPAD, "Move Light" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Phone.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh.MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Determine multisample sample count and quality levels supported
    for (int sampleCount = 2; sampleCount < 16; sampleCount++ )
    {
        UINT nQualityLevels = 0;

        D3DDevice()->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &nQualityLevels );
        
        if ( nQualityLevels > 0 )
        {
            m_SampleCountSupported.push_back( sampleCount );
        }
    }

    // Use the min supported sample count to start
    m_nSampleCountIndex = 0;
    m_nSampleCount = m_SampleCountSupported[m_nSampleCountIndex];
    
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.1f, 0.5f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );

    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 0.1f, 10.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Create the FBOs
    if ( FALSE == CreateFBO( m_nWidth, m_nHeight, DXGI_FORMAT_R8G8B8A8_UNORM, m_nSampleCount, &m_MSAART ) )
        return FALSE;
    if ( FALSE == CreateFBO( m_nWidth, m_nHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 1, &m_ResolvedRT ) )
        return FALSE;


    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );
    
    // Resize the arc ball control
    m_ArcBall.Resize( m_nWidth, m_nHeight );

    // Render state blocks
    D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    rdesc.FrontCounterClockwise = TRUE; // Change the winding direction to match GL    
    D3DDevice()->CreateRasterizerState(&rdesc, &m_RasterizerState);
    D3DDeviceContext()->RSSetState(m_RasterizerState.Get());

    // MSAA rasterizer state
    rdesc.MultisampleEnable = TRUE;
    D3DDevice()->CreateRasterizerState(&rdesc, &m_MSAARasterizerState);

    D3D11_DEPTH_STENCIL_DESC dsdesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DefaultDepthStencilState);
    dsdesc.DepthEnable = false;
    D3DDevice()->CreateDepthStencilState(&dsdesc, &m_DisabledDepthStencilState);

    // Quad VB/IB
    UINT32 indices[6] = { 0, 1, 2, 2, 1, 3 };
    if( FALSE == FrmCreateVertexBuffer( 4, sizeof(FLOAT32) * 4, NULL, &m_pQuadVertexBuffer ) )
        return FALSE;

    if( FALSE == FrmCreateIndexBuffer( 6, sizeof(UINT32), &indices, &m_pQuadIndexBuffer ) )
        return FALSE;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_pLogoTexture ) m_pLogoTexture->Release();

    if( m_pPerPixelLightingConstantBuffer ) m_pPerPixelLightingConstantBuffer->Release();

    if( m_pQuadVertexBuffer ) m_pQuadVertexBuffer->Release();
    if( m_pQuadIndexBuffer ) m_pQuadIndexBuffer->Release();    

    DestroyFBO( m_MSAART );
    DestroyFBO( m_ResolvedRT );
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();

    // Process input
    {
        // Get button input since the last frame
        UINT32 nButtons;
        UINT32 nPressedButtons;
        m_Input.GetButtonState( &nButtons, &nPressedButtons );

        // Get pointer input since the last frame
        UINT32     nPointerState;
        FRMVECTOR2 vPointerPosition;
        m_Input.GetPointerState( &nPointerState, &vPointerPosition );

        // Conditionally advance the UI state
        m_UserInterface.HandleInput( m_nWidth, m_nHeight, nPressedButtons,
                                     nPointerState, vPointerPosition );

        // Use the pointer input to rotate the object
        FRMVECTOR4 qArcBallDelta = m_ArcBall.HandleInput( vPointerPosition, nPointerState & FRM_INPUT::POINTER_DOWN );
        if( m_ArcBall.IsDragging() )
            m_qRotationDelta = qArcBallDelta;
        FRMVECTOR4 qDelta = nPointerState ? qArcBallDelta : m_qRotationDelta;
        
        qDelta = FrmVector4Normalize( qDelta );
        m_qMeshRotation = FrmQuaternionMultiply( m_qMeshRotation, qDelta );

        if( nPressedButtons & FRM_INPUT::KEY_1 )
        {
            m_nMaterialIndex  = ( m_nMaterialIndex + 1 ) % NUM_MATERIALS;
            m_strMaterialName = MaterialList[ m_nMaterialIndex ].strMaterial;
        }

        if( nPressedButtons & FRM_INPUT::KEY_2 )
        {
            m_bMSAA = !m_bMSAA;
        }

        INT32 nSampleCount = m_nSampleCount;
        if ( nPressedButtons & FRM_INPUT::KEY_3 )
        {
            if ( m_nSampleCountIndex < m_SampleCountSupported.size() - 1 )
            {
                m_nSampleCountIndex++;
                m_nSampleCount = m_SampleCountSupported[m_nSampleCountIndex];
            }
        }

        if ( nPressedButtons & FRM_INPUT::KEY_4 )
        {
            if ( m_nSampleCountIndex > 0 )
            {
                m_nSampleCountIndex--;
                m_nSampleCount = m_SampleCountSupported[m_nSampleCountIndex];
            }
        }

        // If MSAA sample count changed, recreate RT
        if ( m_nSampleCount != nSampleCount )
        {
            // Re-create the MSAA render target iwth the new sample count
            CreateFBO( m_nWidth, m_nHeight, DXGI_FORMAT_R8G8B8A8_UNORM, m_nSampleCount, &m_MSAART );
        }

        // Update title
        static char strTitle[256];
        if ( m_bMSAA )
        {
            FrmSprintf(strTitle, 255, "MSAA %dX", m_nSampleCount );
            m_UserInterface.SetHeading( strTitle );
        }
        else
        {
            m_UserInterface.SetHeading( "No MSAA" );
        }
        
        // Move the light up/down/left/right
        if( nButtons & FRM_INPUT::DPAD_UP )
            m_vLightPosition.y = FrmMin( +5.0f, m_vLightPosition.y + 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_DOWN )
            m_vLightPosition.y = FrmMax( -5.0f, m_vLightPosition.y - 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_LEFT )
            m_vLightPosition.x = FrmMax( -5.0f, m_vLightPosition.x - 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_RIGHT )
            m_vLightPosition.x = FrmMin( +5.0f, m_vLightPosition.x + 5.0f * fElapsedTime );
    }

    // Orientate the object
    FRMMATRIX4X4 matRotate    = FrmMatrixRotate( m_qMeshRotation );
    FRMMATRIX4X4 matTranslate = FrmMatrixTranslate( 0.0f, -0.07f, 0.0f );

    // Build the matrices
    FRMMATRIX4X4 matModel = FrmMatrixMultiply( matTranslate,   matRotate );
    m_matModelView        = FrmMatrixMultiply( matModel,       m_matView );
    m_matModelViewProj    = FrmMatrixMultiply( m_matModelView, m_matProj );
    m_matNormal           = FrmMatrixNormal( m_matModelView );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{    
    // Render to MSAA render target
    if (m_bMSAA)
    {
        // Enable MSAA
        D3DDeviceContext()->RSSetState(m_MSAARasterizerState.Get());

        BeginFBO( m_MSAART );
    

        // Clear color and depth
        const float clearColor0[4] = { 0.071f, 0.04f, 0.561f, 1.0f };
        D3DDeviceContext()->ClearRenderTargetView( m_MSAART->m_pTexture0RenderTargetView.Get(), clearColor0 );
        D3DDeviceContext()->ClearDepthStencilView( m_MSAART->m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
    }
    else
    {
        // No MSAA, bind the backbuffer as a render target
        D3DDeviceContext()->OMSetRenderTargets( 1, m_windowRenderTargetView.GetAddressOf(), m_windowDepthStencilView.Get() );

        // Clear the backbuffer to a solid color, and reset the depth stencil.
        const float clearColor[4] = { 0.071f, 0.04f, 0.561f, 1.0f };
        D3DDeviceContext()->ClearRenderTargetView( m_windowRenderTargetView.Get(), clearColor );
        D3DDeviceContext()->ClearDepthStencilView( m_windowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
    }

    m_PerPixelLightingShader.Bind();
    m_PerPixelLightingConstantBufferData.vAmbient = MaterialList[m_nMaterialIndex].vAmbient;
    m_PerPixelLightingConstantBufferData.vDiffuse = MaterialList[m_nMaterialIndex].vDiffuse;
    m_PerPixelLightingConstantBufferData.vSpecular = FRMVECTOR4( MaterialList[m_nMaterialIndex].vSpecular, 1.0f );
    m_PerPixelLightingConstantBufferData.matModelView = m_matModelView;
    m_PerPixelLightingConstantBufferData.matModelViewProj = m_matModelViewProj;
    m_PerPixelLightingConstantBufferData.matNormal = m_matNormal;
    m_PerPixelLightingConstantBufferData.vLightPos = FRMVECTOR4( m_vLightPosition, 1.0f );
    m_pPerPixelLightingConstantBuffer->Update( &m_PerPixelLightingConstantBufferData );
    m_pPerPixelLightingConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );

    m_Mesh.Render( &m_PerPixelLightingShader );

    if ( m_bMSAA )
    {        
        EndFBO( m_MSAART );

        // Disable MSAA
        D3DDeviceContext()->RSSetState(m_RasterizerState.Get());

    
        // Resolve the MSAA render target to a non-MSAA target    
        D3DDeviceContext()->ResolveSubresource( m_ResolvedRT->m_pTexture0.Get(),
                                                0,
                                                m_MSAART->m_pTexture0.Get(),
                                                0,
                                                DXGI_FORMAT_R8G8B8A8_UNORM );

        D3DDeviceContext()->DiscardView( m_MSAART->m_pTexture0RenderTargetView.Get() );

        // Bind the backbuffer as a render target, no depth buffer since we do not need to do depth test
        D3DDeviceContext()->OMSetRenderTargets( 1, m_windowRenderTargetView.GetAddressOf(), NULL );
    
        // Discard the view since we re-render the whole screen each frame
        D3DDeviceContext()->DiscardView( m_windowRenderTargetView.Get() );

        // Render the resolved MSAA render target to the screen
        m_PreviewShader.Bind();
        D3DDeviceContext()->PSSetShaderResources( 0, 1, m_ResolvedRT->m_pTexture0ShaderResourceView.GetAddressOf() );
        D3DDeviceContext()->PSSetSamplers( 0, 1, m_ResolvedRT->m_pTexture0Sampler.GetAddressOf() );    

        RenderScreenAlignedQuad();    
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

