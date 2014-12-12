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
#include <FrmFile.h>

#include <OpenGLES/FrmGLES3.h>  // OpenGL ES 3 headers here

#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <OpenGLES/FrmUtilsGLES.h>
#include "Scene.h"
#include "scene/HeightField.hpp"

GLuint skyVAO, skyVBO, skyIBO;

//--------------------------------------------------------------------------------------
// Name: LoadTGA()
// Desc: Helper function to load a 32-bit TGA image file
//--------------------------------------------------------------------------------------
BYTE* LoadTGA( const CHAR* strFileName, UINT32* pWidth, UINT32* pHeight, UINT32* nFormat )
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
	FRM_FILE* pFile;
    if( FALSE == FrmFile_Open( strFileName, FRM_FILE_READ, &pFile ) )
        return NULL;

	FrmFile_Read( pFile, &header, sizeof(header) );
    UINT32 nPixelSize = header.PixelDepth / 8;
    (*pWidth)  = header.ImageWidth;
    (*pHeight) = header.ImageHeight;
    (*nFormat) = nPixelSize == 3 ? GL_RGB : GL_RGBA;

    BYTE* pBits = new BYTE[ nPixelSize * header.ImageWidth * header.ImageHeight ];
	FrmFile_Read( pFile, pBits, nPixelSize * header.ImageWidth * header.ImageHeight );
	FrmFile_Close( pFile );

    // Convert the image from BGRA to RGBA
    BYTE* p = pBits;
    for( UINT32 y=0; y<header.ImageHeight; y++ )
    {
        for( UINT32 x=0; x<header.ImageWidth; x++ )
        {
            BYTE temp = p[2]; p[2] = p[0]; p[0] = temp;
            p += nPixelSize;
        }
    }

    return pBits;
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

    m_hOverlayShader                     = 0;
    m_pLogoTexture                       = NULL;

	m_OpenGLESVersion				     = GLES3;

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

	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CW );//original demo build around backwards windings

	m_shaderParamsArray[0] = m_HoffmanScatter.getHg( );
	m_shaderParamsArray[1] = m_HoffmanScatter.getInscattering(  );
	m_shaderParamsArray[2] = m_HoffmanScatter.getMie(  );
	m_shaderParamsArray[3] = m_HoffmanScatter.getTurbidity(  );

	m_shaderParamsArray[4] = 0.01f; 
	m_shaderParamsArray[5] = 0.002f;
	m_SkyDome.create( 5160.0, 1000.0, 60, 100, 580.0);

	// init skydome
	glGenVertexArrays(1, &skyVAO);
		
	glGenBuffers(1, &skyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FRMVECTOR3) * m_SkyDome.getVertexCount(), m_SkyDome.getVertices(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &skyIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * m_SkyDome.getIndexCount(), m_SkyDome.getIndices(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/SkyDome.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
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
	UINT32 nWidth, nHeight, nFormat;
    BYTE*  pHeightMapData = LoadTGA( "Samples/Textures/heights.tga", &nWidth, &nHeight, &nFormat );
    if( NULL == pHeightMapData )
          return FALSE;

    HeightField heightField;
                heightField.createFromRGBImage  ( pHeightMapData
                                                , nWidth 
                                                , nHeight 
                                                , 2.0
                                                , 40.0
                                                );

	delete pHeightMapData;

	BYTE*  pTextureData = LoadTGA( "Samples/Textures/terrain.tga", &nWidth, &nHeight, &nFormat );
    if( NULL == pTextureData )
		return FALSE;

	UINT32 nNormalsWidth, nNormalsHeight, nNormalsFormat;
	BYTE*  pNormalMapData = LoadTGA( "Samples/Textures/normals.tga", &nNormalsWidth, &nNormalsHeight, &nNormalsFormat );
    if( NULL == pNormalMapData )
		return FALSE;


    m_terrain.create( heightField );

	mover.SetPosition( FRMVECTOR3( -((float)m_terrain.getTotalWidth())/2.0f, -70.0f, -((float)m_terrain.getTotalHeight())/2.0f));
	m_terrain.GenTerrainAndNormalTextures( pTextureData, nWidth, nHeight, 
							pNormalMapData, nNormalsWidth, nNormalsHeight);

	delete pTextureData;
	delete pNormalMapData;

	////////////////

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Initialize the camera and light perspective matrices
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matCameraPersp = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 6000.0f );
  //  m_matLightPersp  = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, 1.0f, 5.0f, 20.0f );

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Setup the camera view matrix
    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 0.0f, 10.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matCameraView = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    // Setup the light look at and up vectors
    m_vLightLookAt = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    m_vLightUp     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );

    // Setup GL state
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );

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

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Delete shader programs   
    if( m_hOverlayShader ) glDeleteProgram( m_hOverlayShader );

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
// Name: CheckFrameBufferStatus()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::CheckFrameBufferStatus()
{
    GLenum nStatus;
    nStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch( nStatus )
    {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            break;
        default:
            FrmAssert(0);
    }
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_Timer.MarkFrame(); 
    // Render the user interface
	//FrmSetVertexBuffer( NULL );

	FRMMATRIX4X4 matCameraMeshModelViewNotMoving = m_matCameraMeshModelView;
	FRMMATRIX4X4 matCameraMeshModelViewProjNotMoving;

	matCameraMeshModelViewNotMoving.M(3,0) = 0.0f;
	matCameraMeshModelViewNotMoving.M(3,1) = 0.0f;
	matCameraMeshModelViewNotMoving.M(3,2) = 0.0f;

    matCameraMeshModelViewProjNotMoving  = FrmMatrixMultiply( matCameraMeshModelViewNotMoving, m_matCameraPersp );
	

	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CW );//original demo build around backwards windings


	{
		FRMVECTOR3 sunDirection = m_SkyDome.getSunDirection();
 
 		m_HoffmanScatter.SetViewAndProjMat( matCameraMeshModelViewNotMoving, matCameraMeshModelViewProjNotMoving );
 		m_HoffmanScatter.renderSkyLight( sunDirection
 							 , skyVAO
 							 , skyVBO
 							 , skyIBO
 							 , m_SkyDome.getIndexCount()
 							 );

		m_HoffmanScatter.SetViewAndProjMat( m_matCameraMeshModelView, m_matCameraMeshModelViewProj );
		m_HoffmanScatter.renderTerrain( sunDirection
							 , (const float*)m_terrain.getVertices()
							 , m_terrain.getVertexCount() * 3
							 , m_terrain.getIndices()
							 , (const float*)m_terrain.getTexCoords()							 
							 , m_terrain.getIndexCount()
							 , m_terrain.getTerrainMap()
							 , m_terrain.getNormalMap()
							 );
	}


	glFrontFace( GL_CCW );
	m_UserInterface.Render( m_Timer.GetFrameRate() ); 	

}

