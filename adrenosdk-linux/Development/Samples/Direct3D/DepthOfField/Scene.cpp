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
#include <FrmApplication.h>
#include <Direct3D/FrmFontD3D.h>
#include <Direct3D/FrmMesh.h>
#include <Direct3D/FrmPackedResourceD3D.h>
#include <Direct3D/FrmShader.h>
#include <Direct3D/FrmUserInterfaceD3D.h>
#include "Scene.h"

#define THERMALVISUALIZER

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
CombineShader::CombineShader()
{
    m_pCombineConstantBuffer = NULL;
	FocalDistRange.x = 1.55f;
	FocalDistRange.y = 3.0f;
	GaussSpread = 3.0f;
	GaussRho = 1.0f;
#ifndef THERMALVISUALIZER	
	m_CombineConstantBufferData.ThermalColors = FALSE;
#endif	
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CombineShader::Destroy()
{
    if ( m_pCombineConstantBuffer ) m_pCombineConstantBuffer->Release();
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
DownsizeShader::DownsizeShader()
{
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID DownsizeShader::Destroy()
{	
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject::SimpleObject()
{
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
    return new CSample( "DepthOfField" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_pLogoTexture = NULL;
    m_pTexture     = NULL;

	g_pSharpFBO			= NULL;
	g_pBlurFBO			= NULL;
	g_pScratchFBO		= NULL;

    m_pQuadVertexBuffer = NULL;
    m_pQuadIndexBuffer  = NULL;
	
	m_ShouldRotate		= TRUE;
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

		if( FALSE == m_DownsizeShader.Shader.Compile( "DownsizeShaderVS.cso", "DownsizeShaderPS.cso",
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

		if( FALSE == m_CombineShader.Shader.Compile( "CombineShaderVS.cso", "CombineShaderPS.cso",			
			                                         pShaderAttributes, nNumShaderAttributes ) )
			return FALSE;

        // Create constant buffer
        if( FALSE == FrmCreateConstantBuffer( sizeof(m_CombineShader.m_CombineConstantBufferData), 
                                              &m_CombineShader.m_CombineConstantBufferData,
                                              &m_CombineShader.m_pCombineConstantBuffer ) )
            return FALSE;                                              
	}

	
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: CreateFBO()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::CreateFBO( UINT32 nWidth, UINT32 nHeight, BOOL bMRT, BOOL bDepth, FrameBufferObject** ppFBO )
{
	(*ppFBO) = new FrameBufferObject;
	(*ppFBO)->m_nWidth  = nWidth;
	(*ppFBO)->m_nHeight = nHeight;
	
    D3D11_TEXTURE2D_DESC textureDesc;        
    ZeroMemory( &textureDesc, sizeof(textureDesc) );
    textureDesc.Width = nWidth;
    textureDesc.Height = nHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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


    // Create Texture 1 - if we are using an R32_FLOAT MRT for storing depth as a second attachment
    // since depth textures are not supported under D3D11 feature level 9_3
    if (bMRT)
    {
        textureDesc.Format = DXGI_FORMAT_R32_FLOAT;    
        if ( FAILED(D3DDevice()->CreateTexture2D( &textureDesc, NULL, &(*ppFBO)->m_pTexture1 ) ) )
        {
            return FALSE;
        }

        // Create Render Target View 1
        renderTargetViewDesc.Format = textureDesc.Format;    
        if ( FAILED(D3DDevice()->CreateRenderTargetView( (*ppFBO)->m_pTexture1.Get(), 
                                                         &renderTargetViewDesc, 
                                                         &(*ppFBO)->m_pTexture1RenderTargetView ) ) )
        {
            return FALSE;
        }

        // Create Render Shader Resource View 1
        shaderResourceViewDesc.Format = textureDesc.Format;    
        if ( FAILED(D3DDevice()->CreateShaderResourceView( (*ppFBO)->m_pTexture1.Get(),
                                                           &shaderResourceViewDesc,
                                                           &(*ppFBO)->m_pTexture1ShaderResourceView ) ) )
        {
            return FALSE;
        }

        // Create Sampler State 1
        if ( FAILED(D3DDevice()->CreateSamplerState( &samplerDesc, &(*ppFBO)->m_pTexture1Sampler ) ) )
        {
            return FALSE;
        }
    }

    // Create a depth/stencil texture for the depth attachment - only if one is needed.  Otherwise,
    // we will render without a depth buffer
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


    ID3D11RenderTargetView* pRTViews[2] = { pFBO->m_pTexture0RenderTargetView.Get(), 
                                            pFBO->m_pTexture1RenderTargetView.Get() };
    D3DDeviceContext()->OMSetRenderTargets( 2, pRTViews, pFBO->m_pDepthStencilView.Get() );
    
    // In every one of the render targets, we don't need the previous results so 
    // tell D3D to discard the contents.  This is a performance optimization to prevent the
    // driver from having to do a heavyweight resolve.
    D3DDeviceContext()->DiscardView( pFBO->m_pTexture0RenderTargetView.Get() );
    if (pFBO->m_pTexture1RenderTargetView.Get() != NULL)
    {
        D3DDeviceContext()->DiscardView( pFBO->m_pTexture1RenderTargetView.Get() );
    }
    
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
    ID3D11RenderTargetView* pRTViews[2] = { m_windowRenderTargetView.Get(), NULL };
    D3DDeviceContext()->OMSetRenderTargets( 2, pRTViews, m_windowDepthStencilView.Get() );

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
	m_UserInterface.AddFloatVariable( &m_CombineShader.GaussSpread, (char *)"Gauss filter spread" );
	m_UserInterface.AddFloatVariable( &m_CombineShader.FocalDistRange.x, (char *)"Focal distance" );
	m_UserInterface.AddFloatVariable( &m_CombineShader.FocalDistRange.y, (char *)"Focal range" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
//	m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Toggle thermal visualizer" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Decrease focal distance" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Increase focal distance" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Decrease focal range" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Increase focal range" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease gauss spread" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase gauss spread" );
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
        
	m_AmbientLight = FRMVECTOR4( 0.1f, 0.1f, 0.1f, 0.0f );
        
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

    FLOAT32 Quad[] =
	{
		+1.0, +1.0f, 1.0f, 1.0f,
		-1.0, +1.0f, 0.0f, 1.0f,
		+1.0, -1.0f, 1.0f, 0.0f,
		-1.0, -1.0f, 0.0f, 0.0f,
	};
    UINT32 indices[6] = { 0, 1, 2, 2, 1, 3 };
    if( FALSE == FrmCreateVertexBuffer( 4, sizeof(FLOAT32) * 4, &Quad[0], &m_pQuadVertexBuffer ) )
        return FALSE;

    if( FALSE == FrmCreateIndexBuffer( 6, sizeof(UINT32), &indices, &m_pQuadIndexBuffer ) )
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

	// Q = Zf / Zf - Zn
	m_CombineShader.NearFarQ.x = zNear;
	m_CombineShader.NearFarQ.y = zFar;
	m_CombineShader.NearFarQ.z = ( zFar / ( zFar - zNear ) );

	// To keep sizes consistent, scale the projection matrix in landscape oriention
	if( fAspect > 1.0f )
	{
		m_matProj.M(0,0) *= fAspect;
		m_matProj.M(1,1) *= fAspect;
	}

    // Setup our FBOs
    m_nOutOfFocusFBOWidth  = m_nWidth / 2;
    m_nOutOfFocusFBOHeight = m_nHeight / 2;

	if( FALSE == CreateFBO( m_nWidth, m_nHeight, TRUE, TRUE, &g_pSharpFBO ) )
		return FALSE;

	if( FALSE == CreateFBO( m_nOutOfFocusFBOWidth, m_nOutOfFocusFBOHeight, FALSE, FALSE, &g_pBlurFBO ) )
		return FALSE;

	if( FALSE == CreateFBO( m_nOutOfFocusFBOWidth, m_nOutOfFocusFBOHeight, FALSE, FALSE, &g_pScratchFBO ) )
		return FALSE;

	m_BlurShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_nWidth, 1.0f / (FLOAT32)m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_pTexture )     m_pTexture->Release();
    if( m_pLogoTexture ) m_pLogoTexture->Release();

    if( m_pQuadVertexBuffer ) m_pQuadVertexBuffer->Release();
    if( m_pQuadIndexBuffer ) m_pQuadIndexBuffer->Release();

	DestroyFBO( g_pSharpFBO );
	DestroyFBO( g_pBlurFBO );
	DestroyFBO( g_pScratchFBO );
	
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
}

//--------------------------------------------------------------------------------------
// Name: GetCameraPos27()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos( FLOAT32 fTime )
{
	FRMVECTOR3 BaseOffset = FRMVECTOR3( 4.5f, 2.0f, 0.0f );
	FRMVECTOR3 CurPosition = FRMVECTOR3( 1.7f * FrmCos( 0.25f * fTime ), 0.5f * FrmSin( 0.15f * fTime ), -1.9f * FrmSin( 0.25f * fTime ) );
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
#ifndef THERMALVISUALIZER	
	if( nPressedButtons & INPUT_KEY_1 )
	{
		m_CombineShader.ThermalColors = !m_CombineShader.ThermalColors;
	}
#endif

	if( nButtons & INPUT_KEY_2 )
	{
		// decrease focal distance
		FLOAT32 MIN_FOCAL_DIST = 0.5f; // actually 0.25, since FocalDist gets squared
		m_CombineShader.FocalDistRange.x -= 1.5f * ElapsedTime;
		m_CombineShader.FocalDistRange.x = max( MIN_FOCAL_DIST, m_CombineShader.FocalDistRange.x );
	}

	if( nButtons & INPUT_KEY_3 )
	{
		// increase focal distance
		FLOAT32 MAX_FOCAL_DIST = 3.5f; // actually 6.25, since FocalDist gets squared
		m_CombineShader.FocalDistRange.x += 1.5f * ElapsedTime;
		m_CombineShader.FocalDistRange.x = min( MAX_FOCAL_DIST, m_CombineShader.FocalDistRange.x );
	}

	if( nButtons & INPUT_KEY_4 )
	{
		// decrease range of clarity
		FLOAT32 MIN_FOCAL_RANGE = 0.4f;
		m_CombineShader.FocalDistRange.y -= 2.0f * ElapsedTime;
		m_CombineShader.FocalDistRange.y = max( MIN_FOCAL_RANGE, m_CombineShader.FocalDistRange.y );
	}

	if( nButtons & INPUT_KEY_5 )
	{
		// increase range of clarity
		FLOAT32 MAX_FOCAL_RANGE = 5.0f;
		m_CombineShader.FocalDistRange.y += 2.0f * ElapsedTime;
		m_CombineShader.FocalDistRange.y = min( MAX_FOCAL_RANGE, m_CombineShader.FocalDistRange.y );
	}


	if( nButtons & INPUT_KEY_6 )
	{
		m_CombineShader.GaussSpread -= 2.0f * ElapsedTime;
		m_CombineShader.GaussSpread = max( 0.0f, m_CombineShader.GaussSpread );
	}

	if( nButtons & INPUT_KEY_7 )
	{
		m_CombineShader.GaussSpread += 2.0f * ElapsedTime;
		m_CombineShader.GaussSpread = min( 3.0f, m_CombineShader.GaussSpread );
	}


	// update light position
	m_LightPos = FRMVECTOR3( 3.3f, 2.0f, -0.42f );

	// animate the camera
	static FLOAT32 TotalTime = 0.0f;
	if( m_ShouldRotate )
	{
		static FLOAT32 CameraSpeed = 1.0f;
		TotalTime += ElapsedTime * CameraSpeed;
	}

	// build camera transforms
	FRMVECTOR3 UpDir = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
	FRMVECTOR3 LookPos = GetCameraPos( TotalTime + 0.05f );
	m_CameraPos   = GetCameraPos( TotalTime );
	m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
	m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

	// update objects
	m_Object.Update( ElapsedTime, m_ShouldRotate );
}

//--------------------------------------------------------------------------------------
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::DrawScene()
{

	BeginFBO( g_pSharpFBO );

    // Clear just depth since we will draw to the whole framebuffer
    D3DDeviceContext()->ClearDepthStencilView( g_pSharpFBO->m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
    
    D3DDeviceContext()->RSSetState( m_DefaultRasterizerState.Get() );
    D3DDeviceContext()->OMSetDepthStencilState( m_DefaultDepthStencilState.Get(), 0 );

	DrawObject( &m_Object );

    D3DDeviceContext()->RSSetState( m_RasterizerState.Get() );

	EndFBO( g_pSharpFBO );
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
VOID CSample::GaussBlur( FrameBufferObject* TargetRT )
{
	// Blur in the X direction
	{
		BeginFBO( g_pScratchFBO );

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
        D3DDeviceContext()->PSSetShaderResources( 0, 1, TargetRT->m_pTexture0ShaderResourceView.GetAddressOf() );
        D3DDeviceContext()->PSSetSamplers( 0, 1, TargetRT->m_pTexture0Sampler.GetAddressOf() );
        
		RenderScreenAlignedQuad();

        // Unbind textures
        ID3D11ShaderResourceView* noTex[1] = { NULL };
        D3DDeviceContext()->PSSetShaderResources( 0, 1, noTex );
	}

	// Then blur in the Y direction
	{
		BeginFBO( TargetRT );

        m_BlurShader.Shader.Bind();
     
		FRMVECTOR2 StepSizeY = FRMVECTOR2( 0.0f, m_BlurShader.StepSize.y * m_CombineShader.GaussSpread );
        m_BlurShader.m_BlurConstantBufferData.StepSize = StepSizeY;
        m_BlurShader.m_pBlurConstantBuffer->Update( &m_BlurShader.m_BlurConstantBufferData );
        m_BlurShader.m_pBlurConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );


        // Bind textures
        D3DDeviceContext()->PSSetShaderResources( 0, 1, g_pScratchFBO->m_pTexture0ShaderResourceView.GetAddressOf() );
        D3DDeviceContext()->PSSetSamplers( 0, 1, g_pScratchFBO->m_pTexture0Sampler.GetAddressOf() );
        
		RenderScreenAlignedQuad();

        // Unbind textures
        ID3D11ShaderResourceView* noTex[1] = { NULL };
        D3DDeviceContext()->PSSetShaderResources( 0, 1, noTex );

		EndFBO( TargetRT );
	}
}

//--------------------------------------------------------------------------------------
// Name: CreateBlurredScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::CreateBlurredImage()
{
	// Part 1: downsize the framebuffer to a smaller render target
	{
		BeginFBO( g_pBlurFBO );

        D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
        float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        UINT32 sampleMask = 0xffffffff;
        D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        
		D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );
		
        m_DownsizeShader.Shader.Bind();
        
        // Bind textures
        D3DDeviceContext()->PSSetShaderResources( 0, 1, g_pSharpFBO->m_pTexture0ShaderResourceView.GetAddressOf() );
        D3DDeviceContext()->PSSetSamplers( 0, 1, g_pSharpFBO->m_pTexture0Sampler.GetAddressOf() );

		RenderScreenAlignedQuad();

        // Unbind textures
        ID3D11ShaderResourceView* noTex[1] = { NULL };
        D3DDeviceContext()->PSSetShaderResources( 0, 1, noTex );
        
	}

	// Part 2: blur
	GaussBlur( g_pBlurFBO );
}

//--------------------------------------------------------------------------------------
// Name: DrawCombinedScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::DrawCombinedScene()
{        
    // Set backbuffer as render target, do not use a depth buffer since we are just drawing
    // a full screen quad and text and don't need depth test
    D3DDeviceContext()->OMSetRenderTargets( 1, m_windowRenderTargetView.GetAddressOf(), NULL );

    // Discard the contents of the color buffer since we will draw the whole screen
    D3DDeviceContext()->DiscardView( m_windowRenderTargetView.Get() );

    D3DDeviceContext()->RSSetState( m_CullDisabledRasterizerState.Get() );
    float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    UINT32 sampleMask = 0xffffffff;
    D3DDeviceContext()->OMSetBlendState( m_DisabledBlendState.Get(), blendFactor, sampleMask );        
	D3DDeviceContext()->OMSetDepthStencilState( m_DisabledDepthStencilState.Get(), 0 );		

    FRMVECTOR2 FocalDistRange;
	FocalDistRange.x = pow( m_CombineShader.FocalDistRange.x, 2.0f );
	FocalDistRange.y = ( 1.0f / max( 0.00001f, m_CombineShader.FocalDistRange.y ) );

    m_CombineShader.Shader.Bind();

    m_CombineShader.m_CombineConstantBufferData.FocalDistRange = FocalDistRange;
    m_CombineShader.m_CombineConstantBufferData.NearQ = FRMVECTOR2( m_CombineShader.NearFarQ.x, m_CombineShader.NearFarQ.z );
    m_CombineShader.m_pCombineConstantBuffer->Update( &m_CombineShader.m_CombineConstantBufferData );
    m_CombineShader.m_pCombineConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );
    

    // Sharp Texture - 0
    D3DDeviceContext()->PSSetShaderResources( 0, 1, g_pSharpFBO->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 0, 1, g_pSharpFBO->m_pTexture0Sampler.GetAddressOf() );

    // Blurred Texture - 1
    D3DDeviceContext()->PSSetShaderResources( 1, 1, g_pBlurFBO->m_pTexture0ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 1, 1, g_pBlurFBO->m_pTexture0Sampler.GetAddressOf() );

    // Depth texture - 2
    D3DDeviceContext()->PSSetShaderResources( 2, 1, g_pSharpFBO->m_pTexture1ShaderResourceView.GetAddressOf() );
    D3DDeviceContext()->PSSetSamplers( 2, 1, g_pSharpFBO->m_pTexture1Sampler.GetAddressOf() );

    RenderScreenAlignedQuad();

    // Unbind textures
    ID3D11ShaderResourceView* noTex[3] = { NULL, NULL, NULL };
    D3DDeviceContext()->PSSetShaderResources( 0, 3, noTex );
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

	// Update the timer
	m_Timer.MarkFrame();

	// Render the user interface
	m_UserInterface.Render( m_Timer.GetFrameRate() );
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


