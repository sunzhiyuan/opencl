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
#include "Scene19.h"


static float    ModelScale = 1.0f;


//--------------------------------------------------------------------------------------
// Name: CSample19()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample19::CSample19( const CHAR* strName ) : CScene( strName )
{
    m_hPerPixelLightingShader =	NULL;

    m_fRotateTime = 0.0f;

    m_nWidth          = 500;
    m_nHeight         = 600;

    m_CameraPos = FRMVECTOR3( 0.0f, 0.20f, 0.6f );


    // default tuning:
    //     g_Roughness = 0.2;
    //     g_Reflection = 0.01;
    //     g_DiffuseIntensity = 0.7;
    //     g_SpecularIntensity = 0.3;

    m_DiffuseIntensity = 0.55f;
    m_Roughness = 0.35f;
    m_FresnelReflection = 0.43f;
    m_SpecularIntensity = 1.0f;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample19::InitShaders()
{
    // Create the PerPixelLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
            { "g_TangentIn",  FRM_VERTEX_TANGENT }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MetalShader.vs",
                                                      "Demos/AdrenoShaders/Shaders/MetalShader.fs",
                                                      &m_hPerPixelLightingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_ModelMatrixId          = glGetUniformLocation( m_hPerPixelLightingShader,  "g_MatModel" );
        m_ModelViewProjMatrixId  = glGetUniformLocation( m_hPerPixelLightingShader,  "g_MatModelViewProj" );
        m_NormalMatrixId         = glGetUniformLocation( m_hPerPixelLightingShader,  "g_MatNormal" );
        m_LightPositionId        = glGetUniformLocation( m_hPerPixelLightingShader,  "g_LightPos" );
        m_EyePositionId          = glGetUniformLocation( m_hPerPixelLightingShader,  "g_EyePos" );
        m_RoughnessId            = glGetUniformLocation( m_hPerPixelLightingShader,  "g_Roughness" );
        m_ReflectionId           = glGetUniformLocation( m_hPerPixelLightingShader,  "g_Reflection" );
        m_DiffuseId              = glGetUniformLocation( m_hPerPixelLightingShader,  "g_DiffuseIntensity" );
        m_SpecularId             = glGetUniformLocation( m_hPerPixelLightingShader,  "g_SpecularIntensity" );
        m_DiffuseTextureId       = glGetUniformLocation( m_hPerPixelLightingShader,  "g_DiffuseTexture" );
        m_BumpTextureId          = glGetUniformLocation( m_hPerPixelLightingShader,  "g_BumpTexture" );
        m_UVPullId               = glGetUniformLocation( m_hPerPixelLightingShader,  "g_UVPull" );
    }

	return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample19::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_bShouldRotate = TRUE;
//    m_fDeformSize   = 0.0030f;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/19_Textures.pak" ) )
        return FALSE;

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_bShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_DiffuseIntensity, (char *)"Diffuse Intensity" );
    m_UserInterface.AddFloatVariable( &m_Roughness, (char *)"Roughness" );
    m_UserInterface.AddFloatVariable( &m_FresnelReflection, (char *)"Fresnel Index" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, (char *)"Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Decrease diffuse color" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Increase diffuse color" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Decrease roughness" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Increase roughness" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease fresnel index" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase fresnel index" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );


    m_DiffuseTexture[0] = resource.GetTexture( "Metal1" );
    m_DiffuseTexture[1] = resource.GetTexture( "Metal2" );
    m_BumpTexture[0] = resource.GetTexture( "MetalBump1" );
    m_BumpTexture[1] = resource.GetTexture( "MetalBump2" );


    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Trophy2.mesh" ) )
        return FALSE;

    ModelScale = 1.0f;


    if( FALSE == m_Mesh.MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
    
    glEnable( GL_DEPTH_TEST );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample19::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 0.1f, 10.0f );

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
VOID CSample19::Destroy()
{
    // Free shader memory
    if( m_hPerPixelLightingShader ) glDeleteProgram( m_hPerPixelLightingShader );

	// Free texture memory
    for( int i = 0; i < 2; i++ )
    {
        if( m_DiffuseTexture[i] ) m_DiffuseTexture[i]->Release();
        if( m_BumpTexture[i] ) m_BumpTexture[i]->Release();
    }

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample19::Update()
{
    // Get the current time
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();
    if( m_bShouldRotate )
    {
        m_fRotateTime += fElapsedTime * 0.75f;
    }


    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();



    if( nButtons & INPUT_KEY_2 )
    {
        m_DiffuseIntensity -= 1.0f * fElapsedTime;
        m_DiffuseIntensity = max( m_DiffuseIntensity, 0.1f );
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_DiffuseIntensity += 1.0f * fElapsedTime;
        m_DiffuseIntensity = min( m_DiffuseIntensity, 1.0f );
    }

    if( nButtons & INPUT_KEY_4 )
    {
        m_Roughness -= 1.0f * fElapsedTime;
        m_Roughness = max( m_Roughness, 0.1f );
    }

    if( nButtons & INPUT_KEY_5 )
    {
        m_Roughness += 1.0f * fElapsedTime;
        m_Roughness = min( m_Roughness, 1.0f );
    }

    if( nButtons & INPUT_KEY_6 )
    {
        m_FresnelReflection -= 1.0f * fElapsedTime;
        m_FresnelReflection = max( m_FresnelReflection, 0.1f );
    }

    if( nButtons & INPUT_KEY_7 )
    {
        m_FresnelReflection += 1.0f * fElapsedTime;
        m_FresnelReflection = min( m_FresnelReflection, 1.0f );
    }


    // Toggle rotation
    if( nPressedButtons & INPUT_KEY_8 )
    {
        m_bShouldRotate = 1 - m_bShouldRotate;
    }
}


//--------------------------------------------------------------------------------------
// Name: BuildMatrices()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample19::BuildMatrices( const FRMVECTOR3& Position )
{
    // Animate the object
    FRMMATRIX4X4 matCenter = FrmMatrixTranslate( 0.0f, 0.0f, 0.0f );
    FRMMATRIX4X4 matPosition = FrmMatrixTranslate( Position.x, Position.y, Position.z );
    FRMMATRIX4X4 matRotate1 = FrmMatrixRotate( -0.4f, 1.0f, 0.0f, 0.0f );
    FRMMATRIX4X4 matRotate2 = FrmMatrixRotate( m_fRotateTime,  0.0f, 1.0f, 0.0f );

    FRMVECTOR3 LookDir   = FRMVECTOR3( 0.0f, 0.0f, -1.0f );
    FRMVECTOR3 UpDir     = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 Temp      = m_CameraPos + LookDir;
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, Temp, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // Build the matrices
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    m_matModel = ScaleMat;
    m_matModel = FrmMatrixMultiply( m_matModel, matCenter );
    m_matModel = FrmMatrixMultiply( m_matModel, matRotate2 );
    m_matModel = FrmMatrixMultiply( m_matModel, matRotate1 );
    m_matModel = FrmMatrixMultiply( m_matModel, matPosition );

    m_matModelView     = FrmMatrixMultiply( m_matModel, m_matView );
    m_matModelViewProj = FrmMatrixMultiply( m_matModelView, m_matProj );
    m_matNormal        = FrmMatrixInverse( m_matModel );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample19::Render()
{
    FRMVECTOR3 Eye = m_CameraPos;
    static FRMVECTOR3 LightOrigin = FRMVECTOR3( 0.0f, 10.0f, 10.0f );
    m_LightPos = LightOrigin;

    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    glUseProgram( m_hPerPixelLightingShader );
    BuildMatrices( FRMVECTOR3( 0.0f, 0.05f, 0.22f ) );
    glUniform1f( m_RoughnessId, m_Roughness );
    glUniform1f( m_ReflectionId, m_FresnelReflection );
    glUniform1f( m_DiffuseId, m_DiffuseIntensity );
    glUniform1f( m_SpecularId, m_SpecularIntensity );
    glUniform1f( m_UVPullId, 6.0f );

    SetTexture( m_DiffuseTextureId, m_DiffuseTexture[0]->m_hTextureHandle, 0 );
    SetTexture( m_BumpTextureId, m_BumpTexture[0]->m_hTextureHandle, 1 );

    glUniformMatrix4fv( m_ModelMatrixId, 1, FALSE, (FLOAT32*)&m_matModel );
    glUniformMatrix4fv( m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniformMatrix4fv( m_NormalMatrixId, 1, FALSE, (FLOAT32*)&m_matNormal );
    glUniform3fv( m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform3fv( m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    m_Mesh.Render();


//     for( int i = 0; i < 2; i++ )
//     {
//         glUseProgram( m_hPerPixelLightingShader );
// 
//         switch( i )
//         {
//         case 0:
//             BuildMatrices( FRMVECTOR3( -0.11f, 0.04f, 0.0f ) );
//             glUniform1f( m_RoughnessId, m_Roughness );
//             glUniform1f( m_ReflectionId, m_FresnelReflection );
//             glUniform1f( m_DiffuseId, m_DiffuseIntensity );
//             glUniform1f( m_SpecularId, m_SpecularIntensity );
//             glUniform1f( m_UVPullId, 6.0f );
//             break;
// 
//         case 1:
//         default:
//             BuildMatrices( FRMVECTOR3( 0.11f, 0.04f, 0.0f ) );
//             glUniform1f( m_RoughnessId, 0.2f );
//             glUniform1f( m_ReflectionId, 0.7f );
//             glUniform1f( m_DiffuseId, 0.2f );
//             glUniform1f( m_SpecularId, m_SpecularIntensity );
//             glUniform1f( m_UVPullId, 2.0f );
//             break;
//         }
// 
//         SetTexture( m_DiffuseTextureId, m_DiffuseTexture[i]->m_hTextureHandle, 0 );
//         SetTexture( m_BumpTextureId, m_BumpTexture[i]->m_hTextureHandle, 1 );
// 
//         glUniformMatrix4fv( m_ModelMatrixId, 1, FALSE, (FLOAT32*)&m_matModel );
//         glUniformMatrix4fv( m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
//         glUniformMatrix4fv( m_NormalMatrixId, 1, FALSE, (FLOAT32*)&m_matNormal );
//         glUniform3fv( m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
//         glUniform3fv( m_EyePositionId, 1, (FLOAT32*)&Eye.v );
//         m_Mesh.Render();
//     }

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

