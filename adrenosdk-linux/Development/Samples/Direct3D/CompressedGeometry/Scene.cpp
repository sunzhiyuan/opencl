//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: Geometry Compression
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
#include <Direct3D/FrmUtilsD3D.h>
#include "Scene.h"



//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Compressed Geometry" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
	m_nCurShaderIndex					 = 0;
	m_MaterialDiffuse					 = FRMVECTOR3(0.6f, 0.1f, 0.1f);
    m_pLogoTexture                       = NULL;
    m_nCurMeshIndex                      = 0;

    m_pCompressedGeometryConstantBuffer  = NULL;

    m_hDiffuseSpecularTexId              = 0;
    m_hDiffuseSpecularBufId              = 0;
    m_hDiffuseSpecularDepthBufId         = 0;

    m_nCurMeshIndex                      = 0;

}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION,  DXGI_FORMAT_R16G16B16A16_SNORM },
            { "NORMAL",   FRM_VERTEX_NORMAL, DXGI_FORMAT_R8G8B8A8_UINT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == m_CompressionShaders[COMPRESS_F16].Compile( "CompressedGeometryF16VS.cso", "CompressedGeometryF16PS.cso",                                                  
	                                                pShaderAttributes, nNumShaderAttributes ) )
        {
            return FALSE;
        }
    }

    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "POSITION", FRM_VERTEX_POSITION,  DXGI_FORMAT_R8G8B8A8_UINT },
            { "NORMAL",   FRM_VERTEX_NORMAL, DXGI_FORMAT_R8G8B8A8_UINT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

	    if( FALSE == m_CompressionShaders[COMPRESS_8].Compile( "CompressedGeometry8VS.cso", "CompressedGeometry8PS.cso",
                                                      pShaderAttributes, nNumShaderAttributes ) )
	    {
            return FALSE;
	    }
    }
    
    if ( FALSE == FrmCreateConstantBuffer(sizeof(CompressedGeometryConstantBuffer), &m_CompressedGeometryConstantBufferData,
                                          &m_pCompressedGeometryConstantBuffer))
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

    m_vLightPosition = FRMVECTOR3( 0.0f, 8.0f, 0.01f );

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
	m_UserInterface.SetHeading("Compressed using 16 bit Compression Transform");

    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );

    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Model" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Next Shader Compression Type" );

	m_CurMeshArray = m_MeshesF16;
     // Load the meshes
    if( FALSE == m_MeshesF16[ CUBE_MESH ].Load( "Cube.mesh" ) )//
        return FALSE;
	if( FALSE == m_MeshesF16[ CUBE_MESH ].CompressionTransformPosition16bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshesF16[ SPHERE_MESH ].Load( "Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshesF16[ SPHERE_MESH ].CompressionTransformPosition16bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshesF16[ BUMPY_SPHERE_MESH ].Load( "BumpySphere.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshesF16[ BUMPY_SPHERE_MESH ].CompressionTransformPosition16bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshesF16[ TORUS_MESH ].Load( "Torus.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshesF16[ TORUS_MESH ].CompressionTransformPosition16bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_MeshesF16[ ROCKET_MESH ].Load( "Rocket.mesh" ) )
        return FALSE;
    if( FALSE == m_MeshesF16[ ROCKET_MESH ].CompressionTransformPosition16bitMakeDrawable() )
        return FALSE;	
		//////////////
	 if( FALSE == m_Meshes8[ CUBE_MESH ].Load( "Cube.mesh" ) )
        return FALSE;
	if( FALSE == m_Meshes8[ CUBE_MESH ].CompressionTransformPosition8bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes8[ SPHERE_MESH ].Load( "Sphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes8[ SPHERE_MESH ].CompressionTransformPosition8bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes8[ BUMPY_SPHERE_MESH ].Load( "BumpySphere.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes8[ BUMPY_SPHERE_MESH ].CompressionTransformPosition8bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes8[ TORUS_MESH ].Load( "Torus.mesh" ) )
        return FALSE;
    if( FALSE == m_Meshes8[ TORUS_MESH ].CompressionTransformPosition8bitMakeDrawable() )
        return FALSE;
    if( FALSE == m_Meshes8[ ROCKET_MESH ].Load( "Rocket.mesh" ) )
        return FALSE;
	if( FALSE == m_Meshes8[ ROCKET_MESH ].CompressionTransformPosition8bitMakeDrawable() )
        return FALSE;

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


    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_pCompressedGeometryConstantBuffer ) m_pCompressedGeometryConstantBuffer->Release();

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

	// Change to next shader
    if( nPressedButtons & INPUT_KEY_2 )
    {
        m_nCurShaderIndex = ( m_nCurShaderIndex + 1 ) % NUM_SHADER;

		switch(m_nCurShaderIndex)
		{
			case COMPRESS_F16:

				m_UserInterface.SetHeading("Compressed using 16 bit Compression Transform");
				m_CurMeshArray = m_MeshesF16;
				m_MaterialDiffuse = FRMVECTOR3(0.6f, 0.1f, 0.1f);
			break;
			case COMPRESS_8:

				m_UserInterface.SetHeading("Compressed using 8 bit Compression Transform");
				m_CurMeshArray = m_Meshes8;
				m_MaterialDiffuse = FRMVECTOR3(0.6f, 0.6f, 0.6f);
			break;
		}
		
    }
    // Change to next mesh
    if( nPressedButtons & INPUT_KEY_1 )
    {
        m_nCurMeshIndex = ( m_nCurMeshIndex + 1 ) % NUM_MESHES;
    }

    // Setup the mesh's camera relative model view, model view projection, and normal matrices
    FRMMATRIX4X4 matMeshRotate    = FrmMatrixRotate( fTime, FRMVECTOR3( 0.0f, 1.0f, 0.0f ) );
    FRMMATRIX4X4 matMeshRotate2   = FrmMatrixRotate( fTime, FRMVECTOR3( 1.0f, 0.0f, 0.0f ) );
    FRMMATRIX4X4 matMeshModel     = FrmMatrixTranslate( 0.0f, 0.0f, 0.0f );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate, matMeshModel );
    matMeshModel                  = FrmMatrixMultiply( matMeshRotate2, matMeshModel );
    m_matCameraMeshModelView      = FrmMatrixMultiply( matMeshModel, m_matCameraView );
    m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
    m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );

}



//--------------------------------------------------------------------------------------
// Name: FreeMeshes()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::FreeMeshes()
{
    for( INT32 nMeshId = 0; nMeshId < NUM_MESHES; ++nMeshId )
    {
        m_MeshesF16[ nMeshId ].Destroy(); 
		m_Meshes101012[ nMeshId ].Destroy(); 
		m_Meshes24[ nMeshId ].Destroy(); 
		m_Meshes8[ nMeshId ].Destroy(); 
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

    m_CompressionShaders[ m_nCurShaderIndex ].Bind();    
	//grab the compression transfom matrix
	FRMMATRIX4X4 ct_mat = FrmMatrixInverse(FRMMATRIX4X4(m_CurMeshArray[ m_nCurMeshIndex ].GetCTMatrix()));
	ct_mat = FrmMatrixTranspose(ct_mat);

    // Render the mesh
    m_CompressedGeometryConstantBufferData.vAmbient = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
    m_CompressedGeometryConstantBufferData.vDiffuse = FRMVECTOR4( m_MaterialDiffuse.x, m_MaterialDiffuse.y, m_MaterialDiffuse.z, 1.0 );
    m_CompressedGeometryConstantBufferData.vSpecular = FRMVECTOR4( 1.0f, 0.6f, 0.65f, 10.0f );
    m_CompressedGeometryConstantBufferData.matModelView = m_matCameraMeshModelView;
    m_CompressedGeometryConstantBufferData.matCT = ct_mat;
    m_CompressedGeometryConstantBufferData.matModelViewProj = m_matCameraMeshModelViewProj;
    m_CompressedGeometryConstantBufferData.matNormal = FRMMATRIX4X4( m_matCameraMeshNormal );
    m_CompressedGeometryConstantBufferData.vLightPos = FRMVECTOR4( m_vLightPosition.x, m_vLightPosition.y, m_vLightPosition.z, 1.0f );
    m_pCompressedGeometryConstantBuffer->Update( &m_CompressedGeometryConstantBufferData );
    m_pCompressedGeometryConstantBuffer->Bind( CFrmConstantBuffer::BindFlagVS | CFrmConstantBuffer::BindFlagPS );


    m_CurMeshArray[ m_nCurMeshIndex ].Render( &m_CompressionShaders[ m_nCurShaderIndex] );
   
	// Update the timer
    m_Timer.MarkFrame();
    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

