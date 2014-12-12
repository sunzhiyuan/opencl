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
#include "Scene22.h"


float ModelScale = 1.0f;
INT32 EyeColor = 0;



//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
EyeShader::EyeShader()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID EyeShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject22::SimpleObject22()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
    BumpTexture = NULL;
    DiffuseColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
    SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 5.0f );
    ReflectionStrength = 1.0f;
    IrisGlow = 1.0f;

    for( int i = 0; i < EYE_COLORS; i++ )
        DiffuseTexture[i] = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject22::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    // spin it
    // PI already defined const FLOAT32 PI = 3.141592f;
    const FLOAT32 RotationOffset = PI * 0.4f;
    const FLOAT32 TurnAmount = PI * 0.25f;
    FRMMATRIX4X4 RotateMat = FrmMatrixRotate( -sin( RotateTime * 1.5f ) * TurnAmount + RotationOffset, 0.0f, 1.0f, 0.0f );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat );

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject22::Destroy()
{
    for( int i = 0; i < EYE_COLORS; i++ )
        if( DiffuseTexture[i] ) DiffuseTexture[i]->Release();
    if( BumpTexture ) BumpTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: CSample22()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample22::CSample22( const CHAR* strName ) : CScene( strName )
{
    m_CubemapHandle = 0;

    m_nWidth = 700;
    m_nHeight = 500;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample22::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/22_Textures.pak" ) )
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
    m_UserInterface.AddFloatVariable( &m_Object.IrisGlow, (char *)"Iris light" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, (char *)"Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Change eye color -" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Change eye color +" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease iris light" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase iris light" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Sphere3.mesh" ) )
        return FALSE;
    m_CameraPos = FRMVECTOR3( 0.0f, 0.0f, 44.0f );
    float Split = 1.1f;
    float yAdjust = -0.05f;
    ModelScale = 0.7f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    m_AmbientLight = FRMVECTOR4( 0.0f, 0.1f, 0.16f, 0.0f );

    // Set up the objects
    m_Object.DiffuseTexture[0] = resource.GetTexture( "Eye1" );
    m_Object.DiffuseTexture[1] = resource.GetTexture( "Eye2" );
    m_Object.DiffuseTexture[2] = resource.GetTexture( "Eye3" );
    m_Object.BumpTexture = resource.GetTexture( "EyeBump1" );
    m_Object.Drawable = &m_Mesh;
    m_Object.Position.y += yAdjust;
    m_Object.SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 65.0f );
    m_Object.ReflectionStrength = 0.3f;
    m_Object.IrisGlow = 3.0f;

//     m_Object.RotateTime = 6.7466364f;
//     m_ShouldRotate = FALSE;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample22::InitShaders()
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/EyeShader.vs", "Demos/AdrenoShaders/Shaders/EyeShader.fs",
            &m_EyeShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_EyeShader.m_ModelMatrixId         = glGetUniformLocation( m_EyeShader.ShaderId, "g_MatModel" );
        m_EyeShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_EyeShader.ShaderId, "g_MatModelViewProj" );
        m_EyeShader.m_NormalMatrixId        = glGetUniformLocation( m_EyeShader.ShaderId, "g_MatNormal" );
        m_EyeShader.m_LightPositionId       = glGetUniformLocation( m_EyeShader.ShaderId, "g_LightPos" );
        m_EyeShader.m_EyePositionId         = glGetUniformLocation( m_EyeShader.ShaderId, "g_EyePos" );
        m_EyeShader.m_SpecularColorId       = glGetUniformLocation( m_EyeShader.ShaderId, "g_SpecColor" );
        m_EyeShader.m_AmbientColorId        = glGetUniformLocation( m_EyeShader.ShaderId, "g_AmbientColor" );
        m_EyeShader.m_DiffuseTextureId      = glGetUniformLocation( m_EyeShader.ShaderId, "g_DiffuseTexture" );
        m_EyeShader.m_BumpTextureId         = glGetUniformLocation( m_EyeShader.ShaderId, "g_BumpTexture" );
        m_EyeShader.m_ReflectionStrId       = glGetUniformLocation( m_EyeShader.ShaderId, "g_ReflectionStrength" );
        m_EyeShader.m_IrisGlowId            = glGetUniformLocation( m_EyeShader.ShaderId, "g_IrisGlow" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample22::Resize()
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
VOID CSample22::Destroy()
{
    // Free shader memory
    m_EyeShader.Destroy();

    // Free objects
    m_Object.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample22::Update()
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

    if( nPressedButtons & INPUT_KEY_4 )
    {
        EyeColor--;
        if( EyeColor < 0 )
            EyeColor = EYE_COLORS - 1;
    }

    if( nPressedButtons & INPUT_KEY_5 )
    {
        EyeColor++;
        if( EyeColor >= EYE_COLORS )
            EyeColor = 0;
    }

    if( nButtons & INPUT_KEY_6 )
    {
        m_Object.IrisGlow -= 4.0f * ElapsedTime;
        m_Object.IrisGlow = max( 0.0f, m_Object.IrisGlow );
    }

    if( nButtons & INPUT_KEY_7 )
    {
        m_Object.IrisGlow += 4.0f * ElapsedTime;
        m_Object.IrisGlow = min( 8.0f, m_Object.IrisGlow );
    }

    // update light position
    m_LightPos = FRMVECTOR3( -4.0f, 10.0f, 10.0f );

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
// Name: GetCameraPos22()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos22( FLOAT32 fTime )
{
    return FRMVECTOR3( 4.5f + 1.7f * FrmCos( 0.25f*fTime ), 
        2.0f + 1.1f * FrmSin( 0.15f*fTime ), 
        0.0f - 1.9f * FrmSin( 0.25f*fTime ) );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample22::DrawObject( SimpleObject22* Object )
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

    glUseProgram( m_EyeShader.ShaderId );
    glUniformMatrix4fv( m_EyeShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_EyeShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_EyeShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_EyeShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_EyeShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_EyeShader.m_SpecularColorId, 1, (FLOAT32*)&Object->SpecularColor.v );
    glUniform4fv( m_EyeShader.m_AmbientColorId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform1f( m_EyeShader.m_ReflectionStrId, Object->ReflectionStrength );
    glUniform1f( m_EyeShader.m_IrisGlowId, Object->IrisGlow );

    // note: the 2D samples start at index 1 when using a cube map.  It doesn't
    // matter if the cube sampler comes first or last in the shader.
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_CubemapHandle );
    SetTexture( m_EyeShader.m_DiffuseTextureId, Object->DiffuseTexture[EyeColor]->m_hTextureHandle, 1 );
    SetTexture( m_EyeShader.m_BumpTextureId, Object->BumpTexture->m_hTextureHandle, 2 );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample22::Render()
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

    // Put the state back
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
}
