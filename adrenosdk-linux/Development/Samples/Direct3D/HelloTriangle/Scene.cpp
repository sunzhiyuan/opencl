//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: D3D11 Hello Triangle
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
#include "Scene.h"



//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Hello Triangle" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_constantBuffer = 0;
}



//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    {
        { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32_FLOAT },
        { "TEXCOORD", FRM_VERTEX_TEXCOORD0, DXGI_FORMAT_R32G32_FLOAT }
    };
    const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

    if ( FALSE == m_shader.Compile( "HelloTriangleVS.cso",
                                    "HelloTrianglePS.cso",
                                    pShaderAttributes, nNumShaderAttributes ) )
    {
        return FALSE;
    }

    float triangleVertices[] = 
    {
       -0.5f, -0.5f,    // Pos0
        0.0f,  0.0f,    // Tex0
        0.0f,  0.5f,    // Pos1
        0.5f,  1.0f,    // Tex1
        0.5f, -0.5f,    // Pos2
        1.0f,  0.0f,    // Tex2
    };

    unsigned short triangleIndices[] = 
    {
        0, 1, 2,
    };

    if ( FALSE == FrmCreateVertexBuffer(3, sizeof(float) * 4, triangleVertices, &m_vertexBuffer)) 
    {
        return FALSE;
    }

    if ( FALSE == FrmCreateIndexBuffer(3, sizeof(unsigned short), triangleIndices, &m_indexBuffer))
    {
        return FALSE;
    }

    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32) m_nHeight;
    m_constantBufferData.Perspective = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f);

    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 4.0f, 5.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_constantBufferData.View = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    if ( FALSE == FrmCreateConstantBuffer(sizeof(AppConstantBuffer), &m_constantBufferData, &m_constantBuffer))
    {
        return FALSE;
    }

    // 2x2 Image, 3 bytes per pixel (R, G, B)
    BYTE pixels[4 * 4] =
    {  
      255,   0,   0,  255, // Red
        0, 255,   0,  255, // Green
        0,   0, 255,  255, // Blue
      255, 255,   0,  255  // Yellow
    };

    if ( FALSE == FrmCreateTexture(2, 2, 1, DXGI_FORMAT_R8G8B8A8_UNORM, 0, pixels, sizeof(pixels), &m_texture))
    {
        return FALSE;
    }
    
    
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32) m_nHeight;
    m_constantBufferData.Perspective = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f);

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    delete m_vertexBuffer;
    delete m_indexBuffer;
    delete m_constantBuffer;
    delete m_texture;
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

    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

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

    // Bind the VS/PS
    m_shader.Bind();

    // Update the constant buffer and bind it
    m_constantBuffer->Update(&m_constantBufferData);
    m_constantBuffer->Bind(CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS);

    // Bind the vertex buffer/index buffer
    m_vertexBuffer->Bind(0);
    m_indexBuffer->Bind(0);

    // Bind the texture
    m_texture->Bind(0);
       
    FrmDrawIndexedVertices( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, 3, sizeof(unsigned short), 0);
}

