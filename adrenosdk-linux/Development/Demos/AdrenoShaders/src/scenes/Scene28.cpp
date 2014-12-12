//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include "Scene28.h"


float ModelScale28 = 1.0f;


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MotionBlurShader::MotionBlurShader()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MotionBlurShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject28::SimpleObject28()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
    DiffuseTexture = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject28::Update( FRMMATRIX4X4& matView, FRMMATRIX4X4& matProj, FLOAT ElapsedTime, BOOL Paused )
{
    if( !Paused )
    {
        RotateTime += ElapsedTime * 0.5f;

        Position.x = 8.0f * FrmSin( RotateTime * 8.0f );
        Position.z = 8.0f * FrmCos( RotateTime * 8.0f );
    }

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale28, ModelScale28, ModelScale28 );
    ModelMatrix = ScaleMat;

    // stand it upright
    FRMMATRIX4X4 RotateMat1 = FrmMatrixRotate( FRM_PI * 0.5f, 1.0f, 0.0f, 0.0f );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat1 );

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );

	// save last view and projection
	LastMatModelView = MatModelView;
    LastMatModelViewProj = MatModelViewProj;

	// view and projection
	MatModelView = FrmMatrixMultiply( ModelMatrix, matView );
	MatModelViewProj = FrmMatrixMultiply( MatModelView, matProj );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject28::Destroy()
{
    if( DiffuseTexture ) DiffuseTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject28::GenRandomPosition()
{
    FLOAT32 Spread = 4.0f;

    RotateTime = FrmRand() * 10.0f;
    Position.x = FrmRand() * Spread * 2.0f - Spread;
    Position.y = FrmRand() * Spread + 1.0f;
    Position.z = FrmRand() * Spread * 2.0f - Spread;
}


//--------------------------------------------------------------------------------------
// Name: CSample28()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample28::CSample28( const CHAR* strName ) : CScene( strName )
{
    m_StretchAmount = 4.0f;

    m_nWidth = 800;
    m_nHeight = 480;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample28::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    // NOTE SAME TEXTURES AS 27
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/27_Textures.pak" ) )
        return FALSE;


    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_StretchAmount, (char *)"Stretch amount" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease stretch amount" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase stretch amount" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Torus2.mesh" ) )
        return FALSE;
    
    float Split = 1.1f;
    float yAdjust = -0.05f;
    //ModelScale28 = 1.0f;
    ModelScale28 = 0.5f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

	// Set camera position
    m_CameraPos = FRMVECTOR3( 0.0f, 5.0f, -12.5f );
    
    // Set up the objects
    for( int i = 0; i < 10; i++ )
    {
        m_Object[i].Drawable = &m_Mesh;
        m_Object[i].DiffuseTexture = resource.GetTexture( "FieldStone" );

        m_Object[i].GenRandomPosition();
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample28::InitShaders()
{
    // Create the motion blur shader
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MotionBlurShader.vs", "Demos/AdrenoShaders/Shaders/MotionBlurShader.fs",
            &m_MotionBlurShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_MotionBlurShader.m_ModelViewMatrixId     = glGetUniformLocation( m_MotionBlurShader.ShaderId, "g_MatModelView" );
        m_MotionBlurShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_MotionBlurShader.ShaderId, "g_MatModelViewProj" );
        m_MotionBlurShader.m_LastModelViewMatrixId = glGetUniformLocation( m_MotionBlurShader.ShaderId, "g_LastMatModelView" );
        m_MotionBlurShader.m_LastModelViewProjMatrixId = glGetUniformLocation( m_MotionBlurShader.ShaderId, "g_LastMatModelViewProj" );
        m_MotionBlurShader.m_LightPositionId       = glGetUniformLocation( m_MotionBlurShader.ShaderId, "g_LightPos" );
        m_MotionBlurShader.m_EyePositionId         = glGetUniformLocation( m_MotionBlurShader.ShaderId, "g_EyePos" );
        m_MotionBlurShader.m_DiffuseTextureId      = glGetUniformLocation( m_MotionBlurShader.ShaderId, "g_DiffuseTexture" );
        m_MotionBlurShader.m_StretchAmountId       = glGetUniformLocation( m_MotionBlurShader.ShaderId, "g_StretchAmount" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample28::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    FLOAT32 FOVy = FrmRadians( 85 );
    FLOAT32 zNear = 0.25f;
    FLOAT32 zFar = 40.0f;
    m_matProj = FrmMatrixPerspectiveFovRH( FOVy, fAspect, zNear, zFar );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

	// update scene
	UpdateScene( 0.0f );

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample28::Destroy()
{
    // Free shader memory
    m_MotionBlurShader.Destroy();

    // Free objects
    for( int i = 0; i < 10; i++ )
        m_Object[i].Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: UpdateScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample28::UpdateScene( FLOAT32 ElapsedTime )
{
	// update light position
    m_LightPos = FRMVECTOR3( 3.3f, 2.0f, -0.42f );

    // build camera transforms
    FRMVECTOR3 UpDir = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 LookPos = -m_CameraPos;

    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // update objects
    for( int i = 0; i < 10; i++ )
        m_Object[i].Update( m_matView, m_matProj, ElapsedTime, !m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample28::Update()
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

    // toggle rotation
    if( nPressedButtons & INPUT_KEY_8 )
    {
        m_ShouldRotate = 1 - m_ShouldRotate;
    }

    if( nButtons & INPUT_KEY_6 )
    {
        m_StretchAmount -= 4.0f * ElapsedTime;
        m_StretchAmount = max( 0.0f, m_StretchAmount );
    }

    if( nButtons & INPUT_KEY_7 )
    {
        m_StretchAmount += 4.0f * ElapsedTime;
        m_StretchAmount = min( 4.0f, m_StretchAmount );
    }

	// update the scene
    UpdateScene( ElapsedTime );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample28::DrawObject( SimpleObject28* Object )
{
    FRMMATRIX4X4    MatNormal = FrmMatrixInverse( Object->ModelMatrix );
    FRMVECTOR3      Eye = m_CameraPos;

    if ( !Object || !Object->Drawable )
        return;

    glUseProgram( m_MotionBlurShader.ShaderId );
    glUniformMatrix4fv( m_MotionBlurShader.m_ModelViewMatrixId, 1, FALSE, (FLOAT32*)&Object->MatModelView );
    glUniformMatrix4fv( m_MotionBlurShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&Object->MatModelViewProj );
    glUniformMatrix4fv( m_MotionBlurShader.m_LastModelViewMatrixId, 1, FALSE, (FLOAT32*)&Object->LastMatModelView );
    glUniformMatrix4fv( m_MotionBlurShader.m_LastModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&Object->LastMatModelViewProj );
    glUniform3fv( m_MotionBlurShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_MotionBlurShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform1f( m_MotionBlurShader.m_StretchAmountId, m_StretchAmount );
    SetTexture( m_MotionBlurShader.m_DiffuseTextureId, Object->DiffuseTexture->m_hTextureHandle, 0 );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample28::DrawScene()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClearDepthf( 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

    for( int i = 0; i < 10; i++ )
        DrawObject( &m_Object[i] );

    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

}

//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample28::Render()
{
    // Draw everything in the scene with motion blur
    DrawScene();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
