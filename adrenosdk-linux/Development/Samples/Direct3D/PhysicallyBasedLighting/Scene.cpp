//--------------------------------------------------------------------------------------
// Author:                 QUALCOMM, Adreno SDK
// Desc: Physically Based Lighting
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
#include <Direct3D/FrmUtilsD3D.h>
#include "Scene.h"



//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Physically Bassed Lighting" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
	m_nCurShaderIndex					 = 0;

    m_pLogoTexture                       = NULL;
    m_nCurMeshIndex                      = 0;

    m_pPhysicallyBasedLightingConstantBuffer = NULL;

	////////////
	m_SkyColor							 = FRMVECTOR3(0.9f, 0.1f, 0.1f);
	m_GroundColor						 = FRMVECTOR3(0.1f, 0.1f, 0.1f);
	m_MaterialSpecular 				     = FRMVECTOR3(0.6f, 0.5f, 0.5f);
	m_MaterialDiffuse 					 = FRMVECTOR3(0.9f, 0.3f, 0.3f);
	m_LightColor 						 = FRMVECTOR3(0.9f, 0.9f, 0.9f);
	m_MaterialGlossiness				 = 0.4f;

}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
    { 
        { "POSITION", FRM_VERTEX_POSITION, DXGI_FORMAT_R32G32B32_FLOAT },
        { "NORMAL",   FRM_VERTEX_NORMAL, DXGI_FORMAT_R32G32B32_FLOAT }
    };
    const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

	if( FALSE == m_Shaders[COOK_TORRANCE].Compile( "PhysicallyBasedLightingVS.cso", "CookTorrancePS.cso",
                                                   pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}

	if( FALSE == m_Shaders[COOK_TORRANCE_FULL].Compile( "PhysicallyBasedLightingVS.cso", "CookTorranceFullPS.cso",
                                                        pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}

	if( FALSE == m_Shaders[KEMEN_SZIRMAY_KALOS].Compile( "PhysicallyBasedLightingVS.cso", "KemenSzirmayKalosPS.cso",
                                                         pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}

    if( FALSE == m_Shaders[OPTIMIZED].Compile( "PhysicallyBasedLightingVS.cso", "OptimizedPS.cso",
                                                pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}

	if( FALSE == m_Shaders[BLINN_PHONG].Compile( "PhysicallyBasedLightingVS.cso", "BlinnPhongPS.cso",
                                                 pShaderAttributes, nNumShaderAttributes ) )
	{
        return FALSE;
	}

    if( FALSE == FrmCreateConstantBuffer( sizeof(m_PhysicallyBasedLightingConstantBufferData), &m_PhysicallyBasedLightingConstantBufferData,
                                          &m_pPhysicallyBasedLightingConstantBuffer ) )
    {
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
    // Initialize sample variables
    m_nCurMeshIndex  = CUBE_MESH;

    m_vLightPosition = FRMVECTOR3( 0.0f, 4.0f, 0.0f );

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

	//hack in a title for first model
	m_UserInterface.SetHeading("Cook Torrance");

    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );

    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Model" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Next Shader" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Increase Material Glossinesss" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Decrease Material Glossinesss" );

     // Load the meshes
    if( FALSE == m_Meshes[ CUBE_MESH ].Load( "Cube.mesh" ) )//
        return FALSE;
	if( FALSE == m_Meshes[ CUBE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ SPHERE_MESH ].Load( "Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ BUMPY_SPHERE_MESH ].Load( "BumpySphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ BUMPY_SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ TORUS_MESH ].Load( "Torus.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ TORUS_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ ROCKET_MESH ].Load( "Rocket.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ ROCKET_MESH ].MakeDrawable() )
        return FALSE;
	////////////

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Initialize the camera and light perspective matrices
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f );
    m_matLightPersp  = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, 1.0f, 5.0f, 20.0f );

    // Setup the camera view matrix
    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 0.0f, 10.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matCameraView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    // Setup the light look at and up vectors
    m_vLightLookAt = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vLightUp     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );

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
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f );
    m_matLightPersp  = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, 1.0f, 5.0f, 20.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matCameraPersp.M(0,0) *= fAspect;
        m_matCameraPersp.M(1,1) *= fAspect;
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

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_pPhysicallyBasedLightingConstantBuffer ) m_pPhysicallyBasedLightingConstantBuffer->Release();

    // Release textures
    if( m_pLogoTexture ) m_pLogoTexture->Release();

    // Free mesh
    FreeMeshes();
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

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
    {
        m_UserInterface.AdvanceState();
    }

    // Move shadow left
    if( nButtons & INPUT_DPAD_LEFT )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 0.0f, 0.0f, 1.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow right
    if( nButtons & INPUT_DPAD_RIGHT )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 0.0f, 0.0f, 1.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow up
    if( nButtons & INPUT_DPAD_DOWN )
    {
        FLOAT32 fAngle = -fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }

    // Move shadow down
    if( nButtons & INPUT_DPAD_UP )
    {
        FLOAT32 fAngle = fElapsedTime;
        FRMMATRIX4X4 matRotate = FrmMatrixRotate( fAngle, 1.0f, 0.0f, 0.0f );
        FRMVECTOR4 vTmp = FrmVector3Transform( m_vLightPosition, matRotate );
        m_vLightPosition = FRMVECTOR3( vTmp.x, vTmp.y, vTmp.z );
    }


	m_MaterialDiffuse = FRMVECTOR3(0.6f, 0.1f, 0.1f);
	
    
    // Change to next mesh
    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_nCurMeshIndex = ( m_nCurMeshIndex + 1 ) % NUM_MESHES;
    }

	

	if( nPressedButtons & INPUT_KEY_2 )
    {
        m_nCurShaderIndex = ( m_nCurShaderIndex + 1 ) % NUM_SHADERS;
		switch(m_nCurShaderIndex)
		{
		case COOK_TORRANCE:
			m_UserInterface.SetHeading("Cook Torrance");
		break;
		case COOK_TORRANCE_FULL:
			m_UserInterface.SetHeading("Cook Torrance Full");
		break;
		case KEMEN_SZIRMAY_KALOS:
			m_UserInterface.SetHeading("Kemen Szirmay Kalos");
		break;
		case OPTIMIZED:
			m_UserInterface.SetHeading("Optimized");
		break;
		case BLINN_PHONG:
			m_UserInterface.SetHeading("Blinn Phong");
		break;
		default:
				FrmAssert(0);
		}
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_MaterialGlossiness += 0.01f;
    }
	if( nButtons & INPUT_KEY_4 )
    {
        m_MaterialGlossiness -= 0.01f;

		m_MaterialGlossiness = max(m_MaterialGlossiness, 0.0f);
    }



    // Setup the mesh's camera relative model view, model view projection, and normal matrices
    FRMMATRIX4X4 matMeshRotate    = FrmMatrixRotate( fTime, FRMVECTOR3( 0.0f, 1.0f, 0.0f ) );
    FRMMATRIX4X4 matMeshRotate2   = FrmMatrixRotate( fTime, FRMVECTOR3( 1.0f, 0.0f, 0.0f ) );
    FRMMATRIX4X4 matMeshModel     = FrmMatrixTranslate( 0.0f, 0.0f, 0.0f );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate, matMeshModel );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate2, matMeshModel );

//	m_matWorld                    = matMeshModel;

    m_matCameraMeshModelView      = FrmMatrixMultiply( matMeshModel, m_matCameraView );
    m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
    m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );

	//m_matWorldNormal			  = FrmMatrixNormal( m_matWorld );

}



//--------------------------------------------------------------------------------------
// Name: FreeMeshes()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeMeshes()
{
    for( INT32 nMeshId = 0; nMeshId < NUM_MESHES; ++nMeshId )
    {
        m_Meshes[ nMeshId ].Destroy(); 
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
  
    m_Shaders[ m_nCurShaderIndex ].Bind();

    // Render the mesh
    m_PhysicallyBasedLightingConstantBufferData.matModelView = m_matCameraMeshModelView;
    m_PhysicallyBasedLightingConstantBufferData.matModelViewProj = m_matCameraMeshModelViewProj;
    m_PhysicallyBasedLightingConstantBufferData.matNormal = FRMMATRIX4X4( m_matCameraMeshNormal );
    m_PhysicallyBasedLightingConstantBufferData.vLightPos = FRMVECTOR4( m_vLightPosition.x, m_vLightPosition.y, m_vLightPosition.z, 1.0f );
    m_PhysicallyBasedLightingConstantBufferData.SkyColor = FRMVECTOR4( m_SkyColor, 1.0f );
	m_PhysicallyBasedLightingConstantBufferData.GroundColor = FRMVECTOR4( m_GroundColor, 1.0f );
    m_PhysicallyBasedLightingConstantBufferData.vMaterialSpecular = FRMVECTOR4( m_MaterialSpecular, 1.0f );
    m_PhysicallyBasedLightingConstantBufferData.vMaterialDiffuse = FRMVECTOR4( m_MaterialDiffuse, 1.0f );
    m_PhysicallyBasedLightingConstantBufferData.vLightColor = FRMVECTOR4( m_LightColor, 1.0f );
    m_PhysicallyBasedLightingConstantBufferData.fMaterialGlossiness = m_MaterialGlossiness;
	
    m_pPhysicallyBasedLightingConstantBuffer->Update( &m_PhysicallyBasedLightingConstantBufferData ) ;
    m_pPhysicallyBasedLightingConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );

    m_Meshes[ m_nCurMeshIndex ].Render( &m_Shaders[ m_nCurShaderIndex ] );
			
	// Update the timer
    m_Timer.MarkFrame();
    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

