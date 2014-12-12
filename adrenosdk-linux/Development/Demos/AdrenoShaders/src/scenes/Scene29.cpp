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
#include "Scene29.h"

float ModelScale29 = 1.0f;


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader29::MaterialShader29()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader29::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
DepthFogShader::DepthFogShader()
{
    ShaderId = 0;
    FogColors = FRMVECTOR4( 0.1f, 0.2f, 0.3f, 1.0f );
    FogStart = 1.7f;
    FogRange = 7.2f;
    FogDensity = 0.46f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID DepthFogShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject29::SimpleObject29()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject29::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale29, ModelScale29, ModelScale29 );
    ModelMatrix = ScaleMat;

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject29::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample29()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample29::CSample29( const CHAR* strName ) : CScene( strName )
{
    m_nWidth = 800;
    m_nHeight = 600;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample29::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
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
    m_UserInterface.AddFloatVariable( &m_DepthFogShader.FogDensity, (char *)"Fog density" );
    m_UserInterface.AddFloatVariable( &m_DepthFogShader.FogRange, (char *)"Fog transition range" );
    m_UserInterface.AddFloatVariable( &m_DepthFogShader.FogStart, (char *)"Fog start distance" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Decrease fog density" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Increase fog density" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Decrease fog range" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Increase fog range" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease fog start" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase fog start" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Map3.mesh" ) )
        return FALSE;
    m_CameraPos = FRMVECTOR3( 6.2f, 2.0f, 0.0f );
    float Split = 1.1f;
    float yAdjust = -0.05f;
    ModelScale29 = 1.0f;

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
BOOL CSample29::InitShaders()
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MaterialShader29.vs", "Demos/AdrenoShaders/Shaders/MaterialShader29.fs",
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/DepthFogShader.vs", "Demos/AdrenoShaders/Shaders/DepthFogShader.fs",
            &m_DepthFogShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_DepthFogShader.ColorTextureId = glGetUniformLocation( m_DepthFogShader.ShaderId, "g_ColorTexture" );
        m_DepthFogShader.DepthTextureId = glGetUniformLocation( m_DepthFogShader.ShaderId, "g_DepthTexture" );
        m_DepthFogShader.NearQId = glGetUniformLocation( m_DepthFogShader.ShaderId, "g_NearQ" );
        m_DepthFogShader.FogColorId = glGetUniformLocation( m_DepthFogShader.ShaderId, "g_FogColor" );
        m_DepthFogShader.FogParamsId = glGetUniformLocation( m_DepthFogShader.ShaderId, "g_FogParams" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample29::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    FLOAT32 FOVy = FrmRadians( 86 );
    FLOAT32 zNear = 0.25f;
    FLOAT32 zFar = 20.0f;
    m_matProj = FrmMatrixPerspectiveFovRH( FOVy, fAspect, zNear, zFar );

    // Q = Zf / Zf - Zn
    m_DepthFogShader.NearFarQ.x = zNear;
    m_DepthFogShader.NearFarQ.y = zFar;
    m_DepthFogShader.NearFarQ.z = ( zFar / ( zFar - zNear ) );

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
VOID CSample29::Destroy()
{
    // Free shader memory
    m_MaterialShader.Destroy();
    m_DepthFogShader.Destroy();

    // Free objects
    m_Object.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos29()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos29( FLOAT32 fTime )
{
    FRMVECTOR3 BaseOffset = FRMVECTOR3( -1.5f, 1.9f, 0.0f );
    FRMVECTOR3 CurPosition = FRMVECTOR3( 1.2f * FrmCos( 0.25f * fTime ), 0.1f * FrmSin( 0.15f * fTime ), -1.3f * FrmSin( 0.25f * fTime ) );
    return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample29::Update()
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
        m_DepthFogShader.FogDensity -= FogDensitySpeed * ElapsedTime;
        m_DepthFogShader.FogDensity = max( 0.1f, m_DepthFogShader.FogDensity );
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_DepthFogShader.FogDensity += FogDensitySpeed * ElapsedTime;
        m_DepthFogShader.FogDensity = min( 0.9f, m_DepthFogShader.FogDensity );
    }

    // change fog range
    FLOAT32 MinFogRange = 0.1f;
    FLOAT32 MaxFogRange = ( m_DepthFogShader.NearFarQ.y - m_DepthFogShader.NearFarQ.x ) * 0.8f;
    FLOAT32 FogRangeSpeed = ( MaxFogRange - MinFogRange ) * 0.75f;
    if( nButtons & INPUT_KEY_4 )
    {
        m_DepthFogShader.FogRange -= FogRangeSpeed * ElapsedTime;
        m_DepthFogShader.FogRange = max( MinFogRange, m_DepthFogShader.FogRange );
    }

    if( nButtons & INPUT_KEY_5 )
    {
        m_DepthFogShader.FogRange += FogRangeSpeed * ElapsedTime;
        m_DepthFogShader.FogRange = min( MaxFogRange, m_DepthFogShader.FogRange );
    }


    // change fog start
    FLOAT32 FogStartSpeed = ( m_DepthFogShader.NearFarQ.y - m_DepthFogShader.NearFarQ.x ) * 0.75f;
    if( nButtons & INPUT_KEY_6 )
    {
        m_DepthFogShader.FogStart -= FogStartSpeed * ElapsedTime;
        m_DepthFogShader.FogStart = max( m_DepthFogShader.NearFarQ.x, m_DepthFogShader.FogStart );
    }

    if( nButtons & INPUT_KEY_7 )
    {
        FLOAT32 Speed = ( m_DepthFogShader.NearFarQ.y - m_DepthFogShader.NearFarQ.x ) * 0.5f;

        m_DepthFogShader.FogStart += FogStartSpeed * ElapsedTime;
        m_DepthFogShader.FogStart = min( m_DepthFogShader.NearFarQ.y, m_DepthFogShader.FogStart );
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
    FRMVECTOR3 LookPos = GetCameraPos29( TotalTime + 0.05f );
    m_CameraPos   = GetCameraPos29( TotalTime );
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // update objects
    m_Object.Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample29::DrawObject( SimpleObject29* Object )
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
VOID CSample29::DrawScene()
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
VOID CSample29::DrawFoggedScene()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDisable( GL_CULL_FACE );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );

    glUseProgram( m_DepthFogShader.ShaderId );
    SetTexture( m_DepthFogShader.ColorTextureId, m_OffscreenRT.TextureHandle, 0 );
    SetTexture( m_DepthFogShader.DepthTextureId, m_OffscreenRT.DepthTextureHandle, 2 );
    glUniform2f( m_DepthFogShader.NearQId, m_DepthFogShader.NearFarQ.x, m_DepthFogShader.NearFarQ.z );
    glUniform4fv( m_DepthFogShader.FogColorId, 1, m_DepthFogShader.FogColors.v );
    glUniform3f( m_DepthFogShader.FogParamsId, m_DepthFogShader.FogStart, m_DepthFogShader.FogRange, m_DepthFogShader.FogDensity );

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample29::Render()
{

    // Draw regular stuff to the offscreen framebuffer
    DrawScene();

    // Add fog while overlaying to the screen
    DrawFoggedScene();

    // Clean up state
//    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glFrontFace( GL_CCW );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
