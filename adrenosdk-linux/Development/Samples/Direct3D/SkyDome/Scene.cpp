//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: Skydome
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
#include <Direct3D/FrmUtilsD3D.h>
#include "Scene.h"
#include "Scene\HeightField.hpp"


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
    return new CSample( "Sky Dome" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
	m_usingHoffman						 = true;
	m_nCurShaderIndex					 = 0;
    m_hModelViewMatrixLoc                = 0;
    m_hModelViewProjMatrixLoc            = 0;
    m_hLightPositionLoc                  = 0;

    m_pLogoTexture                       = NULL;

}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
	for(int i=0;i<10;++i)
	{
		m_shaderParamsArray[i] = 1.0;
	}
	if( !m_HoffmanScatter.create() )
	{
		return FALSE;
	}

	m_shaderParamsArray[0] = m_HoffmanScatter.getHg( );
	m_shaderParamsArray[1] = m_HoffmanScatter.getInscattering(  );
	m_shaderParamsArray[2] = m_HoffmanScatter.getMie(  );
	m_shaderParamsArray[3] = m_HoffmanScatter.getTurbidity(  );

	m_shaderParamsArray[4] = 0.01f; 
	m_shaderParamsArray[5] = 0.002f;
	m_SkyDome.create( 5160.0, 1000.0, 60, 100, 580.0);


    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Initialize sample variables
  //  m_nCurMeshIndex  = CUBE_MESH;

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
    m_UserInterface.AddOverlay( m_pLogoTexture, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f, 1.0f );

    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Help" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Increase Hg param " );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Decrease Hg param " );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, "Increase InscatteringMultiplier param" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, "Decrease InscatteringMultiplier param" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, "Increase MieMultiplier param" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, "Decrease MieMultiplier param" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, "Increase Turbitity param" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, "Decrease Turbitity param" );
//	m_UserInterface.AddHelpKey( FRM_FONT_KEY_9, "Change Shader Hoffman/Preetham" );

	// Create a new terrain height field and landscape
	UINT32 nWidth, nHeight;
    BYTE*  pHeightMapData = LoadTGA( "heights.tga", &nWidth, &nHeight, 3 );
    if( NULL == pHeightMapData )
          return FALSE;

    HeightField heightField;
                heightField.createFromRGBImage  ( pHeightMapData
                                                , nWidth 
                                                , nHeight 
                                                , 2.0
                                                , 40.0
                                                );

	delete [] pHeightMapData;

	BYTE*  pTextureData = LoadTGA( "terrain.tga", &nWidth, &nHeight, 4 );
    if( NULL == pTextureData )
		return FALSE;

	UINT32 nNormalsWidth, nNormalsHeight;
	BYTE*  pNormalMapData = LoadTGA( "normals.tga", &nNormalsWidth, &nNormalsHeight, 4 );
    if( NULL == pNormalMapData )
		return FALSE;


    m_terrain.create( heightField );

	mover.SetPosition( FRMVECTOR3( -((float)m_terrain.getTotalWidth())/2.0f, -70.0f, -((float)m_terrain.getTotalHeight())/2.0f));
	m_terrain.GenTerrainAndNormalTextures( pTextureData, nWidth, nHeight, 
							pNormalMapData, nNormalsWidth, nNormalsHeight);

	delete [] pTextureData;
	delete [] pNormalMapData;

	////////////////

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Initialize the camera and light perspective matrices
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 6000.0f );
  //  m_matLightPersp  = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, 1.0f, 5.0f, 20.0f );

    // Initialize the viewport
#ifdef TEMP
    glViewport( 0, 0, m_nWidth, m_nHeight );
#endif

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
    rdesc.CullMode = D3D11_CULL_NONE; // No culling for the terrain
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
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 6000.0f );

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
    // Release textures
    if( m_pLogoTexture ) m_pLogoTexture->Release();
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

	mover.Update( m_Input, fTime);

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

	if( nButtons & INPUT_KEY_1 )
    {
		if(m_usingHoffman)
			m_shaderParamsArray[0] = IncreaseParam( m_shaderParamsArray[0] );
		else
			m_shaderParamsArray[4] = IncreaseParam( m_shaderParamsArray[4] );
    }
	if( nButtons & INPUT_KEY_2 )
    {
		if(m_usingHoffman)
			m_shaderParamsArray[0] = DecreaseParam( m_shaderParamsArray[0] );
		else
			m_shaderParamsArray[4] = DecreaseParam( m_shaderParamsArray[4] );
    }
	if( nButtons & INPUT_KEY_3 )
    {
		if(m_usingHoffman)
			m_shaderParamsArray[1] = IncreaseParam( m_shaderParamsArray[1] );
		else
			m_shaderParamsArray[5] = IncreaseParam( m_shaderParamsArray[5] );
    }
	if( nButtons & INPUT_KEY_4 )
    {
		if(m_usingHoffman)
			m_shaderParamsArray[1] = DecreaseParam( m_shaderParamsArray[1] );
		else
			m_shaderParamsArray[5] = DecreaseParam( m_shaderParamsArray[5] );
    }
	if( nButtons & INPUT_KEY_5 )
    {
        m_shaderParamsArray[2] = IncreaseParam( m_shaderParamsArray[2] );
    }
	if( nButtons & INPUT_KEY_6 )
    {
        m_shaderParamsArray[2] = DecreaseParam( m_shaderParamsArray[2] );
    }
	if( nButtons & INPUT_KEY_7 )
    {
        m_shaderParamsArray[3] = IncreaseParam( m_shaderParamsArray[3] );
    }
	if( nButtons & INPUT_KEY_8 )
    {
        m_shaderParamsArray[3] = DecreaseParam( m_shaderParamsArray[3] );
    }
	if( nPressedButtons & INPUT_KEY_9 )
    {
       //Put back to turn on preetham m_usingHoffman = !m_usingHoffman;
    }


	m_HoffmanScatter.setHg( m_shaderParamsArray[0] );
	m_HoffmanScatter.setInscattering( m_shaderParamsArray[1] );
	m_HoffmanScatter.setMie( m_shaderParamsArray[2] );
	m_HoffmanScatter.setTurbidity( m_shaderParamsArray[3] );

    // Setup the mesh's camera relative model view, model view projection, and normal matrices
    FRMMATRIX4X4 matMeshModel     = FrmMatrixTranslate( 0.0f, 0.0f, 0.0f );

	m_matCameraView = mover.GetInverseWorldMatrix();

    m_matCameraMeshModelView      = FrmMatrixMultiply( matMeshModel, m_matCameraView );
    m_matCameraMeshModelViewProj  = FrmMatrixMultiply( m_matCameraMeshModelView, m_matCameraPersp );
    m_matCameraMeshNormal         = FrmMatrixNormal( m_matCameraMeshModelView );

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

    m_Timer.MarkFrame(); 
    // Render the user interface
	//FrmSetVertexBuffer( NULL );

	FRMMATRIX4X4 matCameraMeshModelViewNotMoving = m_matCameraMeshModelView;
	FRMMATRIX4X4 matCameraMeshModelViewProjNotMoving;

	matCameraMeshModelViewNotMoving.M(3,0) = 0.0f;
	matCameraMeshModelViewNotMoving.M(3,1) = 0.0f;
	matCameraMeshModelViewNotMoving.M(3,2) = 0.0f;

    matCameraMeshModelViewProjNotMoving  = FrmMatrixMultiply( matCameraMeshModelViewNotMoving, m_matCameraPersp );
	

	{
		m_HoffmanScatter.SetViewAndProjMat( matCameraMeshModelViewNotMoving, matCameraMeshModelViewProjNotMoving );
		m_HoffmanScatter.renderSkyLight( m_SkyDome.getSunDirection()
							 , (const float*)m_SkyDome.getVertices()
                             , (unsigned int) m_SkyDome.getVertexCount()
							 , m_SkyDome.getIndices()
							 , (unsigned int) m_SkyDome.getIndexCount()
							 );

		m_HoffmanScatter.SetViewAndProjMat( m_matCameraMeshModelView, m_matCameraMeshModelViewProj );
		m_HoffmanScatter.renderTerrain( m_SkyDome.getSunDirection()
							 , (const float*)m_terrain.getVertices()
							 , m_terrain.getIndices()
							 , (const float*)m_terrain.getTexCoords()
                             , (unsigned int) m_terrain.getVertexCount()
							 , (unsigned int) m_terrain.getIndexCount()
							 , m_terrain.getTerrainMap()
							 , m_terrain.getNormalMap()
							 );
	}

	m_UserInterface.Render( m_Timer.GetFrameRate() ); 	

}

