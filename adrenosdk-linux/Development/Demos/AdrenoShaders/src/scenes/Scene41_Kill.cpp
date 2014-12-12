//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "Scene41.h"

//--------------------------------------------------------------------------------------
// Name: OffscreenRT41()
// Desc: Class constructor.  Clears render target data
//--------------------------------------------------------------------------------------
OffscreenRT41::OffscreenRT41()
{
    BufferHandle = NULL;
    TextureHandle = NULL;
    DepthTextureHandle = NULL;
    Width = 0;
    Height = 0;
}


//--------------------------------------------------------------------------------------
// Name: ~OffscreenRT41()
// Desc: Class destructor.
//--------------------------------------------------------------------------------------
OffscreenRT41::~OffscreenRT41()
{
    FreeRT();
}


//--------------------------------------------------------------------------------------
// Name: CreateRT
// Desc: Creates a render target for drawing to.
//--------------------------------------------------------------------------------------
BOOL OffscreenRT41::CreateRT( UINT32 DesiredWidth, UINT32 DesiredHeight, BOOL CreateDepthBuffer )
{
    // Free the old RT, in case it was previously created
    FreeRT();

    Width = DesiredWidth;
    Height = DesiredHeight;

    // Make a depth buffer texture
    if( CreateDepthBuffer )
    {
        glGenTextures( 1, &DepthTextureHandle );
        glBindTexture( GL_TEXTURE_2D, DepthTextureHandle );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Width, Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
    }

    // Make a color buffer texture
    {
        glGenTextures( 1, &TextureHandle );
        glBindTexture( GL_TEXTURE_2D, TextureHandle );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    } 

    // Create a frame buffer
    glGenFramebuffers( 1, &BufferHandle );
    glBindFramebuffer( GL_FRAMEBUFFER, BufferHandle );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureHandle, 0 );
    if( CreateDepthBuffer )
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTextureHandle, 0 );


    // Make sure everything went ok
    switch( glCheckFramebufferStatus( GL_FRAMEBUFFER) )
    {
        case GL_FRAMEBUFFER_UNSUPPORTED:
            return FALSE;
    }

    // Reset default frame buffer
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: FreeOffscreenRT41
// Desc: 
//--------------------------------------------------------------------------------------
VOID OffscreenRT41::FreeRT()
{
    if( BufferHandle )
    {
        glBindFramebuffer( GL_FRAMEBUFFER, BufferHandle );
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    }

    if( BufferHandle )
    {
        glDeleteFramebuffers( 1, &BufferHandle );
        BufferHandle = NULL;
    }
    if( TextureHandle )
    {
        glDeleteTextures( 1, &TextureHandle );
        TextureHandle = NULL;
    }
    if( DepthTextureHandle )
    {
        glDeleteTextures( 1, &DepthTextureHandle );
        DepthTextureHandle = NULL;
    }
}


//--------------------------------------------------------------------------------------
// Name: SetFramebuffer
// Desc: Sets the render target as the active frame buffer.
//--------------------------------------------------------------------------------------
VOID OffscreenRT41::SetFramebuffer()
{
    glBindFramebuffer( GL_FRAMEBUFFER, BufferHandle );

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureHandle, 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTextureHandle, 0 );
    glViewport( 0, 0, Width, Height );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader41::MaterialShader41()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader41::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
DownsizeShader41::DownsizeShader41()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID DownsizeShader41::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CombineShader41::CombineShader41()
{
    ShaderId = 0;

    GaussSpread = 3.0f;
    GaussRho = 1.0f;
	BloomIntensity = 4.0f;
	Saturation = 1.0f;
    Contrast = 0.0f;
    ColorCorrect = FRMVECTOR3(1.0f, 1.0f, 1.0f);
    ColorAdd = FRMVECTOR3(0.0f, 0.0f, 0.0f);


}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CombineShader41::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BlurShader41::BlurShader41()
{
    ShaderId = 0;
    StepSize = FRMVECTOR2( 1.0f, 1.0f );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BlurShader41::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject41::SimpleObject41()
{
    ModelScale = 1.0f;
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
    BumpTexture = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject41::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
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
VOID SimpleObject41::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample41()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample41::CSample41( const CHAR* strName ) : CScene( strName )
{
	m_p3DRandomTexture = NULL;

    m_nWidth = 800;
    m_nHeight = 600;
}

//--------------------------------------------------------------------------------------
// Name: CreateRandomTexture3D()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample41::CreateRandomTexture3D( UINT32 nDim )
{
    UINT32 nNumTexels = nDim * nDim * nDim;
    UINT8* pTexels    = new UINT8[ nNumTexels ];
    UINT32 nIndex     = 0;

    // Create a 3D texture of random values
    for( UINT32 nDepth = 0; nDepth < nDim; ++nDepth )
    {
        UINT32* pFilePixels = new UINT32[ nDim * nDim ];
        BYTE*   pPixelWriteData = (BYTE*)pFilePixels;
    
        for( UINT32 nCol = 0; nCol < nDim; ++nCol )
        {
            for( UINT32 nRow = 0; nRow < nDim; ++nRow )
            {
                UINT8 nRandNum = (UINT8)( 256.0f * FrmRand() );
                pTexels[ nIndex++ ] = nRandNum;

                *pPixelWriteData++ = nRandNum;
                *pPixelWriteData++ = 0;
                *pPixelWriteData++ = 0;
                *pPixelWriteData++ = 255;
            }
        }

        CHAR strFileName[80];
        FrmSprintf( strFileName, 80, "Random3D_%03d.tga", nDepth );
        FrmSaveImageAsTGA( strFileName, nDim, nDim, pFilePixels );
        delete[] pFilePixels; 
    }

    BOOL bResult = FrmCreateTexture3D( nDim, nDim, nDim, 1, GL_LUMINANCE,
                                       GL_UNSIGNED_BYTE, 0, pTexels, nNumTexels * sizeof(UINT8),
                                       &m_p3DRandomTexture );
    glTexParameteri( GL_TEXTURE_3D_OES, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D_OES, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    delete[] pTexels;
    return bResult;
}

//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample41::CheckFrameBufferStatus()
{
    switch( glCheckFramebufferStatus(GL_FRAMEBUFFER) )
    {
        case GL_FRAMEBUFFER_COMPLETE:
            return TRUE;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            return FALSE;
    }
    FrmAssert(0);
    return FALSE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample41::Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Create the font
    // Load the packed resources
    // Note same texture pack as 40!
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Media/Textures/40_Textures.pak" ) )
        return FALSE;


#ifdef CREATE_TEXTURES
    CreateRandomTexture3D( 64 );
#else
    m_p3DRandomTexture = resource.GetTexture3D( "Random3D" );
#endif

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.GaussSpread, (char *)"Gauss filter spread" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.BloomIntensity, (char *)"Bloom Intensity" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.Saturation, (char *)"Saturation press 2 or 3" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.ColorCorrect.r, (char *)"Color Mul. Red press 9 or 1" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.ColorCorrect.g, (char *)"Color Mul. Green" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.ColorCorrect.b, (char *)"Color Mul. Blue" );
 	m_UserInterface.AddFloatVariable( &m_CombineShader.ColorAdd.r, (char *)"Color addition Red press a or b" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.ColorAdd.g, (char *)"Color addition Green" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.ColorAdd.b, (char *)"Color addition Blue" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.Contrast, (char *)"Contrast" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_9, (char *)"Decrease Color Multiplication Red" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Increase Color Multiplication Red" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_DOWN, (char *)"Decrease Color Add Red" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_UP, (char *)"Increase Color Add Red" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_LEFT, (char *)"Decrease Contrast" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_RIGHT, (char *)"Increase Contrast" );
	m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Decrease saturation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Increase saturation" );
 	m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Decrease bloom intensity" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Increase bloom intensity" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
 	m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease gauss spread" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase gauss spread" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Media/Meshes/Map3.mesh" ) )
        return FALSE;
    m_CameraPos = FRMVECTOR3( 6.2f, 2.0f, 0.0f );
    float Split = 1.1f;
    float yAdjust = -0.05f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    m_AmbientLight = FRMVECTOR4( 0.1f, 0.1f, 0.1f, 0.0f );

    // Set up the objects
    m_Object.Drawable = &m_Mesh;
    m_Object.Position.y += yAdjust;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample41::InitShaders()
{
    // Create the material shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
            { "g_TangentIn",  FRM_VERTEX_TANGENT },
            { "g_BinormalIn", FRM_VERTEX_BINORMAL }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Media/Shaders/MaterialShader41.vs", "Media/Shaders/MaterialShader41.fs",
            &m_MaterialShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_MaterialShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_MaterialShader.ShaderId, "g_MatModelViewProj" );
        m_MaterialShader.m_ModelMatrixId         = glGetUniformLocation( m_MaterialShader.ShaderId, "g_MatModel" );
        m_MaterialShader.m_LightPositionId       = glGetUniformLocation( m_MaterialShader.ShaderId, "g_LightPos" );
        m_MaterialShader.m_EyePositionId         = glGetUniformLocation( m_MaterialShader.ShaderId, "g_EyePos" );
        m_MaterialShader.m_AmbientColorId        = glGetUniformLocation( m_MaterialShader.ShaderId, "g_AmbientColor" );
        m_MaterialShader.m_DiffuseTextureId      = glGetUniformLocation( m_MaterialShader.ShaderId, "g_DiffuseTexture" );
        m_MaterialShader.m_BumpTextureId         = glGetUniformLocation( m_MaterialShader.ShaderId, "g_BumpTexture" );
    }

    // downsize shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Media/Shaders/DownsizeShader41.vs", "Media/Shaders/DownsizeShader41.fs",
            &m_DownsizeShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_DownsizeShader.ColorTextureId = glGetUniformLocation( m_DownsizeShader.ShaderId, "g_ColorTexture" );
    }

    // blur shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Media/Shaders/BlurShader41.vs", "Media/Shaders/BlurShader41.fs",
            &m_BlurShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_BlurShader.ColorTextureId = glGetUniformLocation( m_BlurShader.ShaderId, "g_ColorTexture" );
        m_BlurShader.GaussWeightId = glGetUniformLocation( m_BlurShader.ShaderId, "g_GaussWeight" );
        m_BlurShader.GaussInvSumId = glGetUniformLocation( m_BlurShader.ShaderId, "g_GaussInvSum" );
        m_BlurShader.StepSizeId = glGetUniformLocation( m_BlurShader.ShaderId, "g_StepSize" );

        SetupGaussWeights();
    }

    // combine shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Media/Shaders/CombineShader41.vs", "Media/Shaders/CombineShader41.fs",
            &m_CombineShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CombineShader.SharpTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_SharpTexture" );
        m_CombineShader.BlurredTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_BlurredTexture" );
        m_CombineShader.BloomIntensityId = glGetUniformLocation( m_CombineShader.ShaderId, "g_BloomIntensity" );
        m_CombineShader.ContrastId = glGetUniformLocation( m_CombineShader.ShaderId, "g_Contrast" );
        m_CombineShader.SaturationId = glGetUniformLocation( m_CombineShader.ShaderId, "g_Saturation" );
        m_CombineShader.ColorCorrectId = glGetUniformLocation( m_CombineShader.ShaderId, "g_ColorCorrect" );
        m_CombineShader.ColorAddId = glGetUniformLocation( m_CombineShader.ShaderId, "g_ColorAdd" );
        m_CombineShader.Rand3DTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_Rand3DTexture" );
        m_CombineShader.StaticFrequencySlotId = glGetUniformLocation( m_CombineShader.ShaderId, "g_fFrequency" );
        m_CombineShader.StaticTimeSlotId = glGetUniformLocation( m_CombineShader.ShaderId, "g_fTime" );
	}

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample41::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    FLOAT32 FOVy = FrmRadians( 86 );
    FLOAT32 zNear = 0.25f;
    FLOAT32 zFar = 20.0f;
    m_matProj = FrmMatrixPerspectiveFovRH( FOVy, fAspect, zNear, zFar );


    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    m_SharpRT.CreateRT( m_nWidth, m_nHeight, TRUE );
    m_BlurredRT.CreateRT( m_nWidth / 2, m_nHeight / 2, FALSE );
    m_ScratchRT.CreateRT( m_nWidth / 2, m_nHeight / 2, FALSE );

    m_BlurShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_nWidth, 1.0f / (FLOAT32)m_nHeight );
    m_DownsizeShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_nWidth, 1.0f / (FLOAT32)m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample41::Destroy()
{
    // Free shader memory
    m_MaterialShader.Destroy();
    m_CombineShader.Destroy();
    m_DownsizeShader.Destroy();
    m_BlurShader.Destroy();

    // Free texture memory
    if( m_p3DRandomTexture ) m_p3DRandomTexture->Release();

    // Free objects
    m_Object.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos41()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos41( FLOAT32 fTime )
{
    FRMVECTOR3 BaseOffset = FRMVECTOR3( 4.5f, 2.0f, 0.0f );
    FRMVECTOR3 CurPosition = FRMVECTOR3( 1.7f * FrmCos( 0.25f * fTime ), 0.5f * FrmSin( 0.15f * fTime ), -1.9f * FrmSin( 0.25f * fTime ) );
    return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample41::Update()
{
    // get the current time
    float ElapsedTime = m_Timer.GetFrameElapsedTime();

    m_fElapsedTime = m_Timer.GetTime();

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

    if( nButtons & INPUT_DPAD_LEFT )
    {
        m_CombineShader.Contrast -= 0.5f * ElapsedTime;
        m_CombineShader.Contrast = max( 0.0f, m_CombineShader.Contrast );
    }

    if( nButtons & INPUT_DPAD_RIGHT )
    {
        m_CombineShader.Contrast += 0.5f * ElapsedTime;
        m_CombineShader.Contrast = min(5.0f, m_CombineShader.Contrast );
    }


    if( nButtons & INPUT_DPAD_DOWN )
    {
        m_CombineShader.ColorAdd.r -= 0.5f * ElapsedTime;
        m_CombineShader.ColorAdd.r = max( 0.0f, m_CombineShader.ColorAdd.r );
    }

    if( nButtons & INPUT_DPAD_UP )
    {
        m_CombineShader.ColorAdd.r += 0.5f * ElapsedTime;
        m_CombineShader.ColorAdd.r = min(1.0f, m_CombineShader.ColorAdd.r );
    }

    if( nButtons & INPUT_KEY_9 )
    {
        m_CombineShader.ColorCorrect.r -= 0.5f * ElapsedTime;
        m_CombineShader.ColorCorrect.r = max( 0.0f, m_CombineShader.ColorCorrect.r );
    }

    if( nButtons & INPUT_KEY_1 )
    {
        m_CombineShader.ColorCorrect.r += 0.5f * ElapsedTime;
        m_CombineShader.ColorCorrect.r = min(1.0f, m_CombineShader.ColorCorrect.r );
    }

    if( nButtons & INPUT_KEY_2 )
    {
        m_CombineShader.Saturation -= 0.5f * ElapsedTime;
        m_CombineShader.Saturation = max( 0.0f, m_CombineShader.Saturation );
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_CombineShader.Saturation += 0.5f * ElapsedTime;
        m_CombineShader.Saturation = min(1.0f, m_CombineShader.Saturation );
    }


    if( nButtons & INPUT_KEY_4 )
    {
        m_CombineShader.BloomIntensity -= 1.0f * ElapsedTime;
        m_CombineShader.BloomIntensity = max( 0.0f, m_CombineShader.BloomIntensity );
    }

    if( nButtons & INPUT_KEY_5 )
    {
        m_CombineShader.BloomIntensity += 1.0f * ElapsedTime;
        m_CombineShader.BloomIntensity = min(16.0f, m_CombineShader.BloomIntensity );
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
    FRMVECTOR3 LookPos = GetCameraPos41( TotalTime + 0.05f );
    m_CameraPos   = GetCameraPos41( TotalTime );
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // update objects
    m_Object.Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: GaussianDistribution41()
// Desc: 
//--------------------------------------------------------------------------------------
FLOAT32 GaussianDistribution41( FLOAT32 X, FLOAT32 Y, FLOAT32 Rho )
{
    FLOAT32 Gauss = 1.0f / sqrtf( 2.0f * FRM_PI * Rho * Rho );
    Gauss *= expf( -( X * X + Y * Y ) / ( 2.0f * Rho * Rho ) );

    return Gauss;
}


//--------------------------------------------------------------------------------------
// Name: SetupGaussWeights()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample41::SetupGaussWeights()
{
    FLOAT32 GaussSum = 0.0f;
    for( int i = 0; i < 4; i++ )
    {
        // normalize to -1..1
        FLOAT32 X = ( FLOAT32( i ) / 3.0f );
        X = ( X - 0.5f ) * 2.0f;

        // spread is tunable
        X *= m_CombineShader.GaussSpread;

        m_BlurShader.GaussWeight[i] = GaussianDistribution41( X, 0.0f, m_CombineShader.GaussRho );

        GaussSum += m_BlurShader.GaussWeight[i];
    }

    m_BlurShader.GaussInvSum = ( 1.0f / GaussSum );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample41::DrawObject( SimpleObject41* Object )
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

    glUseProgram( m_MaterialShader.ShaderId );
    glUniformMatrix4fv( m_MaterialShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_MaterialShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniform3fv( m_MaterialShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_MaterialShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_MaterialShader.m_AmbientColorId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform1i( m_MaterialShader.m_DiffuseTextureId, 0 );
    glUniform1i( m_MaterialShader.m_BumpTextureId, 1 );


    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: RenderScreenAlignedQuad41
// Desc: Draws a full-screen rectangle
//--------------------------------------------------------------------------------------
VOID CSample41::RenderScreenAlignedQuad41()
{
    glDisable( GL_DEPTH_TEST );

    const FLOAT32 Quad[] =
    {
        +1.0, -1.0f, 1.0f, 1.0f,
        -1.0, -1.0f, 0.0f, 1.0f,
        +1.0, +1.0f, 1.0f, 0.0f,
        -1.0, +1.0f, 0.0f, 0.0f,
    };

    glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, Quad );
    glEnableVertexAttribArray( 0 );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glDisableVertexAttribArray( 0 );
    glEnable( GL_DEPTH_TEST );
}


//--------------------------------------------------------------------------------------
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample41::DrawScene()
{
    m_SharpRT.SetFramebuffer();

    glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
    glClearDepthf( 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glFrontFace( GL_CW );

    DrawObject( &m_Object );
}


//--------------------------------------------------------------------------------------
// Name: GaussBlur()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample41::GaussBlur( OffscreenRT41& TargetRT )
{
    // Blur in the X direction
    {
        m_ScratchRT.SetFramebuffer();

        glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR2 StepSizeX = FRMVECTOR2( m_BlurShader.StepSize.x * m_CombineShader.GaussSpread, 0.0f );
        glUseProgram( m_BlurShader.ShaderId );
        glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
        glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
        glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeX.v );
        SetTexture( m_BlurShader.ColorTextureId, TargetRT.TextureHandle, 0 );

        RenderScreenAlignedQuad41();
    }

    // Then blur in the Y direction
    {
        TargetRT.SetFramebuffer();

        glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR2 StepSizeY = FRMVECTOR2( 0.0f, m_BlurShader.StepSize.y * m_CombineShader.GaussSpread );
        glUseProgram( m_BlurShader.ShaderId );
        glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
        glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
        glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeY.v );
        SetTexture( m_BlurShader.ColorTextureId, m_ScratchRT.TextureHandle, 0 );

        RenderScreenAlignedQuad41();
    }
}


//--------------------------------------------------------------------------------------
// Name: CreateBlurredScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample41::CreateBlurredImage()
{
    // Part 1: downsize the framebuffer to a smaller render target
    {
        m_BlurredRT.SetFramebuffer();

        // Render a full-screen quad
        glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_DownsizeShader.ShaderId );
        SetTexture( m_DownsizeShader.ColorTextureId, m_SharpRT.TextureHandle, 0 );

        RenderScreenAlignedQuad41();
    }

    // Part 2: blur
    GaussBlur( m_BlurredRT );
}


//--------------------------------------------------------------------------------------
// Name: DrawCombinedScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample41::DrawCombinedScene()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDisable( GL_CULL_FACE );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );

    glUseProgram( m_CombineShader.ShaderId );
    SetTexture( m_CombineShader.SharpTextureId, m_SharpRT.TextureHandle, 0 );
    SetTexture( m_CombineShader.BlurredTextureId, m_BlurredRT.TextureHandle, 1 );
    glUniform1f( m_CombineShader.BloomIntensityId, m_CombineShader.BloomIntensity );
    glUniform1f( m_CombineShader.ContrastId, m_CombineShader.Contrast );
    glUniform1f( m_CombineShader.SaturationId, m_CombineShader.Saturation );
    glUniform3f( m_CombineShader.ColorCorrectId, m_CombineShader.ColorCorrect.x, m_CombineShader.ColorCorrect.y, m_CombineShader.ColorCorrect.z );
    glUniform3f( m_CombineShader.ColorAddId, m_CombineShader.ColorAdd.x, m_CombineShader.ColorAdd.y, m_CombineShader.ColorAdd.z );

    glUniform1f( m_CombineShader.StaticFrequencySlotId, 8.0f );
    glUniform1f( m_CombineShader.StaticTimeSlotId, m_fElapsedTime );

	// VOID SetTexture3D( INT32 ShaderVar, UINT32 TextureHandle, INT32 SamplerSlot )
  // SetTexture3D( m_CombineShader.Rand3DTextureId, m_p3DRandomTexture->m_hTextureHandle, 0 );
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_3D_OES, m_p3DRandomTexture->m_hTextureHandle );
    glUniform1i( m_CombineShader.Rand3DTextureId, 2 );

	RenderScreenAlignedQuad41();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample41::Render()
{
    // Draw regular stuff to the framebuffer
    DrawScene();

    // Downsize framebuffer and blur
    CreateBlurredImage();

    // Recombine scene with Bloom
    DrawCombinedScene();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
