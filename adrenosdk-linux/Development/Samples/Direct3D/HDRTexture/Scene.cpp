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

// Calculates log2 of number.  
float Log2( float n )  
{  
    // log(n)/log(2) is log2.  
    return log( n ) / log( 2.0f );  
}
//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "HDR Texture" );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader::MaterialShader()
{
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
	m_pLogoTexture = NULL;
	m_HDRTexture[0] = m_HDRTexture[1] = NULL;
    m_pHDRTextureConstantBuffer = NULL;
	m_Exposure = 1.0f;

    m_pQuadIndexBuffer = NULL;
    m_pQuadVertexBuffer = NULL;

	m_nWidth = 800;
	m_nHeight = 480;
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

	// Create shader for 8 8 8 8
	{
		if( FALSE == m_MaterialShader.Shader.Compile( "HDRTextureVS.cso", "HDRTexturePS.cso",
                                                      pShaderAttributes, nNumShaderAttributes ) )
			return FALSE;

        if( FALSE == FrmCreateConstantBuffer( sizeof(m_HDRTextureConstantBufferData), &m_HDRTextureConstantBufferData,
                                              &m_pHDRTextureConstantBuffer ) )
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
	if( FALSE == resource.LoadFromFile( "HDRTextures.pak" ) )
		return FALSE;

	m_HDRTexture[0] = resource.GetTexture( "HDRTexture1" );
	m_HDRTexture[1] = resource.GetTexture( "HDRTexture2" );


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
	m_UserInterface.AddFloatVariable( &m_Exposure, (char *)"Exposure" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Decrease exposure" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Increase exposure" );
	
	m_UserInterface.SetHeading("HDR Using 8 8 8 8 Textures");

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
	// Free shader memory
	m_MaterialShader.Destroy();

	// Free texture memory
	if( m_pLogoTexture ) m_pLogoTexture->Release();
	if( m_HDRTexture[0] ) m_HDRTexture[0]->Release();
	if( m_HDRTexture[1] ) m_HDRTexture[1]->Release();

    if ( m_pQuadVertexBuffer ) m_pQuadVertexBuffer->Release();
    if ( m_pQuadIndexBuffer ) m_pQuadIndexBuffer->Release();
	
    if ( m_pHDRTextureConstantBuffer ) m_pHDRTextureConstantBuffer->Release();

	resource.Destroy();
	
	m_Font.Destroy();
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


	if( nButtons & INPUT_KEY_1 )
	{
		m_Exposure -= 3.0f * ElapsedTime;
		m_Exposure = max( 0.0001f, m_Exposure );
	}

	if( nButtons & INPUT_KEY_2 )
	{
		m_Exposure += 3.0f * ElapsedTime;
		m_Exposure = min( 60.0f, m_Exposure );
	}
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

    m_MaterialShader.Shader.Bind();
	{
        m_HDRTextureConstantBufferData.Exposure = m_Exposure;
        m_pHDRTextureConstantBuffer->Update( &m_HDRTextureConstantBufferData );
        m_pHDRTextureConstantBuffer->Bind( CFrmConstantBuffer::BindFlagPS );

        FRMVECTOR2 CardSize = FRMVECTOR2( 0.4f, 0.8f );
		FRMVECTOR2 CardPos1 = FRMVECTOR2( -0.475f, 0.0f );

        m_HDRTexture[0]->Bind( 0 );
		RenderScreenAlignedQuad( CardPos1 - CardSize, CardPos1 + CardSize );

		FRMVECTOR2 CardPos2 = FRMVECTOR2( 0.475f, 0.0f );
		m_HDRTexture[1]->Bind( 0 );
		RenderScreenAlignedQuad( CardPos2 - CardSize, CardPos2 + CardSize );
	}
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    // Bind the backbuffer as a render target
    D3DDeviceContext()->OMSetRenderTargets( 1, m_windowRenderTargetView.GetAddressOf(), m_windowDepthStencilView.Get() );

    // Clear the backbuffer to a solid color, and reset the depth stencil.
    const float clearColor[4] = { 0.071f, 0.04f, 0.561f, 1.0f };
    D3DDeviceContext()->ClearRenderTargetView( m_windowRenderTargetView.Get(), clearColor );
    D3DDeviceContext()->ClearDepthStencilView( m_windowDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

	DrawScene();

	// Update the timer
	m_Timer.MarkFrame();

	// Render the user interface
	m_UserInterface.Render( m_Timer.GetFrameRate() );
}
