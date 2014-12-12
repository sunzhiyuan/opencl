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
#include "Scene26.h"


float ModelScale26 = 1.0f;



//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
ElephantShader::ElephantShader()
{
    ShaderId = 0;
    SinTanTexture = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID ElephantShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
    if( SinTanTexture ) SinTanTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject26::SimpleObject26()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
    BumpTexture = NULL;
    DiffuseColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
    SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 5.0f );
    DiffuseTexture = NULL;
    Tiling = FRMVECTOR2( 1.0f, 1.0f );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject26::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale26, ModelScale26, ModelScale26 );
    ModelMatrix = ScaleMat;

    // spin it
    // const FLOAT32 PI = 3.141592f;
    FRMMATRIX4X4 RotateMat1 = FrmMatrixRotate( RotateTime, 0.0f, 1.0f, 0.0f );
    FRMMATRIX4X4 RotateMat2 = FrmMatrixRotate( -PI * 0.05f, 1.0f, 0.0f, 0.0f );
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
VOID SimpleObject26::Destroy()
{
    if( DiffuseTexture ) DiffuseTexture->Release();
    if( BumpTexture ) BumpTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: CSample26()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample26::CSample26( const CHAR* strName ) : CScene( strName )
{
    m_nWidth = 700;
    m_nHeight = 500;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample26::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/26_Textures.pak" ) )
        return FALSE;

    // Used for Oren-Nayar calculations
    m_ElephantShader.SinTanTexture = resource.GetTexture( "SinTan" );


    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, (char *)"Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Sphere3.mesh" ) )
        return FALSE;
    m_CameraPos = FRMVECTOR3( 0.0f, 0.0f, 50.0f );
    float Split = 1.1f;
    float yAdjust = -0.05f;
    ModelScale26 = 1.0f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    m_AmbientLight = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f );

    // Set up the objects
    m_Object[0].DiffuseTexture = resource.GetTexture( "Elephant1" );
    m_Object[0].BumpTexture = resource.GetTexture( "ElephantBump1" );
    m_Object[0].Drawable = &m_Mesh;
    m_Object[0].Position += FRMVECTOR3( -Split, yAdjust, 0.0f );
    m_Object[0].SpecularColor = FRMVECTOR4( 0.1f, 0.1f, 0.1f, 10.0f );
    m_Object[0].Tiling = FRMVECTOR2( 2.0f, 2.0f );

    m_Object[1].DiffuseTexture = resource.GetTexture( "Elephant2" );
    m_Object[1].BumpTexture = resource.GetTexture( "ElephantBump2" );
    m_Object[1].Drawable = &m_Mesh;
    m_Object[1].Position += FRMVECTOR3( Split, yAdjust, 0.0f );
    m_Object[1].SpecularColor = FRMVECTOR4( 0.2f, 0.2f, 0.2f, 20.0f );
    m_Object[1].Tiling = FRMVECTOR2( 2.0f, 2.0f );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample26::InitShaders()
{
    // Create the leather shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
            { "g_TangentIn",  FRM_VERTEX_TANGENT }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/ElephantShader.vs", "Demos/AdrenoShaders/Shaders/ElephantShader.fs",
            &m_ElephantShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_ElephantShader.m_ModelMatrixId         = glGetUniformLocation( m_ElephantShader.ShaderId, "g_MatModel" );
        m_ElephantShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_ElephantShader.ShaderId, "g_MatModelViewProj" );
        m_ElephantShader.m_NormalMatrixId        = glGetUniformLocation( m_ElephantShader.ShaderId, "g_MatNormal" );
        m_ElephantShader.m_LightPositionId       = glGetUniformLocation( m_ElephantShader.ShaderId, "g_LightPos" );
        m_ElephantShader.m_EyePositionId         = glGetUniformLocation( m_ElephantShader.ShaderId, "g_EyePos" );
        m_ElephantShader.m_DiffuseColorId        = glGetUniformLocation( m_ElephantShader.ShaderId, "g_DiffuseColor" );
        m_ElephantShader.m_SpecularColorId       = glGetUniformLocation( m_ElephantShader.ShaderId, "g_SpecularColor" );
        m_ElephantShader.m_AmbientColorId        = glGetUniformLocation( m_ElephantShader.ShaderId, "g_AmbientColor" );
        m_ElephantShader.m_DiffuseTextureId      = glGetUniformLocation( m_ElephantShader.ShaderId, "g_DiffuseTexture" );
        m_ElephantShader.m_BumpTextureId         = glGetUniformLocation( m_ElephantShader.ShaderId, "g_BumpTexture" );
        m_ElephantShader.m_SinTanTextureId       = glGetUniformLocation( m_ElephantShader.ShaderId, "g_SinTanTexture" );
        m_ElephantShader.m_TilingId              = glGetUniformLocation( m_ElephantShader.ShaderId, "g_Tiling" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample26::Resize()
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
VOID CSample26::Destroy()
{
    // Free shader memory
    m_ElephantShader.Destroy();

    // Free objects
    for( int i = 0; i < 2; i++ )
        m_Object[i].Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample26::Update()
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

    // update light position
    m_LightPos = FRMVECTOR3( 0.0f, 10.0f, 10.0f );

    // build camera transforms
    FRMVECTOR3 LookDir   = FRMVECTOR3( 0.0f, 0.0f, -1.0f );
    FRMVECTOR3 UpDir     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 Temp      = m_CameraPos + LookDir;
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, Temp, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // update objects
    for( int i = 0; i < 2; i++ )
        m_Object[i].Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos26()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos26( FLOAT32 fTime )
{
    return FRMVECTOR3( 4.5f + 1.7f * FrmCos( 0.25f*fTime ), 
        2.0f + 1.1f * FrmSin( 0.15f*fTime ), 
        0.0f - 1.9f * FrmSin( 0.25f*fTime ) );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample26::DrawObject( SimpleObject26* Object )
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

    glUseProgram( m_ElephantShader.ShaderId );
    glUniformMatrix4fv( m_ElephantShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_ElephantShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_ElephantShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_ElephantShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_ElephantShader.m_LightPositionId, 1, m_LightPos.v );
    glUniform4fv( m_ElephantShader.m_SpecularColorId, 1, Object->SpecularColor.v );
    glUniform4fv( m_ElephantShader.m_AmbientColorId, 1, m_AmbientLight.v );
    glUniform4fv( m_ElephantShader.m_DiffuseColorId, 1, Object->DiffuseColor.v );
    glUniform2fv( m_ElephantShader.m_TilingId, 1, Object->Tiling.v );

    SetTexture( m_ElephantShader.m_DiffuseTextureId, Object->DiffuseTexture->m_hTextureHandle, 0 );
    SetTexture( m_ElephantShader.m_BumpTextureId, Object->BumpTexture->m_hTextureHandle, 1 );
    SetTexture( m_ElephantShader.m_SinTanTextureId, m_ElephantShader.SinTanTexture->m_hTextureHandle, 2 );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample26::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );

    for( int i = 0; i < 2; i++ )
        DrawObject( &m_Object[i] );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
