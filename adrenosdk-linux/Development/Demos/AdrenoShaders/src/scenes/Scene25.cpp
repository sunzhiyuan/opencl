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
#include "Scene25.h"


float ModelScale25 = 1.0f;

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
FishShader::FishShader()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID FishShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject25::SimpleObject25()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
    BumpTexture = NULL;
    DiffuseColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
    SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 5.0f );
    SpecularColor2 = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 5.0f );
    DiffuseTexture = NULL;
    Tiling = FRMVECTOR2( 1.0f, 1.0f );
    ReflectionStrength = 1.0f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject25::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale25, ModelScale25, ModelScale25 );
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
VOID SimpleObject25::Destroy()
{
    if( DiffuseTexture ) DiffuseTexture->Release();
    if( BumpTexture ) BumpTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: CSample25()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample25::CSample25( const CHAR* strName ) : CScene( strName )
{
    m_CubemapHandle = 0;

    m_nWidth = 700;
    m_nHeight = 500;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample25::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/25_Textures.pak" ) )
        return FALSE;

    // Load the cube map for reflections
    if (!LoadCubeMap("00_Cubemap"))
        return FALSE;


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
    m_CameraPos = FRMVECTOR3( 0.0f, 0.0f, 40.0f );
    float Split = 1.1f;
    float yAdjust = -0.05f;
    ModelScale25 = 1.0f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    // Set up lights
    m_AmbientLight = FRMVECTOR4( 0 / 255.0f, 159 / 255.0f, 200 / 255.0f, 0.0f ) * 0.01f;
    m_LightPos = FRMVECTOR3( -10.0f, 8.0f, 7.5f );
    m_LightPos2 = FRMVECTOR3( 10.0f, 5.0f, 0.0f );

    // Set up the objects
    m_Object.DiffuseTexture = resource.GetTexture( "Fish1" );
    m_Object.BumpTexture = resource.GetTexture( "FishBump1" );
    m_Object.Drawable = &m_Mesh;
    m_Object.Position += FRMVECTOR3( 0.0f, yAdjust, 0.0f );
    m_Object.SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 7.0f );
    m_Object.SpecularColor2 = FRMVECTOR4( 162 / 255.0f, 176 / 255.0f, 189 / 255.0f, 15.0f );
    m_Object.Tiling = FRMVECTOR2( 4.0f, 2.0f );
    m_Object.ReflectionStrength = 0.1f;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample25::InitShaders()
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/FishShader.vs", "Demos/AdrenoShaders/Shaders/FishShader.fs",
            &m_FishShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_FishShader.m_ModelMatrixId         = glGetUniformLocation( m_FishShader.ShaderId, "g_MatModel" );
        m_FishShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_FishShader.ShaderId, "g_MatModelViewProj" );
        m_FishShader.m_NormalMatrixId        = glGetUniformLocation( m_FishShader.ShaderId, "g_MatNormal" );
        m_FishShader.m_LightPositionId       = glGetUniformLocation( m_FishShader.ShaderId, "g_LightPos" );
        m_FishShader.m_LightPosition2Id      = glGetUniformLocation( m_FishShader.ShaderId, "g_LightPos2" );
        m_FishShader.m_EyePositionId         = glGetUniformLocation( m_FishShader.ShaderId, "g_EyePos" );
        m_FishShader.m_DiffuseColorId        = glGetUniformLocation( m_FishShader.ShaderId, "g_DiffuseColor" );
        m_FishShader.m_SpecularColorId       = glGetUniformLocation( m_FishShader.ShaderId, "g_SpecularColor" );
        m_FishShader.m_SpecularColor2Id      = glGetUniformLocation( m_FishShader.ShaderId, "g_SpecularColor2" );
        m_FishShader.m_AmbientColorId        = glGetUniformLocation( m_FishShader.ShaderId, "g_AmbientColor" );
        m_FishShader.m_DiffuseTextureId      = glGetUniformLocation( m_FishShader.ShaderId, "g_DiffuseTexture" );
        m_FishShader.m_BumpTextureId         = glGetUniformLocation( m_FishShader.ShaderId, "g_BumpTexture" );
        m_FishShader.m_TilingId              = glGetUniformLocation( m_FishShader.ShaderId, "g_Tiling" );
        m_FishShader.m_ReflectionStrId       = glGetUniformLocation( m_FishShader.ShaderId, "g_ReflectionStrength" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample25::Resize()
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
VOID CSample25::Destroy()
{
    // Free shader memory
    m_FishShader.Destroy();

    // Free objects
    m_Object.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample25::Update()
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

    // build camera transforms
    FRMVECTOR3 LookDir   = FRMVECTOR3( 0.0f, 0.0f, -1.0f );
    FRMVECTOR3 UpDir     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 Temp      = m_CameraPos + LookDir;
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, Temp, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // update objects
    m_Object.Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos25()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos25( FLOAT32 fTime )
{
    return FRMVECTOR3( 4.5f + 1.7f * FrmCos( 0.25f*fTime ), 
        2.0f + 1.1f * FrmSin( 0.15f*fTime ), 
        0.0f - 1.9f * FrmSin( 0.25f*fTime ) );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample25::DrawObject( SimpleObject25* Object )
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

    glUseProgram( m_FishShader.ShaderId );
    glUniformMatrix4fv( m_FishShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_FishShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_FishShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_FishShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_FishShader.m_LightPositionId, 1, m_LightPos.v );
    glUniform3fv( m_FishShader.m_LightPosition2Id, 1, m_LightPos2.v );
    glUniform4fv( m_FishShader.m_SpecularColorId, 1, Object->SpecularColor.v );
    glUniform4fv( m_FishShader.m_SpecularColor2Id, 1, Object->SpecularColor2.v );
    glUniform4fv( m_FishShader.m_AmbientColorId, 1, m_AmbientLight.v );
    glUniform4fv( m_FishShader.m_DiffuseColorId, 1, Object->DiffuseColor.v );
    glUniform2fv( m_FishShader.m_TilingId, 1, Object->Tiling.v );
    glUniform1f( m_FishShader.m_ReflectionStrId, Object->ReflectionStrength );

    glBindTexture( GL_TEXTURE_CUBE_MAP, m_CubemapHandle );
    SetTexture( m_FishShader.m_DiffuseTextureId, Object->DiffuseTexture->m_hTextureHandle, 1 );
    SetTexture( m_FishShader.m_BumpTextureId, Object->BumpTexture->m_hTextureHandle, 2 );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample25::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );

    DrawObject( &m_Object );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
