//=============================================================================
// FILE: scene.cpp
//
// Create and render a scene
//
// AUTHOR:          QUALCOMM
//
//                  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                              All Rights Reserved.
//                          QUALCOMM Proprietary/GTDR
//=============================================================================

//=============================================================================
// INCLUDES AND VARIABLE DEFINITIONS
//=============================================================================

#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <FrmUtils.h>
#include "scene.h"

#include "FrmFile.h"



//--------------------------------------------------------------------------------------
// DisintegrateShader
//--------------------------------------------------------------------------------------
DisintegrateShader::DisintegrateShader()
{
    ShaderId = 0;
}


VOID DisintegrateShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// AssembleShader
//--------------------------------------------------------------------------------------
AssembleShader::AssembleShader()
{
    ShaderId = 0;
}


VOID AssembleShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}



//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject::SimpleObject()
{
    ShouldRender = false;
    ModelScale = 0.7f;
    ModelScale = 1.0f;
    RotateTime = 0.0f;
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    Drawable = NULL;
    DiffuseTexture = NULL;
    BumpTexture = NULL;
    DiffuseColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
    // SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 5.0f );
    SpecularColor = FRMVECTOR4( 0.39f, 0.27f, 0.17f, 32.0f );

    XRotation = 0.0f;
    YRotation = 0.0f;
    ZRotation = 0.0f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject::Update( FLOAT ElapsedTime )
{
#ifdef HIDE_THIS
    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    FRMMATRIX4X4 RotateMat1 = FrmMatrixRotate( XRotation, 1.0f, 0.0f, 0.0f );
    FRMMATRIX4X4 RotateMat2 = FrmMatrixRotate( YRotation, 0.0f, 1.0f, 0.0f );
    FRMMATRIX4X4 RotateMat3 = FrmMatrixRotate( ZRotation, 0.0f, 0.0f, 1.0f );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat1 );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat3 );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat2 );

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
#endif // HIDE_THIS

    RotateTime += ElapsedTime * 0.5f;

    // DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! 
    // DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! 
    // RotateTime = 0.0f;
    // DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! 
    // DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! DEBUG! 

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    // spin it
    FRMMATRIX4X4 RotateMat1 = FrmMatrixRotate( RotateTime, 0.0f, 1.0f, 0.0f );
    // FRMMATRIX4X4 RotateMat2 = FrmMatrixRotate( -FRM_PI * 0.25f, 1.0f, 0.0f, 0.0f );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat1 );
    // ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat2 );

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
    if( DiffuseTexture ) 
        DiffuseTexture->Release();
    if( BumpTexture ) 
        BumpTexture->Release();
    if( NoiseTexture ) 
        NoiseTexture->Release();


    DiffuseTexture = NULL;
    BumpTexture = NULL;
    NoiseTexture = NULL;
}



//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
	return new CScene( "Disintegrate Demo" );
}

//-----------------------------------------------------------------------------
CScene::CScene(const CHAR* strName): CFrmApplication( strName )
	//-----------------------------------------------------------------------------
{
	m_strName = strName;

	m_fRotateTime = 0.0f;

	// Set default values
    m_nWidth = 765;
    m_nHeight = 480;

	// Need defaults for input control
	m_Input.m_nButtons        = 0;
	m_Input.m_nLastButtons    = 0;
	m_Input.m_nPressedButtons = 0;
	m_Input.m_nPointerState   = 0;

	m_pFont = NULL;

	m_FrameCount = 0;
	m_LastFPSDisplayTime = 0.0f;
	m_LastTouchTime = 0.0f;

	// Initialize shader variables
	m_hConstantShader         = 0;
	m_hConstantMVPLoc         = 0;
	m_hConstantColorLoc       = 0;

	m_DiffuseTexture = NULL;
	m_BumpTexture = NULL;
	m_NoiseTexture = NULL;
	m_DecayValue = 0.0f;
	m_AssembleValue = 0.0f;

	// Particle Shader related data
	m_NumParticles = 0;
	m_ParticleFactor = 1;
	m_hParticleShader = 0;
	m_slotModelViewProjMatrix = -1;
	m_slotColorTexture  = -1;
	m_slotNoiseTexture  = -1;
	m_slotNoiseValue = -1;

	m_vParticlePos = NULL;
	m_vParticleUV = NULL;
	m_pSpriteSize = NULL;
	m_ParticleInfo = NULL;

}

//-----------------------------------------------------------------------------
CScene::~CScene()
	//-----------------------------------------------------------------------------
{
	if(m_DiffuseTexture)
		m_DiffuseTexture->Release();
	if(m_BumpTexture)
		m_BumpTexture->Release();
	if(m_NoiseTexture)
		m_NoiseTexture->Release();


	m_DiffuseTexture = NULL;
	m_BumpTexture = NULL;
	m_NoiseTexture = NULL;

}

//--------------------------------------------------------------------------------------
// Name: SetTexture()
// Desc: Prepares a texture for a draw call
//--------------------------------------------------------------------------------------
VOID CScene::SetTexture( INT32 ShaderVar, UINT32 TextureHandle, INT32 SamplerSlot )
{
    glActiveTexture( GL_TEXTURE0 + SamplerSlot );
    glBindTexture( GL_TEXTURE_2D, TextureHandle );
    glUniform1i( ShaderVar, SamplerSlot );
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CScene::Initialize()
{	
	FrmLogMessage("CApplication::LoadFontPack()\n");
	m_pFont = new CFrmFontGLES();
	if( FALSE == m_pFont->Create( "Demos/Disintegrate/Fonts/Tuffy12.pak" ) )
	{
		FrmLogMessage("Failed to load Font Pack!\n");
		return FALSE;
	}
	
	// Load the packed resources
	CFrmPackedResourceGLES resource;
	if( FALSE == resource.LoadFromFile( "Demos/Disintegrate/Textures/Logo.pak" ) )
	{
		FrmLogMessage("Failed to load Logo Texture!\n");
		return FALSE;
	}
	// Create the logo texture
	m_pLogoTexture = resource.GetTexture( "Logo" );    

    m_bShouldRotate = TRUE;

    // Load the packed resources    
    if( FALSE == resource.LoadFromFile( "Demos/Disintegrate/Textures/Textures.pak" ) )
        return FALSE;

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( m_pFont, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5, m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddFloatVariable( &m_DecayValue, (char *)"Decay Level", (char *)"%0.2f" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Decrease Decay Level" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Increase Decay Level" );

    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0,   (char *)"Toggle Info Pane" );

    // Load each mesh
    if( FALSE == m_Mesh.Load( "Demos/Disintegrate/Meshes/boo.mesh" ) )
        return FALSE;

    FrmLogMessage("Initializing particles");
    InitParticles( );

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;




    FrmLogMessage("Setting particle decay values");
    SetParticleDecayValues("Demos/Disintegrate/Textures/noise_lum.tga");

    m_CameraPos = FRMVECTOR3( 0.0f, 0.0f, 5.0f );

    float Split = 2.0f;
    float yAdjust = -0.05f;

    // Initialize the shaders
    if( FALSE == InitShaders() )
    {
        return FALSE;
    }

    m_AmbientLight = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );

    // Set up the textures

    m_DiffuseTexture = resource.GetTexture( "Boo" );
    m_BumpTexture = resource.GetTexture( "BooNormal" );
    m_NoiseTexture = resource.GetTexture( "Noise" );


    // Set up the objects
    m_Objects[0].DiffuseTexture = resource.GetTexture( "Boo" );
    m_Objects[0].BumpTexture = resource.GetTexture( "BooNormal" );
    m_Objects[0].NoiseTexture = resource.GetTexture( "Noise" );
    m_Objects[0].Drawable = &m_Mesh;
    m_Objects[0].Position += FRMVECTOR3( -Split, yAdjust, 0.0f );
    m_Objects[0].DiffuseColor = FRMVECTOR4( 0.1f, 0.1f, 0.1f, 10.0f );
    m_Objects[0].SpecularColor = FRMVECTOR4( 0.1f, 0.1f, 0.1f, 10.0f );

    m_Objects[1].DiffuseTexture = resource.GetTexture( "Boo" );
    m_Objects[1].BumpTexture = resource.GetTexture( "BooNormal" );
    m_Objects[1].NoiseTexture = resource.GetTexture( "Noise" );
    m_Objects[1].Drawable = &m_Mesh;
    m_Objects[1].Position += FRMVECTOR3( Split, yAdjust, 0.0f );
    m_Objects[1].DiffuseColor = FRMVECTOR4( 0.2f, 0.2f, 0.2f, 20.0f );
    m_Objects[1].SpecularColor = FRMVECTOR4( 0.2f, 0.2f, 0.2f, 20.0f );


    // Setup the camera view matrix
    FRMVECTOR3 vCameraPosition = FRMVECTOR3( 0.0f, 4.0f, 10.0f );
    FRMVECTOR3 vCameraLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vCameraUp       = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matCameraView            = FrmMatrixLookAtRH( vCameraPosition, vCameraLookAt, vCameraUp );

    // Setup GL state: Enable culling
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );


    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: InitParticles()
// Desc: Initialize the particles
//--------------------------------------------------------------------------------------
void CScene::InitParticles()
{
    // Put the particles where the vertices of the mesh live
    // For now, only look at the first frame
    void *pVertData;
    void *pIndexData;
    FRM_MESH* pMesh = m_Mesh.GetMeshData( 0, &pVertData, &pIndexData );

    m_NumParticles = pMesh->m_nNumVertices;

    // Out goal here is to get to around 100K maximum particles
    m_ParticleFactor = 25000 / m_NumParticles;

    m_NumParticles *= m_ParticleFactor;

    m_vParticlePos = (FRMVECTOR4 *)malloc(m_NumParticles * sizeof(FRMVECTOR4));
    m_vParticleUV = (FRMVECTOR2 *)malloc(m_NumParticles * sizeof(FRMVECTOR2));
    m_pSpriteSize = (float *)malloc(m_NumParticles * sizeof(float));
    m_ParticleInfo = (ParticleInfo *)malloc(m_NumParticles * sizeof(ParticleInfo));
    if( m_vParticlePos == NULL || 
        m_vParticleUV == NULL ||
        m_pSpriteSize == NULL ||
        m_ParticleInfo == NULL)
    {
        FrmLogMessage("Unable to allocate memory for particles!");
        return;
    }

    FRMVECTOR4 MinPos = FRMVECTOR4(100.0f, 100.0f, 100.0f, 100.0f);
    FRMVECTOR4 MaxPos = FRMVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

    FRMVECTOR4 MinUV = FRMVECTOR4(100.0f, 100.0f, 0.0f, 0.0f);
    FRMVECTOR4 MaxUV = FRMVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);

    UINT32 ActualParticles = m_NumParticles / m_ParticleFactor;
    float *pWalker = (float *)pVertData;
    for (UINT32 uiActual = 0; uiActual < ActualParticles; uiActual++)
    {
        UINT32 uiIndx = uiActual * m_ParticleFactor;

        // Now unpack the current set of vert data into what we care about.
        FRM_VERTEX_ELEMENT* pVertexElements = pMesh->m_VertexLayout;
        UINT32 nOffset = 0;
        while( pVertexElements->nSize )
        {
            switch (pVertexElements->nUsage)
            {
            case FRM_VERTEX_POSITION:   // 0
                m_vParticlePos[uiIndx].x = pWalker[nOffset + 0];
                m_vParticlePos[uiIndx].y = pWalker[nOffset + 1];
                m_vParticlePos[uiIndx].z = pWalker[nOffset + 2];
                m_vParticlePos[uiIndx].w = 1.0f; // Not used
                break;
            case FRM_VERTEX_NORMAL:     // 3
                break;
            case FRM_VERTEX_TANGENT:    // 4
                break;
            case FRM_VERTEX_BINORMAL:   // 5
                break;
            case FRM_VERTEX_TEXCOORD0:  // 6
                m_vParticleUV[uiIndx].x = pWalker[nOffset + 0];
                m_vParticleUV[uiIndx].y = pWalker[nOffset + 1];
                break;
            }
            nOffset += (pVertexElements->nStride / sizeof(float));
            pVertexElements++;
        }

        m_pSpriteSize[uiIndx] = ApplyVariance( 3.0f, 2.0f );

        // This is not vert data. Fill in with whatever.  Should be changed during animation.
        m_ParticleInfo[uiIndx].DecayLevel = (float)uiIndx / (float)m_NumParticles;  // Placeholder
        m_ParticleInfo[uiIndx].LaunchTime = 0.0f;
        m_ParticleInfo[uiIndx].PathTime = 5.0f;
        m_ParticleInfo[uiIndx].vBasePos = m_vParticlePos[uiIndx];
        m_ParticleInfo[uiIndx].vStartPos = m_vParticlePos[uiIndx];
        m_ParticleInfo[uiIndx].vEndPos = m_vParticlePos[uiIndx];
        m_ParticleInfo[uiIndx].vVelocity.x = 0.0f; // ApplyVariance( 0.5f, 0.25f );
        m_ParticleInfo[uiIndx].vVelocity.y = 0.0f; // ApplyVariance( 0.0f, 0.5f );
        m_ParticleInfo[uiIndx].vVelocity.z = 0.0f; // ApplyVariance( 0.0f, 0.5f );

        // My walker is the vert data plus however many we have processed
        pWalker += (pMesh->m_nVertexSize /  sizeof(float));

        float PositionVariance = 0.1f;
        float UVVariance = 0.01f;
        float DecayVariance = 0.01f;

        // Now fill in the particles between the particles from verts
        for (UINT32 uiFactor = 1; uiFactor < m_ParticleFactor; uiFactor++)
        {
            m_vParticlePos[uiIndx + uiFactor].x = m_vParticlePos[uiIndx].x;
            m_vParticlePos[uiIndx + uiFactor].y = m_vParticlePos[uiIndx].y;
            m_vParticlePos[uiIndx + uiFactor].z = m_vParticlePos[uiIndx].z;
            m_vParticlePos[uiIndx + uiFactor].w = m_vParticlePos[uiIndx].w;

            // Just to get different colors
            m_vParticleUV[uiIndx + uiFactor].x = ApplyVariance( m_vParticleUV[uiIndx].x, UVVariance);
            m_vParticleUV[uiIndx + uiFactor].y = ApplyVariance( m_vParticleUV[uiIndx].y, UVVariance);

            m_pSpriteSize[uiIndx + uiFactor] = ApplyVariance( 3.0f, 2.0f );

            m_ParticleInfo[uiIndx + uiFactor].DecayLevel   = ApplyVariance( m_ParticleInfo[uiIndx].DecayLevel, DecayVariance);
            m_ParticleInfo[uiIndx + uiFactor].LaunchTime   = m_ParticleInfo[uiIndx].LaunchTime;
            m_ParticleInfo[uiIndx + uiFactor].PathTime     = m_ParticleInfo[uiIndx].PathTime;

            // Vary where this particle starts
            m_ParticleInfo[uiIndx + uiFactor].vBasePos.x   = ApplyVariance( m_vParticlePos[uiIndx].x, PositionVariance);
            m_ParticleInfo[uiIndx + uiFactor].vBasePos.y   = ApplyVariance( m_vParticlePos[uiIndx].y, PositionVariance);
            m_ParticleInfo[uiIndx + uiFactor].vBasePos.z   = ApplyVariance( m_vParticlePos[uiIndx].z, PositionVariance);
            m_ParticleInfo[uiIndx + uiFactor].vBasePos.w   = 1.0f;
            m_ParticleInfo[uiIndx + uiFactor].vStartPos    = m_ParticleInfo[uiIndx + uiFactor].vBasePos;
            m_ParticleInfo[uiIndx + uiFactor].vEndPos      = m_ParticleInfo[uiIndx].vEndPos;
            m_ParticleInfo[uiIndx + uiFactor].vVelocity.x  = m_ParticleInfo[uiIndx].vVelocity.x;
            m_ParticleInfo[uiIndx + uiFactor].vVelocity.y  = m_ParticleInfo[uiIndx].vVelocity.y;
            m_ParticleInfo[uiIndx + uiFactor].vVelocity.z  = m_ParticleInfo[uiIndx].vVelocity.z;


            // Make sure particle is way off in space until needed
            m_vParticlePos[uiIndx + uiFactor].x = 1000.0f;
        }

        // Make sure particle is way off in space until needed
        m_vParticlePos[uiIndx].x = 1000.0f;


        if(m_vParticlePos[uiIndx].x < MinPos.x) MinPos.x = m_vParticlePos[uiIndx].x;
        if(m_vParticlePos[uiIndx].y < MinPos.y) MinPos.y = m_vParticlePos[uiIndx].y;
        if(m_vParticlePos[uiIndx].z < MinPos.z) MinPos.z = m_vParticlePos[uiIndx].z;
        if(m_vParticlePos[uiIndx].w < MinPos.w) MinPos.w = m_vParticlePos[uiIndx].w;

        if(m_vParticlePos[uiIndx].x > MaxPos.x) MaxPos.x = m_vParticlePos[uiIndx].x;
        if(m_vParticlePos[uiIndx].y > MaxPos.y) MaxPos.y = m_vParticlePos[uiIndx].y;
        if(m_vParticlePos[uiIndx].z > MaxPos.z) MaxPos.z = m_vParticlePos[uiIndx].z;
        if(m_vParticlePos[uiIndx].w > MaxPos.w) MaxPos.w = m_vParticlePos[uiIndx].w;

        if(m_vParticleUV[uiIndx].x < MinUV.x) MinUV.x = m_vParticleUV[uiIndx].x;
        if(m_vParticleUV[uiIndx].y < MinUV.y) MinUV.y = m_vParticleUV[uiIndx].y;

        if(m_vParticleUV[uiIndx].x > MaxUV.x) MaxUV.x = m_vParticleUV[uiIndx].x;
        if(m_vParticleUV[uiIndx].y > MaxUV.y) MaxUV.y = m_vParticleUV[uiIndx].y;
    }

}

//--------------------------------------------------------------------------------------
// Name: SetParticleDecayValues()
// Desc: Read the decay texture and assign decay values to particles
//--------------------------------------------------------------------------------------
void CScene::SetParticleDecayValues(const char *pDecayTexture)
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
    if( FALSE == FrmFile_Open( pDecayTexture, FRM_FILE_READ, &pFile ) )
    {
        FrmLogMessage("Unable to open file: %s", pDecayTexture);
        return;
    }

    FrmFile_Read( pFile, &header, sizeof(header) );
    UINT32 nPixelSize = header.PixelDepth / 8;
    //(*pWidth)  = header.ImageWidth;
    //(*pHeight) = header.ImageHeight;
    //(*nFormat) = nPixelSize == 3 ? GL_RGB : GL_RGBA;

    BYTE* pBits = new BYTE[ nPixelSize * header.ImageWidth * header.ImageHeight ];
    UINT32 nNumPixels = header.ImageWidth * header.ImageHeight;
    FrmFile_Read( pFile, pBits, nPixelSize * header.ImageWidth * header.ImageHeight );
    FrmFile_Close( pFile );

    UINT32 MaxValue = 0;
    // For each particle, use the UV coordinates to look up the luminance value
    for (UINT32 uiIndx = 0; uiIndx < m_NumParticles; uiIndx++)
    {
        float UValue = m_vParticleUV[uiIndx].x;
        float VValue = m_vParticleUV[uiIndx].y;

        // Textures coordinates for this model are [-1, 1] since mirrored
        if(UValue < 0.0f)
            UValue += 1.0f;
        if(VValue < 0.0f)
            VValue += 1.0f;

        UINT32 uiWhichRow = (UINT32)(VValue * (float)header.ImageHeight);
        UINT32 uiWhichCol = (UINT32)(UValue * (float)header.ImageWidth);
        UINT32 uiWhichValue = uiWhichRow * header.ImageWidth + uiWhichCol;
        if(uiWhichValue >= nNumPixels)
            uiWhichValue = 0;

        if(uiWhichValue > MaxValue)
            MaxValue = uiWhichValue;

        m_ParticleInfo[uiIndx].DecayLevel = (float)pBits[uiWhichValue] / 255.0f;

    }

    delete [] pBits;
    return;
}


//--------------------------------------------------------------------------------------
// Name: ResetSimulation()
// Desc: Resets the decay values and the particles
//--------------------------------------------------------------------------------------
void CScene::ResetSimulation()
{
    for (UINT32 uiIndx = 0; uiIndx < m_NumParticles; uiIndx++)
    {
        ParticleInfo *pOneParticle = &m_ParticleInfo[uiIndx];
        pOneParticle->LaunchTime = 0.0f;
        pOneParticle->PathTime = 0.0f;
        pOneParticle->vStartPos = pOneParticle->vBasePos;

        m_vParticlePos[uiIndx].x = pOneParticle->vBasePos.x;
        m_vParticlePos[uiIndx].y = pOneParticle->vBasePos.y;
        m_vParticlePos[uiIndx].z = pOneParticle->vBasePos.z;
        m_vParticlePos[uiIndx].w = 1.0f; // Not used

        // Make sure this particle is initially culled
        m_vParticlePos[uiIndx].x = 1000.0f;
    }

    m_DecayValue = 0.0f;
    m_AssembleValue = 0.0f;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CScene::InitShaders()
{
    // Create the Distintegrate shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
            { "g_TangentIn",  FRM_VERTEX_TANGENT }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/Disintegrate/Shaders/Disintegrate.vs", "Demos/Disintegrate/Shaders/Disintegrate.fs",
            &m_DisintegrateShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_DisintegrateShader.m_ModelMatrixId         = glGetUniformLocation( m_DisintegrateShader.ShaderId,  "g_MatModel" );
        m_DisintegrateShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_DisintegrateShader.ShaderId,  "g_MatModelViewProj" );
        m_DisintegrateShader.m_NormalMatrixId        = glGetUniformLocation( m_DisintegrateShader.ShaderId,  "g_MatNormal" );
        m_DisintegrateShader.m_LightPositionId       = glGetUniformLocation( m_DisintegrateShader.ShaderId,  "g_LightPos" );
        m_DisintegrateShader.m_EyePositionId         = glGetUniformLocation( m_DisintegrateShader.ShaderId,  "g_EyePos" );
        m_DisintegrateShader.m_AmbientLightId        = glGetUniformLocation( m_DisintegrateShader.ShaderId,  "g_AmbientLight" );
        m_DisintegrateShader.m_SpecularColorId       = glGetUniformLocation( m_DisintegrateShader.ShaderId,  "g_SpecularColor" );
        m_DisintegrateShader.m_BumpTextureId         = glGetUniformLocation( m_DisintegrateShader.ShaderId,  "g_BumpTexture" );
        m_DisintegrateShader.m_DiffuseTextureId      = glGetUniformLocation( m_DisintegrateShader.ShaderId,  "g_DiffuseTexture" );
        m_DisintegrateShader.m_NoiseTextureId        = glGetUniformLocation( m_DisintegrateShader.ShaderId,  "g_NoiseTexture" );
        m_DisintegrateShader.m_DecayValueId          = glGetUniformLocation( m_DisintegrateShader.ShaderId,  "g_fDecayValue" );
    }

    // Create the Distintegrate shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
            { "g_TangentIn",  FRM_VERTEX_TANGENT }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/Disintegrate/Shaders/Assemble.vs", "Demos/Disintegrate/Shaders/Assemble.fs",
            &m_AssembleShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_AssembleShader.m_ModelMatrixId         = glGetUniformLocation( m_AssembleShader.ShaderId,  "g_MatModel" );
        m_AssembleShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_AssembleShader.ShaderId,  "g_MatModelViewProj" );
        m_AssembleShader.m_NormalMatrixId        = glGetUniformLocation( m_AssembleShader.ShaderId,  "g_MatNormal" );
        m_AssembleShader.m_LightPositionId       = glGetUniformLocation( m_AssembleShader.ShaderId,  "g_LightPos" );
        m_AssembleShader.m_EyePositionId         = glGetUniformLocation( m_AssembleShader.ShaderId,  "g_EyePos" );
        m_AssembleShader.m_AmbientLightId        = glGetUniformLocation( m_AssembleShader.ShaderId,  "g_AmbientLight" );
        m_AssembleShader.m_SpecularColorId       = glGetUniformLocation( m_AssembleShader.ShaderId,  "g_SpecularColor" );
        m_AssembleShader.m_BumpTextureId         = glGetUniformLocation( m_AssembleShader.ShaderId,  "g_BumpTexture" );
        m_AssembleShader.m_DiffuseTextureId      = glGetUniformLocation( m_AssembleShader.ShaderId,  "g_DiffuseTexture" );
        m_AssembleShader.m_NoiseTextureId        = glGetUniformLocation( m_AssembleShader.ShaderId,  "g_NoiseTexture" );
        m_AssembleShader.m_AssembleValueId       = glGetUniformLocation( m_AssembleShader.ShaderId,  "g_fAssembleValue" );
    }

    // Create the Constant shader
    {

        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "g_vVertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);
        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/Disintegrate/Shaders/Constant.vs",
            "Demos/Disintegrate/Shaders/Constant.fs",
            &m_hConstantShader,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        m_hConstantMVPLoc   = glGetUniformLocation( m_hConstantShader, "g_mModelViewProjectionMatrix" );
        m_hConstantColorLoc = glGetUniformLocation( m_hConstantShader, "g_vColor" );
    }

    // Create the point sprite shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "g_vPosition",        0 },
            { "g_vUV",              1 },
            { "g_PointSize",        2 },
            
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile(   "Demos/Disintegrate/Shaders/PointSprites.vs", 
                                                        "Demos/Disintegrate/Shaders/PointSprites.fs",
                                                        &m_hParticleShader,
                                                        pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_slotModelViewProjMatrix = glGetUniformLocation( m_hParticleShader,  "g_matModelViewProj" );
        m_slotColorTexture  = glGetUniformLocation( m_hParticleShader,  "g_TextureSampler" );
        m_slotNoiseTexture  = glGetUniformLocation( m_hParticleShader,  "g_NoiseTexture" );
        m_slotNoiseValue    = glGetUniformLocation( m_hParticleShader,  "g_fNoiseValue" );
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CScene::Resize()
{
    BuildCameraTransforms(m_nWidth, m_nHeight);

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CScene::Destroy()
{
    // Free shader memory
    m_DisintegrateShader.Destroy();
    m_AssembleShader.Destroy();

    // Delete shader programs
    if( m_hConstantShader )
        glDeleteProgram( m_hConstantShader );

    // Free objects
    for (UINT32 iIndx = 0; iIndx < NUM_OBJECTS; iIndx++)
    {
        m_Objects[iIndx].Destroy();
    }

    // Free mesh memory
    m_Mesh.Destroy();

    // Particle Shader related data
    if( m_hParticleShader )
    {
        glDeleteProgram( m_hParticleShader );
        m_hParticleShader = 0;
    }

    if(m_vParticlePos)
    {
        free(m_vParticlePos);
        m_vParticlePos = NULL;
    }

    if(m_vParticleUV)
    {
        free(m_vParticleUV);
        m_vParticleUV = NULL;
    }

    if(m_pSpriteSize)
    {
        free(m_pSpriteSize);
        m_pSpriteSize = NULL;
    }

    if(m_ParticleInfo)
    {
        free(m_ParticleInfo);
        m_ParticleInfo = NULL;
    }

}


//-----------------------------------------------------------------------------
void CScene::KeyDown(unsigned int keyCode)
	//-----------------------------------------------------------------------------
{
	//FrmLogMessage("CApplication::KeyDown(%d)\n", keyCode);

	m_Input.m_nButtons |= keyCode;	
}

//-----------------------------------------------------------------------------
void CScene::KeyUp(unsigned int keyCode)
	//-----------------------------------------------------------------------------
{
	//FrmLogMessage("CApplication::KeyUp(%d)\n", keyCode);

	m_Input.m_nButtons &= ~keyCode;
}

//-----------------------------------------------------------------------------
void CScene::TouchEvent(unsigned int xPos, unsigned int yPos)
	//-----------------------------------------------------------------------------
{
	// FrmLogMessage("CApplication::TouchEvent(%d, %d)\n", xPos, yPos);
    float FrmGetTimeResult = FrmGetTime();

	// Just for sanity, we only respond to touch events at 10Hz
	UINT32 TimeNow = (unsigned int)FrmGetTimeResult * 1000;
	if(TimeNow - m_LastTouchTime < 100)
	{
		// Ignore this touch event
		return;
	}

	// If touch is outside the screen, ignore it.
	if (xPos >= (unsigned int)m_nWidth ||
		yPos >= (unsigned int)m_nHeight)
	{
		FrmLogMessage("Ignoring touch event that is outside screen area");
		return;
	}

	// Handle the touch event
	m_LastTouchTime = FrmGetTimeResult * 1000.0f;

	FRMVECTOR2 vPointerPosition;
	vPointerPosition.x = +2.0f * (FLOAT32)xPos/(FLOAT32)(m_nWidth -1) - 1.0f;
	vPointerPosition.y = -2.0f * (FLOAT32)yPos/(FLOAT32)(m_nHeight-1) + 1.0f;

	// LogInfo("Screen Touch at (%0.3f, %0.3f)", vPointerPosition.x, vPointerPosition.y);
	m_Input.m_vPointerPosition = vPointerPosition;
	m_Input.m_nPointerState = FRM_INPUT::POINTER_DOWN | FRM_INPUT::POINTER_PRESSED;
	
	TouchEvent(vPointerPosition.x, vPointerPosition.y);
}

//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CScene::Update()
{
// 	if( (m_Input.m_nPointerState & FRM_INPUT::POINTER_PRESSED))
// 	{
// 		float ClickPos = 1.0f;
// 		ClickPos = m_Input.m_vPointerPosition.x;
// 
// 		// Reset the button/pointer state
// 		m_Input.m_nButtons = 0;
// 		m_Input.m_nPointerState = 0;
// 	}

    // Get the current time
    FLOAT32 fTime        = m_Timer.GetFrameTime();
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();
    if(fElapsedTime == 0.0f)
        return;
	// Process input
	UINT32 nButtons;
	UINT32 nPressedButtons;
	FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

   	UINT32 pntState;
	FRMVECTOR2 newMousePose;
	m_Input.GetPointerState( &pntState, &newMousePose );

    if( m_bShouldRotate )
    {
        m_fRotateTime += fElapsedTime * 0.25f;
    }

    // Toggle user interface0
    if( nPressedButtons & INPUT_KEY_0 )
    {
		m_UserInterface.AdvanceState();
	}

    // Decrease Noise Level
    if( nButtons & INPUT_KEY_4 )
    {
        if( m_DecayValue > 0.0f )
        {
            m_DecayValue -= fElapsedTime * 0.25f;
            if(m_DecayValue < 0.0f)
                m_DecayValue = 0.0f;
        }
    }




    // Increase Noise Level
    if( nButtons & INPUT_KEY_5 )
    {
        if( m_DecayValue < 1.0f )
        {
            m_DecayValue += fElapsedTime * 0.25f;
            if(m_DecayValue > 1.0f)
                m_DecayValue = 1.0f;
        }
    }

     if(pntState & FRM_INPUT::POINTER_DOWN)
     {
         if(newMousePose.x >= 0.0f)
         {
            if( m_DecayValue < 1.0f )
            {
                m_DecayValue += fElapsedTime * 0.25f;
                if(m_DecayValue > 1.0f)
                    m_DecayValue = 1.0f;
            }
         }
         else
         {
            if( m_DecayValue > 0.0f )
            {
                m_DecayValue -= fElapsedTime * 0.25f;
                if(m_DecayValue < 0.0f)
                    m_DecayValue = 0.0f;
            }
         }
     }


    // Toggle rotation
    if( nPressedButtons & INPUT_KEY_8 )
    {
        m_bShouldRotate = 1 - m_bShouldRotate;
    }

    BuildCameraTransforms(m_nWidth, m_nHeight);

    // update light position
    m_LightPos = FRMVECTOR3( 0.0f, 15.0f, 15.0f );

    // update objects
    for (UINT32 iIndx = 0; iIndx < NUM_OBJECTS; iIndx++)
    {
        m_Objects[iIndx].Update( fElapsedTime );
    }



    // Setup the mesh's camera relative model view, model view projection, and normal matrices
    FRMMATRIX4X4 matMeshRotate    = FrmMatrixIdentity(); // FrmMatrixRotate( 0.0f, FRMVECTOR3( 0.0f, 1.0f, 0.0f ) );
    FRMMATRIX4X4 matMeshRotate2   = FrmMatrixIdentity(); // FrmMatrixRotate( 0.0f, FRMVECTOR3( 1.0f, 0.0f, 0.0f ) );
    FRMMATRIX4X4 matMeshTranslate = FrmMatrixTranslate( 0.0f, 2.0f, 0.0f );

    FRMMATRIX4X4 matMeshModel     = FrmMatrixMultiply( matMeshRotate, matMeshTranslate );
    m_matMeshModel                = FrmMatrixMultiply( matMeshRotate2, matMeshModel );

    UpdateParticles(fElapsedTime);
}

//--------------------------------------------------------------------------------------
// Name: UpdateParticles()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CScene::UpdateParticles(FLOAT32 fElapsedTime)
{
    float TimeNow = m_Timer.GetFrameTime();

    for (UINT32 uiIndx = 0; uiIndx < m_NumParticles; uiIndx++)
    {
        // If this particle has not decayed (visible) yet, ignore
        if (m_ParticleInfo[uiIndx].DecayLevel >  m_DecayValue )
            continue;

        // See if this particle has been launched yet
        if(m_ParticleInfo[uiIndx].LaunchTime == 0.0f)
        {
            // Need the position to be adjusted by the model view
            FRMVECTOR4 NewPos = FrmVector4Transform( m_ParticleInfo[uiIndx].vBasePos, m_Objects[0].ModelMatrix );
            m_ParticleInfo[uiIndx].vStartPos = NewPos;
            m_ParticleInfo[uiIndx].vStartPos.w = 1.0f;
            m_vParticlePos[uiIndx] = m_ParticleInfo[uiIndx].vStartPos;

            m_ParticleInfo[uiIndx].LaunchTime = TimeNow;
            m_ParticleInfo[uiIndx].PathTime = ApplyVariance( 2.0f, 0.5f );

            m_vParticlePos[uiIndx].x = m_ParticleInfo[uiIndx].vStartPos.x;
            m_vParticlePos[uiIndx].y = m_ParticleInfo[uiIndx].vStartPos.y;
            m_vParticlePos[uiIndx].z = m_ParticleInfo[uiIndx].vStartPos.z;

            continue;
        }

        float Elapsed  = TimeNow - m_ParticleInfo[uiIndx].LaunchTime;
        if(m_ParticleInfo[uiIndx].PathTime > Elapsed)
        {
            // Still traveling
            float LerpVal = Elapsed / m_ParticleInfo[uiIndx].PathTime;
            FRMVECTOR4 EndPos = FrmVector4Transform( m_ParticleInfo[uiIndx].vBasePos, m_Objects[1].ModelMatrix );

            // Lerp between starting and ending values
            m_vParticlePos[uiIndx].x = FrmLerp(LerpVal, m_ParticleInfo[uiIndx].vStartPos.x, EndPos.x);
            m_vParticlePos[uiIndx].y = FrmLerp(LerpVal, m_ParticleInfo[uiIndx].vStartPos.y, EndPos.y);
            m_vParticlePos[uiIndx].z = FrmLerp(LerpVal, m_ParticleInfo[uiIndx].vStartPos.z, EndPos.z);
            m_vParticlePos[uiIndx].w = 1.0f;
        }
        else
        {
            // The particle has arrived.  Move it to cull-mans land
            m_vParticlePos[uiIndx].x = 1000.0f;

            if(m_ParticleInfo[uiIndx].DecayLevel > m_AssembleValue)
                m_AssembleValue = m_ParticleInfo[uiIndx].DecayLevel;
        }
    }

}

//--------------------------------------------------------------------------------------
// Name: DrawDisintegrateObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CScene::DrawDisintegrateObject( SimpleObject* Object )
{
    FRMMATRIX4X4    MatModel;
    FRMMATRIX4X4    MatModelView;
    FRMMATRIX4X4    MatNormal;

    if ( !Object || !Object->Drawable )
        return;

    MatModel         = Object->ModelMatrix;
    MatModelView     = FrmMatrixMultiply( MatModel, m_matView );
    Object->MatModelViewProj = FrmMatrixMultiply( MatModelView, m_matProj );
    // MatNormal        = FrmMatrixInverse( MatModel );

    // The normal matrix is basically the upper 3x3 of the ModelView 
    MatNormal        = MatModelView;
    MatNormal.m[0][3] = 0.0f;
    MatNormal.m[1][3] = 0.0f;
    MatNormal.m[2][3] = 0.0f;

    MatNormal.m[3][0] = 0.0f;
    MatNormal.m[3][1] = 0.0f;
    MatNormal.m[3][2] = 0.0f;
    MatNormal.m[3][3] = 1.0f;

    glUseProgram( m_DisintegrateShader.ShaderId );
    glUniformMatrix4fv( m_DisintegrateShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_DisintegrateShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&Object->MatModelViewProj );
    glUniformMatrix4fv( m_DisintegrateShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_DisintegrateShader.m_EyePositionId, 1, (FLOAT32*)&m_CameraPos.v );
    glUniform3fv( m_DisintegrateShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_DisintegrateShader.m_AmbientLightId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform4fv( m_DisintegrateShader.m_SpecularColorId, 1, Object->SpecularColor.v );
    glUniform1f( m_DisintegrateShader.m_DecayValueId, m_DecayValue );

    SetTexture( m_DisintegrateShader.m_DiffuseTextureId, Object->DiffuseTexture->m_hTextureHandle, 0 );
    SetTexture( m_DisintegrateShader.m_BumpTextureId, Object->BumpTexture->m_hTextureHandle, 1 );
    SetTexture( m_DisintegrateShader.m_NoiseTextureId, Object->NoiseTexture->m_hTextureHandle, 2 );

    Object->Drawable->Render();
}

//--------------------------------------------------------------------------------------
// Name: DrawAssembleObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CScene::DrawAssembleObject( SimpleObject* Object )
{
    FRMMATRIX4X4    MatModel;
    FRMMATRIX4X4    MatModelView;
    FRMMATRIX4X4    MatNormal;

    if ( !Object || !Object->Drawable )
        return;

    MatModel           = Object->ModelMatrix;
    MatModelView       = FrmMatrixMultiply( MatModel, m_matView );
    Object->MatModelViewProj   = FrmMatrixMultiply( MatModelView, m_matProj );
    // MatNormal          = FrmMatrixInverse( MatModel );

    // The normal matrix is basically the upper 3x3 of the ModelView 
    MatNormal        = MatModelView;
    MatNormal.m[0][3] = 0.0f;
    MatNormal.m[1][3] = 0.0f;
    MatNormal.m[2][3] = 0.0f;

    MatNormal.m[3][0] = 0.0f;
    MatNormal.m[3][1] = 0.0f;
    MatNormal.m[3][2] = 0.0f;
    MatNormal.m[3][3] = 1.0f;

    glUseProgram( m_AssembleShader.ShaderId );
    glUniformMatrix4fv( m_AssembleShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_AssembleShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&Object->MatModelViewProj );
    glUniformMatrix4fv( m_AssembleShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_AssembleShader.m_EyePositionId, 1, (FLOAT32*)&m_CameraPos.v );
    glUniform3fv( m_AssembleShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_AssembleShader.m_AmbientLightId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform4fv( m_AssembleShader.m_SpecularColorId, 1, Object->SpecularColor.v );
    glUniform1f( m_AssembleShader.m_AssembleValueId, m_AssembleValue );

    SetTexture( m_AssembleShader.m_DiffuseTextureId, Object->DiffuseTexture->m_hTextureHandle, 0 );
    SetTexture( m_AssembleShader.m_BumpTextureId, Object->BumpTexture->m_hTextureHandle, 1 );
    SetTexture( m_AssembleShader.m_NoiseTextureId, Object->NoiseTexture->m_hTextureHandle, 2 );

    Object->Drawable->Render();
}

//--------------------------------------------------------------------------------------
// Name: BuildInfoString()
// Desc: 
//--------------------------------------------------------------------------------------
void CScene::BuildInfoString(char *pBuffer, int iSize, float fFrameRate)
{
    FrmSprintf( pBuffer, iSize, "FPS: %0.2f (Particles = %d; Decay = %0.2f; Assemble = %0.2f)", fFrameRate, m_NumParticles, m_DecayValue, m_AssembleValue);
}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CScene::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glClearColor( 0.07f, 0.07f, 0.07f, 1.0f );
    glClearColor( 0.25f, 0.25f, 0.25f, 1.0f );
    glClearColor( 0.0f, 0.0f, 0.25f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    // Draw objects
    //for( UINT32 uiIndx = 0; uiIndx < NUM_OBJECTS; uiIndx++ )
    //    DrawObject( &m_Objects[uiIndx] );
    glDisable(GL_CULL_FACE);

    if(m_DecayValue < 1.0f)
        DrawDisintegrateObject( &m_Objects[0] );

    if(m_AssembleValue > 0.0f)
        DrawAssembleObject( &m_Objects[1] );

    // Draw the particles
    if(m_NumParticles && m_DecayValue > 0)
    {
        glDisable( GL_DEPTH_TEST );

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        SetTexture( m_slotColorTexture, m_DiffuseTexture->m_hTextureHandle, 0 );
        SetTexture( m_slotNoiseTexture, m_NoiseTexture->m_hTextureHandle, 1 );
        glUseProgram( m_hParticleShader );

        // Particles are not rotated!
        FRMMATRIX4X4    MatModel;
        FRMMATRIX4X4    MatModelView;
        FRMMATRIX4X4    MatModelViewProj;

        MatModel         = FrmMatrixIdentity();
        MatModelView     = FrmMatrixMultiply( MatModel, m_matView );
        MatModelViewProj = FrmMatrixMultiply( MatModelView, m_matProj );

        // glUniformMatrix4fv( m_slotModelViewProjMatrix, 1, FALSE, (FLOAT32*)&m_Objects[0].MatModelViewProj );
        glUniformMatrix4fv( m_slotModelViewProjMatrix, 1, FALSE, (FLOAT32*)&MatModelViewProj );
        glUniform1f( m_slotNoiseValue, m_DecayValue );

		FrmClearVertexAttributeArrays();

        glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, (VOID*)m_vParticlePos );
        glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (VOID*)m_vParticleUV );
        glVertexAttribPointer( 2, 1, GL_FLOAT, GL_FALSE, 0, (VOID*)m_pSpriteSize );

        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );
        glEnableVertexAttribArray( 2 );
        glDrawArrays( GL_POINTS, 0, m_NumParticles );
    }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
//#if defined(OS_WIN32)
    m_UserInterface.Render( m_Timer.GetFrameRate() );
//#endif 

    m_FrameCount++;
    FLOAT TimeNow = m_Timer.GetTime();
    FLOAT TimeDiff = TimeNow - m_LastFPSDisplayTime;
    if(TimeDiff > 1.0f)
    {
        // Display the FPS
        float fFrameRate = (float)m_FrameCount / TimeDiff;
        CHAR strDisplay[80];
        BuildInfoString(strDisplay, 80, fFrameRate);
        FrmLogMessage((char*) "%s\n", strDisplay);

        m_FrameCount = 0;
        m_LastFPSDisplayTime = TimeNow;
    }

    // Now look at how long it has been since last touched
    TimeDiff = TimeNow - m_LastTouchTime;
    if(m_pFont && TimeDiff < 2.0f)
    {
        float fFrameRate = m_Timer.GetFrameRate();

        CHAR strDisplay[80];
        BuildInfoString(strDisplay, 80, fFrameRate);
        m_pFont->SetScaleFactors( 0.8f, 0.8f );
        m_pFont->DrawText( (float)m_nWidth / 2.0f, (float)m_nHeight - 30.0f, FRMCOLOR_WHITE, strDisplay, FRM_FONT_RIGHT );
    }

}

//--------------------------------------------------------------------------------------
void CScene::BuildCameraTransforms(UINT32 nWidth, UINT32 nHeight)
//--------------------------------------------------------------------------------------
{
    FLOAT32 fAspect = (FLOAT32)nWidth / (FLOAT32)nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 1.0f, 100.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // build camera transforms
    FRMVECTOR3 LookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 UpDir     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookAt, UpDir );
    m_matProj     = FrmMatrixMultiply( m_matView, m_matProj );
}

//--------------------------------------------------------------------------------------
// Name: TouchEvent()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CScene::TouchEvent(float xPos, float yPos)
{
    m_LastTouchTime = m_Timer.GetTime();


    if(yPos > 0.0)
    {
        // Top Half of Screen
        if(xPos < 0.0f)
        {
            // Left half of Screen
            ResetSimulation();
        }
        else
        {
            // Right half of Screen
            if( m_DecayValue < 1.0f )
            {
                m_DecayValue += 0.1f;
                if(m_DecayValue > 1.0f)
                    m_DecayValue = 1.0f;
            }
        }
    }
    else
    {
        // Bottom Half of Screen
        if(xPos < 0.f)
        {
            // Left half of Screen
            ResetSimulation();
        }
        else
        {
            // Right half of Screen
            if( m_DecayValue < 1.0f )
            {
                m_DecayValue += 0.01f;
                if(m_DecayValue > 1.0f)
                    m_DecayValue = 1.0f;
            }
        }
    }
}

