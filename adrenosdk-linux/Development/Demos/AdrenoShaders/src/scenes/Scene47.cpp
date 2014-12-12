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
#include "Scene47.h"


static float ModelScale = 1.0f;


EnvShader::EnvShader()
{
    ShaderId = 0;
}


VOID EnvShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


SimpleObject47::SimpleObject47()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 4.0f;
    Drawable = NULL;
    BumpTexture = NULL;
    SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 5.0f );
    ReflectionStrength = 0.1f;
}


VOID SimpleObject47::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    FRMMATRIX4X4 RotateMat1 = FrmMatrixRotate( FRM_PI * 0.5f,  1.0f, 0.0f, 0.0f );
    FRMMATRIX4X4 RotateMat2 = FrmMatrixRotate( RotateTime, 0.0f, 1.0f, 0.0f );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat1 );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat2 );

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


VOID SimpleObject47::Destroy()
{
    if( BumpTexture ) BumpTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: CSample47()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample47::CSample47( const CHAR* strName ) : CScene( strName )
{
    m_pLogoTexture = NULL;
    m_CubemapHandle = 0;

    m_nWidth = 600;
    m_nHeight = 400;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample47::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/47_Textures.pak" ) )
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
    m_Object[0].BumpTexture = resource.GetTexture( "FlatBump" );
    m_Object[0].Drawable = &m_Mesh;
    m_Object[0].Position.x -= Split;
    m_Object[0].Position.y += yAdjust;
    m_Object[0].SpecularColor = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 60.0f );
    m_Object[0].ReflectionStrength = 1.0f;

    m_Object[1].BumpTexture = resource.GetTexture( "fieldstoneBump" );
    m_Object[1].Drawable = &m_Mesh;
    m_Object[1].Position.x += Split;
    m_Object[1].Position.y += yAdjust;
    m_Object[1].SpecularColor = FRMVECTOR4( 2.0f, 2.0f, 2.0f, 80.0f );
    m_Object[1].ReflectionStrength = 1.0f;

    glEnable( GL_DEPTH_TEST );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample47::InitShaders()
{
    // Create the plastic shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
            { "g_TangentIn",  FRM_VERTEX_TANGENT }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/EnvShader.vs", "Demos/AdrenoShaders/Shaders/EnvShader.fs",
            &m_EnvShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_EnvShader.m_ModelMatrixId         = glGetUniformLocation( m_EnvShader.ShaderId,  "g_MatModel" );
        m_EnvShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_EnvShader.ShaderId,  "g_MatModelViewProj" );
        m_EnvShader.m_NormalMatrixId        = glGetUniformLocation( m_EnvShader.ShaderId,  "g_MatNormal" );
        m_EnvShader.m_LightPositionId       = glGetUniformLocation( m_EnvShader.ShaderId,  "g_LightPos" );
        m_EnvShader.m_EyePositionId         = glGetUniformLocation( m_EnvShader.ShaderId,  "g_EyePos" );
        m_EnvShader.m_AmbientLightId        = glGetUniformLocation( m_EnvShader.ShaderId,  "g_AmbientLight" );
        m_EnvShader.m_SpecularColorId       = glGetUniformLocation( m_EnvShader.ShaderId,  "g_SpecularColor" );
        m_EnvShader.m_BumpTextureId         = glGetUniformLocation( m_EnvShader.ShaderId,  "g_BumpTexture" );
        m_EnvShader.m_ReflectionStrId       = glGetUniformLocation( m_EnvShader.ShaderId,  "g_ReflectionStrength" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample47::Resize()
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
VOID CSample47::Destroy()
{
    // Put the state back
    // Need to do it here instead of after each render because some
    // states are not set each frame and this breaks it.
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glFrontFace( GL_CCW );

    // Free shader memory
    m_EnvShader.Destroy();

    // Free texture memory
    if( m_pLogoTexture ) m_pLogoTexture->Release();

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
VOID CSample47::Update()
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
    static FLOAT32 LightOrbitalTime = 0.0f;
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
    for( int i = 0; i < 2; i++ )
        m_Object[i].Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos47()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos47( FLOAT32 fTime )
{
    return FRMVECTOR3( 4.5f + 1.7f * FrmCos( 0.25f*fTime ), 
        2.0f + 1.1f * FrmSin( 0.15f*fTime ), 
        0.0f - 1.9f * FrmSin( 0.25f*fTime ) );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample47::DrawObject( SimpleObject47* Object )
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

    glUseProgram( m_EnvShader.ShaderId );
    glUniformMatrix4fv( m_EnvShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_EnvShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_EnvShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_EnvShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_EnvShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_EnvShader.m_AmbientLightId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform4fv( m_EnvShader.m_SpecularColorId, 1, Object->SpecularColor.v );
    glUniform1f( m_EnvShader.m_ReflectionStrId, Object->ReflectionStrength );

    // note: the 2D samples start at index 1 when using a cube map.  It doesn't
    // matter if the cube sampler comes first or last in the shader.
    SetTexture( m_EnvShader.m_BumpTextureId, Object->BumpTexture->m_hTextureHandle, 1 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_CubemapHandle );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample47::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Draw some objects
    for( int i = 0; i < 2; i++ )
        DrawObject( &m_Object[i] );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );

}
