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
    return new CSample( "Bumped Reflection Mapping" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_pLogoTexture                       = NULL;

    m_pDiffuseMap                        = NULL;
    m_pNormalMap                         = NULL;
    m_pCubeMap                           = NULL;

    m_pBumpedReflectionConstantBuffer    = NULL;

    m_FresnelPower                       = 5.0f;
    m_SpecularPower                      = 20.0f;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Bumped reflection mapping shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION,  DXGI_FORMAT_R32G32B32A32_FLOAT },
            { "TEXCOORD", FRM_VERTEX_TEXCOORD0, DXGI_FORMAT_R32G32_FLOAT },
            { "TANGENT",  FRM_VERTEX_TANGENT,   DXGI_FORMAT_R32G32B32_FLOAT },
            { "BINORMAL", FRM_VERTEX_BINORMAL,  DXGI_FORMAT_R32G32B32_FLOAT },
            { "NORMAL",   FRM_VERTEX_NORMAL,    DXGI_FORMAT_R32G32B32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

	    if( FALSE == m_Shaders[BUMPED_REFLECTION].Compile( "BumpedReflectionMappingVS.cso", "BumpedReflectionMappingPS.cso",
                                                            pShaderAttributes, nNumShaderAttributes ) )
	    {
            return FALSE;
	    }
	
        if( FALSE == FrmCreateConstantBuffer( sizeof(m_BumpedReflectionConstantBufferData), &m_BumpedReflectionConstantBufferData,
                                              &m_pBumpedReflectionConstantBuffer ) )
        {
            return FALSE;
        }
    }

    // Skydome shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION,  DXGI_FORMAT_R32G32B32_FLOAT },
            { "NORMAL",   FRM_VERTEX_NORMAL,    DXGI_FORMAT_R32G32B32_FLOAT },
            { "TEXCOORD",   FRM_VERTEX_NORMAL,  DXGI_FORMAT_R32G32_FLOAT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

	    if( FALSE == m_Shaders[SKYDOME].Compile( "SkydomeVS.cso", "SkydomePS.cso",
                                                  pShaderAttributes, nNumShaderAttributes ) )
	    {
            return FALSE;
	    }
	
        if( FALSE == FrmCreateConstantBuffer( sizeof(m_SkydomeConstantBufferData), &m_SkydomeConstantBufferData,
                                              &m_pSkydomeConstantBuffer ) )
        {
            return FALSE;
        }
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

    m_pDiffuseMap = resource.GetTexture( "DiffuseMap" );
    m_pNormalMap = resource.GetTexture( "NormalMap" );
    m_pCubeMap = resource.GetCubeMap( "CubeMap" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;

	//hack in a title for first model
	m_UserInterface.SetHeading("Bumped Reflection with Fresnel");

    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );
    m_UserInterface.AddFloatVariable( &m_FresnelPower, "Fresnel Power" );
    m_UserInterface.AddFloatVariable( &m_SpecularPower, "Specular Power" );

    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Increase Fresnel Power" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Decrease Fresnel Power" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Increase Specular Power" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Decrease Specular Power" );

     // Load the meshes
    if( FALSE == m_Meshes[ SPHERE_MESH ].Load( "SphereTS.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ SPHERE_MESH ].MakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes[ SKYDOME_MESH ].Load( "Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes[ SKYDOME_MESH ].MakeDrawable() )
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
    m_CameraPos    = vCameraPosition;
    m_vLightLookAt = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vLightUp     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );


    // Initial state
    D3D11_RASTERIZER_DESC rdesc = CD3D11_RASTERIZER_DESC(D3D11_DEFAULT);    
    D3DDevice()->CreateRasterizerState(&rdesc, &m_BackFaceRasterizerState); 
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
    if( m_pBumpedReflectionConstantBuffer ) m_pBumpedReflectionConstantBuffer->Release();
    if( m_pSkydomeConstantBuffer )          m_pSkydomeConstantBuffer->Release();

    // Release textures
    if( m_pLogoTexture ) m_pLogoTexture->Release();
    if( m_pDiffuseMap )  m_pDiffuseMap->Release();
    if( m_pNormalMap )   m_pNormalMap->Release();
    if( m_pCubeMap )     m_pCubeMap->Release();

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

    if( nButtons & INPUT_KEY_1 )
    {
        m_FresnelPower += 0.5f;
    }
	
    if( nButtons & INPUT_KEY_2 )
    {
        m_FresnelPower -= 0.5f;

		m_FresnelPower = max(m_FresnelPower, 0.0f);
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_SpecularPower += 0.5f;
    }
	
    if( nButtons & INPUT_KEY_4 )
    {
        m_SpecularPower -= 0.5f;

		m_SpecularPower = max(m_SpecularPower, 0.0f);
    }


    
    // update light position
	m_LightPos = m_CameraPos;
	
    // Setup the mesh's camera relative model view, model view projection, and normal matrices
    FRMMATRIX4X4 matMeshRotate    = FrmMatrixRotate( fTime, FRMVECTOR3( 0.0f, 1.0f, 0.0f ) );
    FRMMATRIX4X4 matMeshRotate2   = FrmMatrixRotate( fTime, FRMVECTOR3( 1.0f, 0.0f, 0.0f ) );
    FRMMATRIX4X4 matMeshModel     = FrmMatrixTranslate( 0.0f, 0.0f, 0.0f );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate, matMeshModel );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate2, matMeshModel );

    m_matModel                    = matMeshModel;
    m_matCameraMeshModelView      = FrmMatrixMultiply( matMeshModel, m_matCameraView );
    m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
    m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );

    FRMMATRIX4X4 matSkyDome       = FrmMatrixMultiply( FrmMatrixScale( 8.0, 8.0, 8.0 ),  FrmMatrixTranslate( 0.0f, 0.0f, -10.0f ) );
    m_matSkyDomeModelViewProj     = FrmMatrixMultiply( matSkyDome, m_matCameraPersp );
    m_matSkyDomeNormal            = FrmMatrixNormal( matSkyDome );

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
    D3DDeviceContext()->OMSetRenderTargets( 1, m_windowRenderTargetView.GetAddressOf(), m_windowDepthStencilView.Get() );
    
    // Discard the color buffer, we draw to all fragments so this saves performance
    D3DDeviceContext()->DiscardView( m_windowRenderTargetView.Get() );
    
    // Clear the depth buffer
    D3DDeviceContext()->ClearDepthStencilView( m_windowDepthStencilView.Get(),  D3D11_CLEAR_DEPTH, 1.0f, 0 );
  
    m_Shaders[ BUMPED_REFLECTION ].Bind();

    // Render the bumped reflection mapped mesh
    m_BumpedReflectionConstantBufferData.MatModelViewProj = m_matCameraMeshModelViewProj;
    m_BumpedReflectionConstantBufferData.MatModelView = m_matCameraMeshModelView;
    m_BumpedReflectionConstantBufferData.MatModel = m_matModel;
    m_BumpedReflectionConstantBufferData.MatNormal = FRMMATRIX4X4( m_matCameraMeshNormal );
    m_BumpedReflectionConstantBufferData.EyePos = FRMVECTOR4( m_CameraPos, 1.0 );
    m_BumpedReflectionConstantBufferData.LightPos = FRMVECTOR4( m_LightPos, 1.0 );
    m_BumpedReflectionConstantBufferData.FresnelPower = m_FresnelPower;
    m_BumpedReflectionConstantBufferData.SpecularPower = m_SpecularPower;    
	
    m_pBumpedReflectionConstantBuffer->Update( &m_BumpedReflectionConstantBufferData ) ;
    m_pBumpedReflectionConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );

    m_pDiffuseMap->Bind( 0 );
    m_pNormalMap->Bind( 1 );
    m_pCubeMap->Bind( 2 );

    m_Meshes[ SPHERE_MESH ].Render( &m_Shaders[ BUMPED_REFLECTION ] );

    // Render the skydome

    // Draw backfaces of sphere
    D3DDeviceContext()->RSSetState( m_BackFaceRasterizerState.Get() );
    
    m_Shaders[ SKYDOME ].Bind();
    m_SkydomeConstantBufferData.MatModelViewProj = m_matSkyDomeModelViewProj;
    m_SkydomeConstantBufferData.MatNormal = FRMMATRIX4X4( m_matSkyDomeNormal );
    m_pSkydomeConstantBuffer->Update( &m_SkydomeConstantBufferData );
    m_pSkydomeConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS );

    m_pCubeMap->Bind( 0 );

    m_Meshes[ SKYDOME_MESH ].Render( &m_Shaders[ SKYDOME ] );

    // Reset state
    D3DDeviceContext()->RSSetState( m_RasterizerState.Get() );
			
	// Update the timer
    m_Timer.MarkFrame();
    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

