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
#include "Scene32.h"



//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader32::MaterialShader32()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader32::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
GlassShader::GlassShader()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID GlassShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BackdropShader::BackdropShader()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BackdropShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject32::SimpleObject32()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
    ModelScale = 1.0f;
    GlassColor = FRMVECTOR3( 1.0f, 1.0f, 1.0f );
    RotateAmount = 0.0f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject32::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );

    // rotation
    FRMMATRIX4X4 RotateMat = FrmMatrixRotate( RotateAmount, 0.0f, 1.0f, 0.0f );

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );


    ModelMatrix = FrmMatrixMultiply( ScaleMat, RotateMat );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject32::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample32()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample32::CSample32( const CHAR* strName ) : CScene( strName )
{
    m_DiffuseTexture = NULL;

    m_nWidth = 800;
    m_nHeight = 600;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample32::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    // NOTE SAME TEXTURES AS 27
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/27_Textures.pak" ) )
        return FALSE;

    // Create textures
    // JCOOK: This is not used and is loading wrong texture anyway
    // m_DiffuseTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) ) return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5, m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );

    // Load some meshes
    if( FALSE == m_LevelMesh.Load( "Demos/AdrenoShaders/Meshes/Map3.mesh" ) ) return FALSE;
    if( FALSE == m_GlassMesh[0].Load( "Demos/AdrenoShaders/Meshes/Teapot.mesh" ) ) return FALSE;
    if( FALSE == m_GlassMesh[1].Load( "Demos/AdrenoShaders/Meshes/Torus.mesh" ) ) return FALSE;

    if( FALSE == m_LevelMesh.MakeDrawable( &resource ) ) return FALSE;
    if( FALSE == m_GlassMesh[0].MakeDrawable( &resource ) ) return FALSE;
    if( FALSE == m_GlassMesh[1].MakeDrawable( &resource ) ) return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}


    // Set up the objects
    m_Object[0].Drawable = &m_LevelMesh;        // object 0 is the level
    m_Object[0].ModelScale = 1.0f;

    m_Object[1].Drawable = &m_GlassMesh[0];
    m_Object[1].Position = FRMVECTOR3( 4.5f + 0.3f, 1.2f, 0.0f );
    m_Object[1].ModelScale = 5.0f;
    m_Object[1].GlassColor = FRMVECTOR3( 174.f / 255.f, 243.f / 255.f, 175.f / 255.f );
    m_Object[1].RotateAmount = FRM_PI * 0.5f;

    m_Object[2].Drawable = &m_GlassMesh[1];
    m_Object[2].Position = FRMVECTOR3( 4.5f - 0.4f, 1.3f, 0.0f );
    m_Object[2].ModelScale = 0.15f;
    m_Object[2].GlassColor = FRMVECTOR3( 178.f / 255.f, 205.f / 255.f, 234.f / 255.f );

    m_AmbientLight = FRMVECTOR4( 0.1f, 0.1f, 0.1f, 0.0f );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample32::InitShaders()
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MaterialShader32.vs", "Demos/AdrenoShaders/Shaders/MaterialShader32.fs",
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

    // glass shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/GlassShader.vs", "Demos/AdrenoShaders/Shaders/GlassShader.fs",
            &m_GlassShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_GlassShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_GlassShader.ShaderId, "g_MatModelViewProj" );
        m_GlassShader.m_ModelMatrixId         = glGetUniformLocation( m_GlassShader.ShaderId, "g_MatModel" );
        m_GlassShader.m_NormalMatrixId        = glGetUniformLocation( m_GlassShader.ShaderId, "g_MatNormal" );
        m_GlassShader.m_LightPositionId       = glGetUniformLocation( m_GlassShader.ShaderId, "g_LightPos" );
        m_GlassShader.m_EyePositionId         = glGetUniformLocation( m_GlassShader.ShaderId, "g_EyePos" );
        m_GlassShader.m_BackgroundTextureId   = glGetUniformLocation( m_GlassShader.ShaderId, "g_BackgroundTexture" );
        m_GlassShader.m_GlassParamsId         = glGetUniformLocation( m_GlassShader.ShaderId, "g_GlassParams" );
    }

    // backdrop shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BackdropShader.vs", "Demos/AdrenoShaders/Shaders/BackdropShader.fs",
            &m_BackdropShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_BackdropShader.TextureId     = glGetUniformLocation( m_BackdropShader.ShaderId, "g_Texture" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample32::Resize()
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

    m_GlassShader.m_StepSize = FRMVECTOR2( 1.0f / (FLOAT32)m_nWidth, 1.0f / (FLOAT32)m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample32::Destroy()
{
    // Free shader memory
    m_MaterialShader.Destroy();
    m_GlassShader.Destroy();

    // Free texture memory
    if( m_DiffuseTexture ) m_DiffuseTexture->Release();

    // Free objects
    for( int i = 0; i < 3; i++ )
        m_Object[i].Destroy();

    // Free mesh memory
    m_LevelMesh.Destroy();
    for( int i = 0; i < 2; i++ )
        m_GlassMesh[i].Destroy();
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos32()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos32( FLOAT32 Time )
{
    FRMVECTOR3 BaseOffset = FRMVECTOR3( 4.5f, 2.0f, 0.0f );
    FRMVECTOR3 CurPosition = FRMVECTOR3( 1.4f * FrmCos( 0.25f * Time ), 0.2f * FrmSin( 0.15f * Time ), 1.4f * FrmSin( 0.25f * Time ) );
    return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample32::Update()
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
    m_LightPos = FRMVECTOR3( 4.5f, 4.0f, 0.0f );

    // animate the camera
    static FLOAT32 TotalTime = 0.0f;
    if( m_ShouldRotate )
    {
        static FLOAT32 CameraSpeed = 1.0f;
        TotalTime += ElapsedTime * CameraSpeed;
    }

    // build camera transforms
    FRMVECTOR3 UpDir = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 LookPos = FRMVECTOR3( 4.5f, 1.7f, 0.0f );
    m_CameraPos   = GetCameraPos32( TotalTime );
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // update objects
    for( int i = 0; i < 3; i++ )
        m_Object[i].Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: DrawLevelObject()
// Desc: The level uses resourced textures, which requires a slightly different setup
//--------------------------------------------------------------------------------------
VOID CSample32::DrawLevelObject( SimpleObject32* Object )
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
// Name: DrawGlassObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample32::DrawGlassObject( SimpleObject32* Object, UINT32 BackgroundTexture )
{
    FRMMATRIX4X4    MatModel;
    FRMMATRIX4X4    MatModelView;
    FRMMATRIX4X4    MatModelViewProj;
    FRMMATRIX3X3    MatNormal;
    FRMVECTOR3      Eye = m_CameraPos;

    if ( !Object || !Object->Drawable )
        return;

    MatModel         = Object->ModelMatrix;
    MatModelView     = FrmMatrixMultiply( MatModel, m_matView );
    MatModelViewProj = FrmMatrixMultiply( MatModelView, m_matProj );
    MatNormal        = FrmMatrixNormal( MatModel );

    static float Distort = 0.05f;

    glUseProgram( m_GlassShader.ShaderId );
    glUniformMatrix4fv( m_GlassShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_GlassShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix3fv( m_GlassShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_GlassShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_GlassShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4f( m_GlassShader.m_GlassParamsId, Object->GlassColor.r, Object->GlassColor.g, Object->GlassColor.b, Distort );
    SetTexture( m_GlassShader.m_BackgroundTextureId, BackgroundTexture, 2 );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: DrawOpaques()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample32::DrawOpaques()
{
    m_SharpRT.SetFramebuffer();

    glClearColor( 0.34f, 0.04f, 0.04f, 1.0f );
    glClearDepthf( 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glFrontFace( GL_CW );

    // object 0 is the level
    DrawLevelObject( &m_Object[0] );
}


//--------------------------------------------------------------------------------------
// Name: DrawGlass()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample32::DrawGlass()
{
    // Step 1: blit the back buffer, as a backdrop
    {
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glViewport( 0, 0, m_nWidth, m_nHeight );

        //glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
        //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        //glClear( GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_BackdropShader.ShaderId );
        SetTexture( m_BackdropShader.TextureId, m_SharpRT.TextureHandle, 0 );

        RenderScreenAlignedQuad();
    }

    // Step 2: draw glass objects
    {
        glClearColor( 0.04f, 0.04f, 0.34f, 1.0f );
        //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glClear( GL_DEPTH_BUFFER_BIT );

        glEnable( GL_CULL_FACE );
        glCullFace( GL_BACK );
        glEnable( GL_DEPTH_TEST );
        glDisable( GL_BLEND );
        glFrontFace( GL_CCW );

        // the two glass objects
        DrawGlassObject( &m_Object[1], m_SharpRT.TextureHandle );
        DrawGlassObject( &m_Object[2], m_SharpRT.TextureHandle );
    }
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample32::Render()
{
    // Draw regular stuff to the framebuffer
    DrawOpaques();

    // Blit that buffer back to the screen
    DrawGlass();

    // Clean up state
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glFrontFace( GL_CCW );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
