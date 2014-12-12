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
#include "Scene10.h"


static float ModelScale = 1.0f;


OrenNayarShader::OrenNayarShader()
{
    ShaderId = 0;
    SinTanTexture = NULL;
}


VOID OrenNayarShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
    if( SinTanTexture ) SinTanTexture->Release();
}

VOID BlinnPhongShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


BlinnPhongShader::BlinnPhongShader()
{
    ShaderId = 0;
}


SimpleObject::SimpleObject()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 2.0f;
    Drawable = NULL;
    DiffuseTexture = NULL;
    BumpTexture = NULL;
    DiffuseColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
    SpecularColor = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f );
}


VOID SimpleObject::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.1f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    // rotate the object
    FRMMATRIX4X4 RotateMat1 = FrmMatrixRotate( RotateTime, 0.0f, 1.0f, 0.0f );
    FRMMATRIX4X4 RotateMat2 = FrmMatrixRotate( 0.2f,  1.0f, 0.0f, 0.0f );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat1 );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat2 );

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


VOID SimpleObject::Destroy()
{
    if( DiffuseTexture ) DiffuseTexture->Release();
    if( BumpTexture ) BumpTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: CSample10()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample10::CSample10( const CHAR* strName ) : CScene( strName )
{
    m_nWidth = 800;
    m_nHeight = 500;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample10::InitShaders()
{
    // Create the Oren-Nayar shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
            { "g_TangentIn",  FRM_VERTEX_TANGENT }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/OrenNayarShader.vs", "Demos/AdrenoShaders/Shaders/OrenNayarShader.fs",
            &m_OrenNayarShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_OrenNayarShader.m_ModelMatrixId         = glGetUniformLocation( m_OrenNayarShader.ShaderId,  "g_MatModel" );
        m_OrenNayarShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_OrenNayarShader.ShaderId,  "g_MatModelViewProj" );
        m_OrenNayarShader.m_NormalMatrixId        = glGetUniformLocation( m_OrenNayarShader.ShaderId,  "g_MatNormal" );
        m_OrenNayarShader.m_LightPositionId       = glGetUniformLocation( m_OrenNayarShader.ShaderId,  "g_LightPos" );
        m_OrenNayarShader.m_EyePositionId         = glGetUniformLocation( m_OrenNayarShader.ShaderId,  "g_EyePos" );
        m_OrenNayarShader.m_AmbientLightId        = glGetUniformLocation( m_OrenNayarShader.ShaderId,  "g_AmbientLight" );
        m_OrenNayarShader.m_DiffuseColorId        = glGetUniformLocation( m_OrenNayarShader.ShaderId,  "g_DiffuseColor" );
        m_OrenNayarShader.m_SpecularColorId       = glGetUniformLocation( m_OrenNayarShader.ShaderId,  "g_SpecularColor" );
        m_OrenNayarShader.m_BumpTextureId         = glGetUniformLocation( m_OrenNayarShader.ShaderId,  "g_BumpTexture" );
        m_OrenNayarShader.m_DiffuseTextureId      = glGetUniformLocation( m_OrenNayarShader.ShaderId,  "g_DiffuseTexture" );
        m_OrenNayarShader.m_SinTanTextureId       = glGetUniformLocation( m_OrenNayarShader.ShaderId,  "g_SinTanTexture" );
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BlinnPhongShader10.vs", "Demos/AdrenoShaders/Shaders/BlinnPhongShader10.fs",
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
BOOL CSample10::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/10_Textures.pak" ) )
        return FALSE;

    // Used for Oren-Nayar calculations
    m_OrenNayarShader.SinTanTexture = resource.GetTexture( "SinTan" );

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
    m_CameraPos = FRMVECTOR3( 0.0f, 0.20f, 60.5f );
    float Split = 1.2f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
    {
        return FALSE;
    }

    m_AmbientLight = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );

    // Set up the objects
    m_Moon1.DiffuseTexture = resource.GetTexture( "MoonMap" );
    m_Moon1.BumpTexture = resource.GetTexture( "MoonBumpMap" );
    m_Moon1.Drawable = &m_Mesh;
    m_Moon1.Position.x -= Split;

    m_Moon2.DiffuseTexture = resource.GetTexture( "MoonMap" );
    m_Moon2.BumpTexture = resource.GetTexture( "MoonBumpMap" );
    m_Moon2.Drawable = &m_Mesh;
    m_Moon2.Position.x += Split;

    glEnable( GL_DEPTH_TEST );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample10::Resize()
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
VOID CSample10::Destroy()
{
    // Free shader memory
    m_OrenNayarShader.Destroy();
    m_BlinnPhongShader.Destroy();

    // Free objects
    m_Moon1.Destroy();
    m_Moon2.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample10::Update()
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
    static FLOAT32 LightOrbitalTime = -3.0f;
    static FRMVECTOR3 LightOrigin = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    if( m_ShouldRotate )
    {
        LightOrbitalTime += ( ElapsedTime * 0.6f );
    }

    m_LightPos = LightOrigin;
    m_LightPos.z += ( cos( LightOrbitalTime ) * 50.0f );
    m_LightPos.x += ( sin( LightOrbitalTime ) * 50.0f );


    // build camera transforms
    FRMVECTOR3 LookDir   = FRMVECTOR3( 0.0f, 0.0f, -1.0f );
    FRMVECTOR3 UpDir     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 Temp      = m_CameraPos + LookDir;
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, Temp, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );


    // update objects
    //     m_Moon1.Update( ElapsedTime, m_ShouldRotate );
    //     m_Moon2.Update( ElapsedTime, m_ShouldRotate );
    float MoonTime = 0.0f;
    m_Moon1.Update( MoonTime, m_ShouldRotate );
    m_Moon2.Update( MoonTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos( FLOAT32 fTime )
{
    return FRMVECTOR3( 4.5f + 1.7f * FrmCos( 0.25f*fTime ), 
        2.0f + 1.1f * FrmSin( 0.15f*fTime ), 
        0.0f - 1.9f * FrmSin( 0.25f*fTime ) );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample10::DrawObject( SimpleObject* Object )
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

    glUseProgram( m_OrenNayarShader.ShaderId );
    glUniformMatrix4fv( m_OrenNayarShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_OrenNayarShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_OrenNayarShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_OrenNayarShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_OrenNayarShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_OrenNayarShader.m_AmbientLightId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform4fv( m_OrenNayarShader.m_DiffuseColorId, 1, Object->DiffuseColor.v );
    glUniform4fv( m_OrenNayarShader.m_SpecularColorId, 1, Object->SpecularColor.v );
    SetTexture( m_OrenNayarShader.m_DiffuseTextureId, Object->DiffuseTexture->m_hTextureHandle, 0 );
    SetTexture( m_OrenNayarShader.m_BumpTextureId, Object->BumpTexture->m_hTextureHandle, 1 );
    SetTexture( m_OrenNayarShader.m_SinTanTextureId, m_OrenNayarShader.SinTanTexture->m_hTextureHandle, 2 );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: DrawObjectBlinnPhong()
// Desc: Renders an object using the Blinn-Phong shading model
//--------------------------------------------------------------------------------------
VOID CSample10::DrawObjectBlinnPhong( SimpleObject* Object )
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
VOID CSample10::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Draw some objects
    DrawObjectBlinnPhong( &m_Moon1 );
    DrawObject( &m_Moon2 );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
