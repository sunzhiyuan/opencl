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
#include "Scene30.h"

float ModelScale30 = 1.0f;


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader30::MaterialShader30()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader30::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
HeightFogShader::HeightFogShader()
{
    ShaderId = 0;
    FogColors = FRMVECTOR4( 0.1f, 0.2f, 0.3f, 1.0f );
    FogHeight = 0.5f;
    FogRange = 2.6f;
    FogDensity = 0.8f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID HeightFogShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject30::SimpleObject30()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject30::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale30, ModelScale30, ModelScale30 );
    ModelMatrix = ScaleMat;

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject30::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample30()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample30::CSample30( const CHAR* strName ) : CScene( strName )
{
    m_nWidth = 800;
    m_nHeight = 600;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample30::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
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
    m_UserInterface.AddFloatVariable( &m_HeightFogShader.FogDensity, (char *)"Fog density" );
    m_UserInterface.AddFloatVariable( &m_HeightFogShader.FogRange, (char *)"Fog transition range" );
    m_UserInterface.AddFloatVariable( &m_HeightFogShader.FogHeight, (char *)"Fog height" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Decrease fog density" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Increase fog density" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Decrease fog range" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Increase fog range" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease fog height" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase fog height" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Map3.mesh" ) )
        return FALSE;
    m_CameraPos = FRMVECTOR3( 6.2f, 2.0f, 0.0f );
    float Split = 1.1f;
    float yAdjust = -0.05f;
    ModelScale30 = 1.0f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    m_AmbientLight = FRMVECTOR4( 0.05f, 0.05f, 0.05f, 0.0f );

    // Set up the objects
    m_Object.Drawable = &m_Mesh;
    m_Object.Position.y += yAdjust;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample30::InitShaders()
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MaterialShader30.vs", "Demos/AdrenoShaders/Shaders/MaterialShader30.fs",
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

    // fog shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/HeightFogShader.vs", "Demos/AdrenoShaders/Shaders/HeightFogShader.fs",
            &m_HeightFogShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_HeightFogShader.ColorTextureId = glGetUniformLocation( m_HeightFogShader.ShaderId, "g_ColorTexture" );
        m_HeightFogShader.DepthTextureId = glGetUniformLocation( m_HeightFogShader.ShaderId, "g_DepthTexture" );
        m_HeightFogShader.ModelViewProjInvMatrixId = glGetUniformLocation( m_HeightFogShader.ShaderId, "g_MatModelViewProjInv" );
        m_HeightFogShader.CamPosId = glGetUniformLocation( m_HeightFogShader.ShaderId, "g_CamPos" );
        m_HeightFogShader.FogColorId = glGetUniformLocation( m_HeightFogShader.ShaderId, "g_FogColor" );
        m_HeightFogShader.FogParamsId = glGetUniformLocation( m_HeightFogShader.ShaderId, "g_FogParams" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample30::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    FLOAT32 FOVy = FrmRadians( 86 );
    zNear = 0.25f;
    zFar = 20.0f;
    m_matProj = FrmMatrixPerspectiveFovRH( FOVy, fAspect, zNear, zFar );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    m_OffscreenRT.CreateRT( m_nWidth, m_nHeight, TRUE );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample30::Destroy()
{
    // Free shader memory
    m_MaterialShader.Destroy();
    m_HeightFogShader.Destroy();

    // Free objects
    m_Object.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos30()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos30( FLOAT32 fTime )
{
    FRMVECTOR3 BaseOffset = FRMVECTOR3( -1.5f, 1.9f, 0.0f );
    FRMVECTOR3 CurPosition = FRMVECTOR3( 1.2f * FrmCos( 0.25f * fTime ), 0.1f * FrmSin( 0.15f * fTime ), -1.3f * FrmSin( 0.25f * fTime ) );
    return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample30::Update()
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

    // change fog density
    FLOAT32 FogDensitySpeed = 0.8f;
    if( nButtons & INPUT_KEY_2 )
    {
        m_HeightFogShader.FogDensity -= FogDensitySpeed * ElapsedTime;
        m_HeightFogShader.FogDensity = max( 0.1f, m_HeightFogShader.FogDensity );
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_HeightFogShader.FogDensity += FogDensitySpeed * ElapsedTime;
        m_HeightFogShader.FogDensity = min( 0.9f, m_HeightFogShader.FogDensity );
    }

    // change fog transition range
    FLOAT32 FogRangeSpeed = 4.5f;
    if( nButtons & INPUT_KEY_4 )
    {
        m_HeightFogShader.FogRange -= FogRangeSpeed * ElapsedTime;
        m_HeightFogShader.FogRange = max( 0.5f, m_HeightFogShader.FogRange );
    }

    if( nButtons & INPUT_KEY_5 )
    {
        m_HeightFogShader.FogRange += FogRangeSpeed * ElapsedTime;
        m_HeightFogShader.FogRange = min( 5.0f, m_HeightFogShader.FogRange );
    }

    // change fog height
    FLOAT32 FogHeightSpeed = 5.0f;
    if( nButtons & INPUT_KEY_6 )
    {
        m_HeightFogShader.FogHeight -= FogHeightSpeed * ElapsedTime;
        m_HeightFogShader.FogHeight = max( -1.0f, m_HeightFogShader.FogHeight );
    }

    if( nButtons & INPUT_KEY_7 )
    {
        m_HeightFogShader.FogHeight += FogHeightSpeed * ElapsedTime;
        m_HeightFogShader.FogHeight = min( 4.0f, m_HeightFogShader.FogHeight );
    }


    // update light position
    m_LightPos = FRMVECTOR3( -1.5f, 3.0f, 0.0f );

    // animate the camera
    static FLOAT32 TotalTime = 12.0f;
    if( m_ShouldRotate )
    {
        static FLOAT32 CameraSpeed = 1.0f;
        TotalTime += ElapsedTime * CameraSpeed;
    }

    // build camera transforms
    FRMVECTOR3 UpDir = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 LookPos = GetCameraPos30( TotalTime + 0.05f );
    m_CameraPos   = GetCameraPos30( TotalTime );
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // update objects
    m_Object.Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample30::DrawObject( SimpleObject30* Object )
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
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample30::DrawScene()
{
    m_OffscreenRT.SetFramebuffer();

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClearDepthf( 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glFrontFace( GL_CW );

    DrawObject( &m_Object );
    
   m_OffscreenRT.DetachFramebuffer(); 
}



//--------------------------------------------------------------------------------------
// Name: DrawCombinedScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample30::DrawFoggedScene()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDisable( GL_CULL_FACE );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );

    FRMMATRIX4X4 MatModel            = FrmMatrixIdentity();
    FRMMATRIX4X4 MatModelView        = FrmMatrixMultiply( MatModel, m_matView );
    FRMMATRIX4X4 MatModelViewProj    = FrmMatrixMultiply( MatModelView, m_matProj );
    FRMMATRIX4X4 MatModelViewProjInv = FrmMatrixInverse( MatModelViewProj );
    FRMVECTOR3   CamPos              = FRMVECTOR3( -MatModelView.m[3][0], -MatModelView.m[3][1], -MatModelView.m[3][2] );

    glUseProgram( m_HeightFogShader.ShaderId );
    SetTexture( m_HeightFogShader.ColorTextureId, m_OffscreenRT.TextureHandle, 0 );
    SetTexture( m_HeightFogShader.DepthTextureId, m_OffscreenRT.DepthTextureHandle, 2 );
    glUniformMatrix4fv( m_HeightFogShader.ModelViewProjInvMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProjInv );
    glUniform4fv( m_HeightFogShader.FogColorId, 1, m_HeightFogShader.FogColors.v );
    glUniform3f( m_HeightFogShader.FogParamsId, m_HeightFogShader.FogHeight + m_HeightFogShader.FogRange * 0.5f, m_HeightFogShader.FogHeight - m_HeightFogShader.FogRange * 0.5f, m_HeightFogShader.FogDensity );
    glUniform3fv( m_HeightFogShader.CamPosId, 1, CamPos.v );

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample30::Render()
{

    // Draw regular stuff to the offscreen framebuffer
    DrawScene();

    // Add fog while overlaying to the screen
    DrawFoggedScene();

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
