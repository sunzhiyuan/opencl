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
#include "Scene20.h"


static float ModelScale = 1.0f;


WoodShader::WoodShader()
{
    ShaderId = 0;
    SinTanTexture = NULL;
}


VOID WoodShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
    if( SinTanTexture ) SinTanTexture->Release();
}

VOID BlinnPhongShader20::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


BlinnPhongShader20::BlinnPhongShader20()
{
    ShaderId = 0;
}


SimpleObject20::SimpleObject20()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 6.8f;
    Drawable = NULL;
    DiffuseTexture = NULL;
    BumpTexture = NULL;
    DiffuseColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
    SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 5.0f );
}


VOID SimpleObject20::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.1f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    // rotate the object
    FRMMATRIX4X4 RotateMat1 = FrmMatrixRotate( RotateTime, 0.0f, 1.0f, 0.0f );
    FRMMATRIX4X4 RotateMat2 = FrmMatrixRotate( -0.3f,  1.0f, 0.0f, 0.0f );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat1 );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat2 );

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


VOID SimpleObject20::Destroy()
{
    if( DiffuseTexture ) DiffuseTexture->Release();
    if( BumpTexture ) BumpTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: CSample20()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample20::CSample20( const CHAR* strName ) : CScene( strName )
{
    m_nWidth = 900;
    m_nHeight = 400;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample20::InitShaders()
{
	// Create the wood shader
	{
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
            { "g_TangentIn",  FRM_VERTEX_TANGENT }
        };

		const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

		if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/WoodShader.vs", "Demos/AdrenoShaders/Shaders/WoodShader.fs",
			&m_WoodShader.ShaderId,
			pShaderAttributes, nNumShaderAttributes ) )
			return FALSE;

		// Make a record of the location for each shader constant
        m_WoodShader.m_ModelMatrixId         = glGetUniformLocation( m_WoodShader.ShaderId,  "g_MatModel" );
		m_WoodShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_WoodShader.ShaderId,  "g_MatModelViewProj" );
        m_WoodShader.m_NormalMatrixId        = glGetUniformLocation( m_WoodShader.ShaderId,  "g_MatNormal" );
		m_WoodShader.m_LightPositionId       = glGetUniformLocation( m_WoodShader.ShaderId,  "g_LightPos" );
        m_WoodShader.m_EyePositionId         = glGetUniformLocation( m_WoodShader.ShaderId,  "g_EyePos" );
		m_WoodShader.m_AmbientLightId        = glGetUniformLocation( m_WoodShader.ShaderId,  "g_AmbientLight" );
		m_WoodShader.m_DiffuseColorId        = glGetUniformLocation( m_WoodShader.ShaderId,  "g_DiffuseColor" );
		m_WoodShader.m_SpecularColorId       = glGetUniformLocation( m_WoodShader.ShaderId,  "g_SpecularColor" );
        m_WoodShader.m_BumpTextureId         = glGetUniformLocation( m_WoodShader.ShaderId,  "g_BumpTexture" );
        m_WoodShader.m_DiffuseTextureId      = glGetUniformLocation( m_WoodShader.ShaderId,  "g_DiffuseTexture" );
        m_WoodShader.m_SinTanTextureId       = glGetUniformLocation( m_WoodShader.ShaderId,  "g_SinTanTexture" );
	}

    // Create a Blinn-Phong shader, for reference
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BlinnPhongShader20.vs", "Demos/AdrenoShaders/Shaders/BlinnPhongShader20.fs",
            &m_BlinnPhongShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        m_BlinnPhongShader.m_NormalMatrixId        = glGetUniformLocation( m_BlinnPhongShader.ShaderId,  "g_MatNormal" );
        m_BlinnPhongShader.m_ModelMatrixId         = glGetUniformLocation( m_BlinnPhongShader.ShaderId,  "g_MatModel" );
        m_BlinnPhongShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_BlinnPhongShader.ShaderId,  "g_MatModelViewProj" );
        m_BlinnPhongShader.m_DiffuseColorId        = glGetUniformLocation( m_BlinnPhongShader.ShaderId,  "g_DiffuseColor" );
        m_BlinnPhongShader.m_SpecularColorId       = glGetUniformLocation( m_BlinnPhongShader.ShaderId,  "g_SpecularColor" );
        m_BlinnPhongShader.m_AmbientLightId        = glGetUniformLocation( m_BlinnPhongShader.ShaderId,  "g_AmbientLight" );
        m_BlinnPhongShader.m_LightPositionId       = glGetUniformLocation( m_BlinnPhongShader.ShaderId,  "g_LightDir" );
        m_BlinnPhongShader.m_DiffuseTextureId      = glGetUniformLocation( m_BlinnPhongShader.ShaderId,  "g_DiffuseTexture" );
    }

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample20::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/20_Textures.pak" ) )
        return FALSE;

    // Used for Oren-Nayar calculations
    m_WoodShader.SinTanTexture = resource.GetTexture( "SinTan" );

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
    m_CameraPos = FRMVECTOR3( 0.0f, 0.0f, 80.5f );
    float Split = 2.2f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    m_AmbientLight = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );

    // Set up the objects
    m_Object1.DiffuseTexture = resource.GetTexture( "WoodGrain1" );
    m_Object1.BumpTexture = resource.GetTexture( "WoodBump1" );
    m_Object1.Drawable = &m_Mesh;
    m_Object1.Position.x -= Split;
    m_Object1.SpecularColor = FRMVECTOR4( 0.2f, 0.2f, 0.2f, 5.0f );

    m_Object2.DiffuseTexture = resource.GetTexture( "WoodGrain2" );
    m_Object2.BumpTexture = resource.GetTexture( "WoodBump2" );
    m_Object2.Drawable = &m_Mesh;
    m_Object2.SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 40.0f );

    m_Object3.DiffuseTexture = resource.GetTexture( "WoodGrain3" );
    m_Object3.BumpTexture = resource.GetTexture( "WoodBump3" );
    m_Object3.Drawable = &m_Mesh;
    m_Object3.Position.x += Split;
    m_Object3.SpecularColor = FRMVECTOR4( 0.5f, 0.5f, 0.5f, 2.0f );

    glEnable( GL_DEPTH_TEST );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample20::Resize()
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
VOID CSample20::Destroy()
{
    // Free shader memory
    m_WoodShader.Destroy();
    m_BlinnPhongShader.Destroy();

    // Free objects
    m_Object1.Destroy();
    m_Object2.Destroy();
    m_Object3.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample20::Update()
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
    m_Object1.Update( ElapsedTime, m_ShouldRotate );
    m_Object2.Update( ElapsedTime, m_ShouldRotate );
    m_Object3.Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos20()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos20( FLOAT32 fTime )
{
    return FRMVECTOR3( 4.5f + 1.7f * FrmCos( 0.25f*fTime ), 
        2.0f + 1.1f * FrmSin( 0.15f*fTime ), 
        0.0f - 1.9f * FrmSin( 0.25f*fTime ) );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample20::DrawObject( SimpleObject20* Object )
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

    glUseProgram( m_WoodShader.ShaderId );
    glUniformMatrix4fv( m_WoodShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_WoodShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_WoodShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_WoodShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_WoodShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_WoodShader.m_AmbientLightId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform4fv( m_WoodShader.m_DiffuseColorId, 1, Object->DiffuseColor.v );
    glUniform4fv( m_WoodShader.m_SpecularColorId, 1, Object->SpecularColor.v );
    SetTexture( m_WoodShader.m_DiffuseTextureId, Object->DiffuseTexture->m_hTextureHandle, 0 );
    SetTexture( m_WoodShader.m_BumpTextureId, Object->BumpTexture->m_hTextureHandle, 1 );
    SetTexture( m_WoodShader.m_SinTanTextureId, m_WoodShader.SinTanTexture->m_hTextureHandle, 2 );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: DrawObjectBlinnPhong()
// Desc: Renders an object using the Blinn-Phong shading model
//--------------------------------------------------------------------------------------
VOID CSample20::DrawObjectBlinnPhong( SimpleObject20* Object )
{
    FRMMATRIX4X4    MatModel;
    FRMMATRIX4X4    MatModelView;
    FRMMATRIX4X4    MatModelViewProj;
    FRMMATRIX3X3    MatModelView3x3;

    if ( !Object || !Object->Drawable )
        return;

    MatModel         = Object->ModelMatrix;
    MatModelView     = FrmMatrixMultiply( Object->ModelMatrix, m_matView );
    MatModelViewProj = FrmMatrixMultiply( MatModelView, m_matProj );
    MatModelView3x3  = FrmMatrixNormalOrthonormal( MatModelView );


    glUseProgram( m_BlinnPhongShader.ShaderId );
    glUniformMatrix4fv( m_BlinnPhongShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_BlinnPhongShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix3fv( m_BlinnPhongShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatModelView3x3 );
    glUniform3fv( m_BlinnPhongShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_BlinnPhongShader.m_AmbientLightId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform4fv( m_BlinnPhongShader.m_DiffuseColorId, 1, Object->DiffuseColor.v );
    glUniform4fv( m_BlinnPhongShader.m_SpecularColorId, 1, Object->SpecularColor.v );
    SetTexture( m_BlinnPhongShader.m_DiffuseTextureId, Object->DiffuseTexture->m_hTextureHandle, 0 );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample20::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Draw some objects
    DrawObject( &m_Object1 );
    DrawObject( &m_Object2 );
    DrawObject( &m_Object3 );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
