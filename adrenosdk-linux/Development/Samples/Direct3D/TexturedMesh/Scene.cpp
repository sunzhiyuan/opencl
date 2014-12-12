//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: D3D11 Textured Mesh
// Author:                 QUALCOMM, Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include <FrmApplication.h>
#include <FrmUtils.h>
#include <Direct3D/FrmMesh.h>
#include <Direct3D/FrmShader.h>
#include <Direct3D/FrmResourceD3D.h>
#include <Direct3D/FrmFontD3D.h>
#include <Direct3D/FrmUserInterfaceD3D.h>
#include "Scene.h"



//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Textured Mesh" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_constantBuffer = 0;
    m_pTexture = NULL;
    m_pLogoTexture = NULL;
}



//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{    

    // Create the font
    if ( FALSE == m_Font.Create( "Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceD3D resource;
    if ( FALSE == resource.LoadFromFile( "Textures.pak" ) )
    {
        return FALSE;
    }

    m_pTexture = resource.GetTexture( "Brick" );
    m_pLogoTexture = resource.GetTexture( "Logo" );

    if ( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;

    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5, 
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );

    
    // Compile the shader
    if ( FALSE == m_shader.Compile( "TexturedMeshVS.cso",
                                    "TexturedMeshPS.cso" ) )
    {
        return FALSE;
    }

    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32) m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f);

    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 1.0f, 2.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matModelView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    if ( FALSE == FrmCreateConstantBuffer(sizeof(AppConstantBuffer), &m_constantBufferData, &m_constantBuffer))
    {
        return FALSE;
    }

    
    if ( FALSE == m_mesh.Load( "Cube.mesh" ) )
    {
        return FALSE;
    }

    
    if ( FALSE == m_mesh.MakeDrawable( &resource ) )
    {
        return FALSE;
    }

    // Initial state
    D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    rdesc.FrontCounterClockwise = TRUE; // Change the winding direction to match GL    
    D3DDevice()->CreateRasterizerState(&rdesc, &m_RasterizerState);
    D3DDeviceContext()->RSSetState(m_RasterizerState.Get());
        
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32) m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f);


    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    delete m_constantBuffer;

    if ( m_pTexture ) m_pTexture->Release();
    if ( m_pLogoTexture ) m_pLogoTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fTime        = m_Timer.GetFrameTime();
	FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();
    
    m_matModelView = FrmMatrixMultiply(FrmMatrixRotate(fElapsedTime, 0.0f, 1.0f, 0.0f), m_matModelView);

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user input
    if ( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

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

    // Update the constant buffer and bind it
    m_constantBufferData.matModelViewProj = FrmMatrixMultiply( m_matModelView, m_matProj );
    m_constantBuffer->Update(&m_constantBufferData);
    m_constantBuffer->Bind(CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS);
    
    m_pTexture->Bind( 0 );

    // Render the mesh
    m_mesh.Render( &m_shader );    
    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

