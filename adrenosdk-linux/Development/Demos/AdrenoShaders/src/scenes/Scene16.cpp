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
#include "Scene16.h"


static float ModelScale = 1.0f;


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SkinShader::SkinShader()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SkinShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject16::SimpleObject16()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 4.0f;
    Drawable = NULL;
    DiffuseTexture = NULL;
    BumpTexture = NULL;
    DiffuseColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
    SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 5.0f );
    m_RollOff = 0.5f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject16::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    FRMMATRIX4X4 RotateMat1 = FrmMatrixRotate( FRM_PI * 0.5f,  1.0f, 0.0f, 0.0f );
    FRMMATRIX4X4 RotateMat2 = FrmMatrixRotate( RotateTime, 0.0f, 1.0f, 0.0f );
    FRMMATRIX4X4 RotateMat3 = FrmMatrixRotate( FRM_PI * 0.5f,  0.0f, 0.0f, 1.0f );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat1 );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat3 );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat2 );

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject16::Destroy()
{
    if( DiffuseTexture ) DiffuseTexture->Release();
    if( BumpTexture ) BumpTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: CSample16()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample16::CSample16( const CHAR* strName ) : CScene( strName )
{
    m_nWidth = 800;
    m_nHeight = 500;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample16::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/16_Textures.pak" ) )
        return FALSE;

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_Object[0].m_MinnaertExponent, (char *)"Minnaert Exponent" );
    m_UserInterface.AddFloatVariable( &m_Object[0].m_RollOff, (char *)"Subsurface rolloff" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, (char *)"Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Decrease subsurface rolloff" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Increase subsurface rolloff" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease minnaert exponent" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase minnaert exponent" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Torus2.mesh" ) )
        return FALSE;
    m_CameraPos = FRMVECTOR3( 0.0f, 0.0f, 55.0f );
    float Split = 1.1f;
    float yAdjust = 0.0;
    ModelScale = 0.7f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    m_AmbientLight = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );

    // Set up the objects
    m_Object[0].DiffuseTexture = resource.GetTexture( "Skin1" );
    m_Object[0].BumpTexture = resource.GetTexture( "SkinBump1" );
    m_Object[0].Drawable = &m_Mesh;
    m_Object[0].Position.x -= Split;
    m_Object[0].Position.y += yAdjust;
    m_Object[0].SpecularColor = FRMVECTOR4( 0.75f, 0.75f, 0.75f, 10.0f );
    m_Object[0].m_MinnaertExponent = 0.75f;
    m_Object[0].m_RollOff = 0.5f;

    m_Object[1].DiffuseTexture = resource.GetTexture( "Skin2" );
    m_Object[1].BumpTexture = resource.GetTexture( "SkinBump1" );
    m_Object[1].Drawable = &m_Mesh;
    m_Object[1].Position.x += Split;
    m_Object[1].Position.y += yAdjust;
    m_Object[1].SpecularColor = FRMVECTOR4( 0.4f, 0.4f, 0.4f, 8.0f );
    m_Object[1].m_MinnaertExponent = 1.0f;
    m_Object[1].m_RollOff = 0.8f;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample16::InitShaders()
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/SkinShader.vs", "Demos/AdrenoShaders/Shaders/SkinShader.fs",
            &m_SkinShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_SkinShader.m_ModelMatrixId         = glGetUniformLocation( m_SkinShader.ShaderId, "g_MatModel" );
        m_SkinShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_SkinShader.ShaderId, "g_MatModelViewProj" );
        m_SkinShader.m_NormalMatrixId        = glGetUniformLocation( m_SkinShader.ShaderId, "g_MatNormal" );
        m_SkinShader.m_LightPositionId       = glGetUniformLocation( m_SkinShader.ShaderId, "g_LightPos" );
        m_SkinShader.m_EyePositionId         = glGetUniformLocation( m_SkinShader.ShaderId, "g_EyePos" );
        m_SkinShader.m_AmbientLightId        = glGetUniformLocation( m_SkinShader.ShaderId, "g_AmbientLight" );
        m_SkinShader.m_SpecularColorId       = glGetUniformLocation( m_SkinShader.ShaderId, "g_SpecColor" );
        m_SkinShader.m_BumpTextureId         = glGetUniformLocation( m_SkinShader.ShaderId, "g_BumpTexture" );
        m_SkinShader.m_DiffuseTextureId      = glGetUniformLocation( m_SkinShader.ShaderId, "g_DiffuseTexture" );
        m_SkinShader.m_MinnaertExponentId    = glGetUniformLocation( m_SkinShader.ShaderId, "g_MinnaertExponent" );
        m_SkinShader.m_RollOffId             = glGetUniformLocation( m_SkinShader.ShaderId, "g_RollOff" );
        m_SkinShader.m_SubsurfColorId        = glGetUniformLocation( m_SkinShader.ShaderId, "g_SubsurfColor" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample16::Resize()
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
VOID CSample16::Destroy()
{
    // Free shader memory
    m_SkinShader.Destroy();

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
VOID CSample16::Update()
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

    if( nButtons & INPUT_KEY_4 )
    {
        m_Object[0].m_RollOff -= 0.01f;
        m_Object[0].m_RollOff = max( 0.0f, m_Object[0].m_RollOff );
    }

    if( nButtons & INPUT_KEY_5 )
    {
        m_Object[0].m_RollOff += 0.01f;
        m_Object[0].m_RollOff = min( 1.0f, m_Object[0].m_RollOff );
    }

    if( nButtons & INPUT_KEY_6 )
    {
        m_Object[0].m_MinnaertExponent -= 0.01f;
        m_Object[0].m_MinnaertExponent = max( 0.01f, m_Object[0].m_MinnaertExponent );
    }

    if( nButtons & INPUT_KEY_7 )
    {
        m_Object[0].m_MinnaertExponent += 0.01f;
        m_Object[0].m_MinnaertExponent = min( 1.0f, m_Object[0].m_MinnaertExponent );
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
// Name: GetCameraPos16()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos16( FLOAT32 fTime )
{
    return FRMVECTOR3( 4.5f + 1.7f * FrmCos( 0.25f*fTime ), 
        2.0f + 1.1f * FrmSin( 0.15f*fTime ), 
        0.0f - 1.9f * FrmSin( 0.25f*fTime ) );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample16::DrawObject( SimpleObject16* Object )
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

    glUseProgram( m_SkinShader.ShaderId );
    glUniformMatrix4fv( m_SkinShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_SkinShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_SkinShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_SkinShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_SkinShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_SkinShader.m_AmbientLightId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform4fv( m_SkinShader.m_SpecularColorId, 1, Object->SpecularColor.v );
    glUniform1f( m_SkinShader.m_MinnaertExponentId, Object->m_MinnaertExponent );
    glUniform1f( m_SkinShader.m_RollOffId, Object->m_RollOff );

    static FRMVECTOR4 SubsurfColor = FRMVECTOR4( 143 / 255.0f, 65 / 255.0f, 47 / 255.0f, 1.0f );
    glUniform4fv( m_SkinShader.m_SubsurfColorId, 1, SubsurfColor.v );

    SetTexture( m_SkinShader.m_DiffuseTextureId, Object->DiffuseTexture->m_hTextureHandle, 0 );
    SetTexture( m_SkinShader.m_BumpTextureId, Object->BumpTexture->m_hTextureHandle, 1 );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample16::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );

    // Draw some objects
    for( int i = 0; i < 2; i++ )
        DrawObject( &m_Object[i] );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
