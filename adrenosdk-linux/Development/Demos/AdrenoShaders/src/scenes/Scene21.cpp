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
#include "Scene21.h"


PlasticShader::PlasticShader()
{
    ShaderId = 0;
}


VOID PlasticShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

VOID BlinnPhongShader21::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


BlinnPhongShader21::BlinnPhongShader21()
{
    ShaderId = 0;
}


SimpleObject21::SimpleObject21()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 4.0f;
    Drawable = NULL;
    DiffuseTexture = NULL;
    BumpTexture = NULL;
    DiffuseColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
    SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 5.0f );
    ReflectionStrength = 0.1f;
    ModelScale = 1.0f;
    Tilt = TRUE;
}


VOID SimpleObject21::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    FRMMATRIX4X4 RotateMat1 = FrmMatrixRotate( FRM_PI * 0.5f,  1.0f, 0.0f, 0.0f );
    FRMMATRIX4X4 RotateMat2 = FrmMatrixRotate( RotateTime, 0.0f, 1.0f, 0.0f );
    if( Tilt )
        ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat1 );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat2 );

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


VOID SimpleObject21::Destroy()
{
    if( DiffuseTexture ) DiffuseTexture->Release();
    if( BumpTexture ) BumpTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: CSample21()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample21::CSample21( const CHAR* strName ) : CScene( strName )
{
    m_CubemapHandle = 0;

    m_nWidth = 800;
    m_nHeight = 600;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample21::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/21_Textures.pak" ) )
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
    if( FALSE == m_Mesh[0].Load( "Demos/AdrenoShaders/Meshes/Sphere3.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh[1].Load( "Demos/AdrenoShaders/Meshes/Torus2.mesh" ) )
        return FALSE;

    m_CameraPos = FRMVECTOR3( 0.0f, 0.0f, 55.0f );
    float Split = 1.1f;
    float yAdjust = 0.0;

    for( int i = 0; i < 2; i++ )
    {
        if( FALSE == m_Mesh[i].MakeDrawable( &resource ) )
            return FALSE;
    }

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    m_AmbientLight = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );

    // Set up the objects
    m_Object[0].DiffuseTexture = resource.GetTexture( "Plastic1" );
    m_Object[0].BumpTexture = resource.GetTexture( "PlasticBump1" );
    m_Object[0].Drawable = &m_Mesh[0];
    m_Object[0].Position.x -= Split;
    m_Object[0].Position.y += yAdjust;
    m_Object[0].SpecularColor = FRMVECTOR4( 0.5f, 0.5f, 0.5f, 40.0f );
    m_Object[0].ReflectionStrength = 0.05f;
    m_Object[0].Tilt = FALSE;

    m_Object[1].DiffuseTexture = resource.GetTexture( "Plastic2" );
    m_Object[1].BumpTexture = resource.GetTexture( "PlasticBump2" );
    m_Object[1].Drawable = &m_Mesh[1];
    m_Object[1].Position.x += Split;
    m_Object[1].Position.y += yAdjust;
    m_Object[1].SpecularColor = FRMVECTOR4( 0.5f, 0.5f, 0.5f, 2.0f );
    m_Object[1].ReflectionStrength = 0.1f;
    m_Object[1].ModelScale = 0.7f;

    glEnable( GL_DEPTH_TEST );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample21::InitShaders()
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PlasticShader.vs", "Demos/AdrenoShaders/Shaders/PlasticShader.fs",
            &m_PlasticShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_PlasticShader.m_ModelMatrixId         = glGetUniformLocation( m_PlasticShader.ShaderId,  "g_MatModel" );
        m_PlasticShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_PlasticShader.ShaderId,  "g_MatModelViewProj" );
        m_PlasticShader.m_NormalMatrixId        = glGetUniformLocation( m_PlasticShader.ShaderId,  "g_MatNormal" );
        m_PlasticShader.m_LightPositionId       = glGetUniformLocation( m_PlasticShader.ShaderId,  "g_LightPos" );
        m_PlasticShader.m_EyePositionId         = glGetUniformLocation( m_PlasticShader.ShaderId,  "g_EyePos" );
        m_PlasticShader.m_AmbientLightId        = glGetUniformLocation( m_PlasticShader.ShaderId,  "g_AmbientLight" );
        m_PlasticShader.m_DiffuseColorId        = glGetUniformLocation( m_PlasticShader.ShaderId,  "g_DiffuseColor" );
        m_PlasticShader.m_SpecularColorId       = glGetUniformLocation( m_PlasticShader.ShaderId,  "g_SpecularColor" );
        m_PlasticShader.m_BumpTextureId         = glGetUniformLocation( m_PlasticShader.ShaderId,  "g_BumpTexture" );
        m_PlasticShader.m_DiffuseTextureId      = glGetUniformLocation( m_PlasticShader.ShaderId,  "g_DiffuseTexture" );
        m_PlasticShader.m_ReflectionStrId       = glGetUniformLocation( m_PlasticShader.ShaderId,  "g_ReflectionStrength" );
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BlinnPhongShader21.vs", "Demos/AdrenoShaders/Shaders/BlinnPhongShader21.fs",
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
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample21::Resize()
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
VOID CSample21::Destroy()
{
    // Free shader memory
    m_PlasticShader.Destroy();
    m_BlinnPhongShader.Destroy();

    // Free objects
    for( int i = 0; i < 2; i++ )
        m_Object[i].Destroy();

    // Free mesh memory
    for( int i = 0; i < 2; i++ )
        m_Mesh[i].Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample21::Update()
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
// Name: GetCameraPos21()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos21( FLOAT32 fTime )
{
    return FRMVECTOR3( 4.5f + 1.7f * FrmCos( 0.25f*fTime ), 
        2.0f + 1.1f * FrmSin( 0.15f*fTime ), 
        0.0f - 1.9f * FrmSin( 0.25f*fTime ) );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample21::DrawObject( SimpleObject21* Object )
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

    glUseProgram( m_PlasticShader.ShaderId );
    glUniformMatrix4fv( m_PlasticShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_PlasticShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_PlasticShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_PlasticShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_PlasticShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_PlasticShader.m_AmbientLightId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform4fv( m_PlasticShader.m_DiffuseColorId, 1, Object->DiffuseColor.v );
    glUniform4fv( m_PlasticShader.m_SpecularColorId, 1, Object->SpecularColor.v );
    glUniform1f( m_PlasticShader.m_ReflectionStrId, Object->ReflectionStrength );

    // note: the 2D samples start at index 1 when using a cube map.  It doesn't
    // matter if the cube sampler comes first or last in the shader.
    SetTexture( m_PlasticShader.m_DiffuseTextureId, Object->DiffuseTexture->m_hTextureHandle, 1 );
    SetTexture( m_PlasticShader.m_BumpTextureId, Object->BumpTexture->m_hTextureHandle, 2 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_CubemapHandle );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: DrawObjectBlinnPhong()
// Desc: Renders an object using the Blinn-Phong shading model
//--------------------------------------------------------------------------------------
VOID CSample21::DrawObjectBlinnPhong( SimpleObject21* Object )
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
VOID CSample21::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glClearColor( 0.07f, 0.07f, 0.07f, 1.0f );
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
