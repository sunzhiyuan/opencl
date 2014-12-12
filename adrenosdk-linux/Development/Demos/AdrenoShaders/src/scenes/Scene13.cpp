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
#include "Scene13.h"


static float ModelScale = 1.0f;


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
StraussShader::StraussShader()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID StraussShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject13::SimpleObject13()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 2.0f;
    Drawable = NULL;
    DiffuseTexture = NULL;
    DiffuseColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
    Metalness = 1.0f;
    Smoothness = 1.0f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject13::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    // rotate the object
    FRMMATRIX4X4 RotateMat1 = FrmMatrixRotate( RotateTime, 0.0f, 1.0f, 0.0f );
    FRMMATRIX4X4 RotateMat2 = FrmMatrixRotate( 0.2f,  1.0f, 0.0f, 0.0f );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat1 );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat2 );

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject13::Destroy()
{
    if( DiffuseTexture ) DiffuseTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: CSample13()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample13::CSample13( const CHAR* strName ) : CScene( strName )
{
    m_nWidth = 800;
    m_nHeight = 400;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample13::InitShaders()
{
    // Create the Strauss shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/StraussShader.vs", "Demos/AdrenoShaders/Shaders/StraussShader.fs",
            &m_StraussShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_StraussShader.m_ModelMatrixId         = glGetUniformLocation( m_StraussShader.ShaderId,  "g_MatModel" );
        m_StraussShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_StraussShader.ShaderId,  "g_MatModelViewProj" );
        m_StraussShader.m_NormalMatrixId        = glGetUniformLocation( m_StraussShader.ShaderId,  "g_MatNormal" );
        m_StraussShader.m_LightDirId            = glGetUniformLocation( m_StraussShader.ShaderId,  "g_LightDir" );
        m_StraussShader.m_CameraPosId           = glGetUniformLocation( m_StraussShader.ShaderId,  "g_CameraPos" );
        m_StraussShader.m_DiffuseColorId        = glGetUniformLocation( m_StraussShader.ShaderId,  "g_DiffuseColor" );
        m_StraussShader.m_DiffuseTextureId      = glGetUniformLocation( m_StraussShader.ShaderId,  "g_DiffuseTexture" );
        m_StraussShader.m_SmoothnessId          = glGetUniformLocation( m_StraussShader.ShaderId,  "g_Smoothness" );
        m_StraussShader.m_MetalnessId           = glGetUniformLocation( m_StraussShader.ShaderId,  "g_Metalness" );
        m_StraussShader.m_TransparencyId        = glGetUniformLocation( m_StraussShader.ShaderId,  "g_Transparency" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample13::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/13_Textures.pak" ) )
        return FALSE;

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
        m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Animate light" );
    m_UserInterface.AddFloatVariable( &m_Object[1].Metalness, (char *)"Metalness" );
    m_UserInterface.AddFloatVariable( &m_Object[1].Smoothness, (char *)"Smoothness" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Decrease metalness" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Increase metalness" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease smoothness" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase smoothness" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, (char *)"Toggle Info Pane" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Sphere3.mesh" ) )
        return FALSE;
    m_CameraPos = FRMVECTOR3( 0.0f, 0.20f, 80.5f );
    float Split = 2.4f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
    {
        return FALSE;
    }

    m_AmbientLight = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );

    // Set up the objects
    m_Object[0].DiffuseTexture = resource.GetTexture( "Strauss" );
    m_Object[0].Drawable = &m_Mesh;
    m_Object[0].Position.x -= Split;
    m_Object[0].Metalness = 0.0f;
    m_Object[0].Smoothness = 0.0f;

    m_Object[1].DiffuseTexture = resource.GetTexture( "Strauss" );
    m_Object[1].Drawable = &m_Mesh;
    m_Object[1].Metalness = 0.95f;
    m_Object[1].Smoothness = 0.55f;

    m_Object[2].DiffuseTexture = resource.GetTexture( "Strauss" );
    m_Object[2].Drawable = &m_Mesh;
    m_Object[2].Position.x += Split;
    m_Object[2].Metalness = 0.6f;
    m_Object[2].Smoothness = 0.95f;

    glEnable( GL_DEPTH_TEST );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample13::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    FLOAT32 FOVy = FrmRadians( 5 );
    m_matProj = FrmMatrixPerspectiveFovRH( FOVy, fAspect, 0.1f, 100.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample13::Destroy()
{
    // Free shader memory
    m_StraussShader.Destroy();

    // Free texture memory

    // Free objects
    for( int i = 0; i < 3; i++ )
        m_Object[i].Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample13::Update()
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

    // adjust smoothness
    if( nPressedButtons & INPUT_KEY_4 )
        m_Object[1].Metalness -= 0.2f;
    if( nPressedButtons & INPUT_KEY_5 )
        m_Object[1].Metalness += 0.2f;
    if( nPressedButtons & INPUT_KEY_6 )
        m_Object[1].Smoothness -= 0.2f;
    if( nPressedButtons & INPUT_KEY_7 )
        m_Object[1].Smoothness += 0.2f;
    m_Object[1].Metalness = min( 0.95f, max( 0.15f, m_Object[1].Metalness ) );
    m_Object[1].Smoothness = min( 0.95f, max( 0.15f, m_Object[1].Smoothness ) );


    // update light position
    static FLOAT32 LightOrbitalTime = -3.0f;
    static FRMVECTOR3 LightOrigin = FRMVECTOR3( 0.0f, 10.0f, 10.0f );
    if( m_ShouldRotate )
    {
        LightOrbitalTime += ( ElapsedTime * 0.6f );
    }
    m_LightPos = LightOrigin;
    m_LightPos.x += ( cos( LightOrbitalTime ) * 15.0f );

    // build camera transforms
    FRMVECTOR3 LookDir   = FRMVECTOR3( 0.0f, 0.0f, -1.0f );
    FRMVECTOR3 UpDir     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 Temp      = m_CameraPos + LookDir;
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, Temp, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );


    // update objects
    for( int i = 0; i < 3; i++ )
        m_Object[i].Update( ElapsedTime, FALSE );
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos13()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos13( FLOAT32 fTime )
{
    return FRMVECTOR3( 4.5f + 1.7f * FrmCos( 0.25f*fTime ), 
        2.0f + 1.1f * FrmSin( 0.15f*fTime ), 
        0.0f - 1.9f * FrmSin( 0.25f*fTime ) );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample13::DrawObject( SimpleObject13* Object )
{
    FRMMATRIX4X4    MatModel;
    FRMMATRIX4X4    MatModelView;
    FRMMATRIX4X4    MatModelViewProj;
    FRMMATRIX3X3    MatNormal;
    FRMVECTOR3      LightDir = -m_CameraPos;

    if ( !Object || !Object->Drawable )
        return;

    MatModel         = Object->ModelMatrix;
    MatModelView     = FrmMatrixMultiply( MatModel, m_matView );
    MatModelViewProj = FrmMatrixMultiply( MatModelView, m_matProj );
    MatNormal        = FrmMatrixNormal( MatModelView );

    glUseProgram( m_StraussShader.ShaderId );
    glUniformMatrix4fv( m_StraussShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_StraussShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix3fv( m_StraussShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_StraussShader.m_CameraPosId, 1, (FLOAT32*)&LightDir.x );
    glUniform3fv( m_StraussShader.m_LightDirId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_StraussShader.m_DiffuseColorId, 1, Object->DiffuseColor.v );
    SetTexture( m_StraussShader.m_DiffuseTextureId, Object->DiffuseTexture->m_hTextureHandle, 0 );
    glUniform1f( m_StraussShader.m_SmoothnessId, Object->Smoothness );
    glUniform1f( m_StraussShader.m_MetalnessId, Object->Metalness );
    glUniform1f( m_StraussShader.m_TransparencyId, 0.0f );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample13::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Draw some objects
    for( int i = 0; i < 3; i++ )
        DrawObject( &m_Object[i] );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
