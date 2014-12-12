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
#include "Scene45.h"


//--------------------------------------------------------------------------------------
// Name: GaussianDistribution45()
// Desc: 
//--------------------------------------------------------------------------------------
FLOAT32 GaussianDistribution45( FLOAT32 X, FLOAT32 Y, FLOAT32 Rho )
{
    FLOAT32 Gauss = 1.0f / sqrtf( 2.0f * FRM_PI * Rho * Rho );
    Gauss *= expf( -( X * X + Y * Y ) / ( 2.0f * Rho * Rho ) );

    return Gauss;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader45::MaterialShader45()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader45::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
NormalShader45::NormalShader45()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID NormalShader45::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
PreviewShader45::PreviewShader45()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID PreviewShader45::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BounceShader45::BounceShader45()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BounceShader45::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BlurShader45::BlurShader45()
{
    ShaderId = 0;
    StepSize = FRMVECTOR2( 1.0f, 1.0f );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BlurShader45::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CombineShader45::CombineShader45()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CombineShader45::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject45::SimpleObject45()
{
    ModelScale = 1.0f;
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
    BumpTexture = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject45::Update( FLOAT ElapsedTime, FRMMATRIX4X4& ViewMat, FRMMATRIX4X4& ProjMat, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );

    // Build the matrices
    ModelView     = FrmMatrixMultiply( ModelMatrix, ViewMat );
    ModelViewProj = FrmMatrixMultiply( ModelView, ProjMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject45::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample45()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample45::CSample45( const CHAR* strName ) : CScene( strName )
{
    m_JitterTexture = NULL;
    m_ViewPositionScalar = 10.0f;
    m_GaussSpread = 1.05f;
    m_GaussRho = 1.0f;
    m_AOIntensity = 4.58f;
    m_AOEdgeFinder = 50.0f;
    m_JitterAmount = 2.0f;
    m_BounceIntensity = 1.0f;

    m_nWidth = 800;
    m_nHeight = 600;
}


//--------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample45::CheckFrameBufferStatus()
{
    switch( glCheckFramebufferStatus(GL_FRAMEBUFFER) )
    {
        case GL_FRAMEBUFFER_COMPLETE:
            return TRUE;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            return FALSE;
    }
    FrmAssert(0);
    return FALSE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample45::Initialize(CFrmFontGLES &Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;
    m_PreviewRT = FALSE;

    // Create the font
    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/45_Textures.pak" ) )
        return FALSE;


    // A jitter texture for AO
    m_JitterTexture = resource.GetTexture( "Jitter" );


    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5, m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_BounceIntensity, (char *)"Bounce Intensity" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Bounce intensity -" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Bounce intensity +" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Map3.mesh" ) )
        return FALSE;
    m_CameraPos = FRMVECTOR3( 6.2f, 2.0f, 0.0f );
    float Split = 1.1f;
    float yAdjust = -0.05f;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    // Set up the objects
    m_Object.Drawable = &m_Mesh;
    m_Object.Position.y += yAdjust;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: SetupGaussWeights()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample45::SetupGaussWeights()
{
    FLOAT32 GaussSum = 0.0f;
    for( int i = 0; i < 4; i++ )
    {
        // normalize to -1..1
        FLOAT32 X = ( FLOAT32( i ) / 3.0f );
        X = ( X - 0.5f ) * 2.0f;

        // spread is tunable
        X *= m_GaussSpread;

        m_BlurShader.GaussWeight[i] = GaussianDistribution45( X, 0.0f, m_GaussRho );

        GaussSum += m_BlurShader.GaussWeight[i];
    }

    m_BlurShader.GaussInvSum = ( 1.0f / GaussSum );
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample45::InitShaders()
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MaterialShader45.vs", "Demos/AdrenoShaders/Shaders/MaterialShader45.fs",
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
    }

    // Normals shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_NormalIn",   FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/NormalShader45.vs", "Demos/AdrenoShaders/Shaders/NormalShader45.fs",
            &m_NormalShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_NormalShader.ModelViewProjMatrixId = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matModelViewProj" );
        m_NormalShader.ModelViewMatrixId     = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matModelView" );
    }

    // One-bounce shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BounceShader45.vs", "Demos/AdrenoShaders/Shaders/BounceShader45.fs",
            &m_BounceShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_BounceShader.NormalTextureId  = glGetUniformLocation( m_BounceShader.ShaderId, "g_NormalTexture" );
        m_BounceShader.DepthTextureId   = glGetUniformLocation( m_BounceShader.ShaderId, "g_DepthTexture" );
        m_BounceShader.JitterTextureId  = glGetUniformLocation( m_BounceShader.ShaderId, "g_JitterTexture" );
        m_BounceShader.ColorTextureId   = glGetUniformLocation( m_BounceShader.ShaderId, "g_ColorTexture" );
        m_BounceShader.ProjInvId        = glGetUniformLocation( m_BounceShader.ShaderId, "g_ProjInv" );
        m_BounceShader.StepSizeId       = glGetUniformLocation( m_BounceShader.ShaderId, "g_StepSize" );
        m_BounceShader.AOIntensityId    = glGetUniformLocation( m_BounceShader.ShaderId, "g_AOIntensity" );
        m_BounceShader.AOEdgeId         = glGetUniformLocation( m_BounceShader.ShaderId, "g_AOEdgeFinder" );
        m_BounceShader.JitterAmountId   = glGetUniformLocation( m_BounceShader.ShaderId, "g_JitterAmount" );
        m_BounceShader.BounceIntensityId = glGetUniformLocation( m_BounceShader.ShaderId, "g_BounceIntensity" );
    }

    // Blur shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BlurShader45.vs", "Demos/AdrenoShaders/Shaders/BlurShader45.fs",
            &m_BlurShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_BlurShader.ColorTextureId = glGetUniformLocation( m_BlurShader.ShaderId, "g_ColorTexture" );
        m_BlurShader.GaussWeightId = glGetUniformLocation( m_BlurShader.ShaderId, "g_GaussWeight" );
        m_BlurShader.GaussInvSumId = glGetUniformLocation( m_BlurShader.ShaderId, "g_GaussInvSum" );
        m_BlurShader.StepSizeId = glGetUniformLocation( m_BlurShader.ShaderId, "g_StepSize" );

        SetupGaussWeights();
    }

    // Combine shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CombineShader45.vs", "Demos/AdrenoShaders/Shaders/CombineShader45.fs",
            &m_CombineShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CombineShader.AOTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_AOTexture" );
        m_CombineShader.ColorTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_ColorTexture" );
   }

    // Preview shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PreviewShader45.vs", "Demos/AdrenoShaders/Shaders/PreviewShader45.fs",
            &m_PreviewShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_PreviewShader.ColorTextureId = glGetUniformLocation( m_PreviewShader.ShaderId, "g_ColorTexture" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample45::Resize()
{
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    FLOAT32 FOVy = FrmRadians( 86 );
    FLOAT32 zNear = 0.25f;
    FLOAT32 zFar = 20.0f;
    m_matProj = FrmMatrixPerspectiveFovRH( FOVy, fAspect, zNear, zFar );

    INT32 AOWidth = m_nWidth / 4;
    INT32 AOHeight = m_nHeight / 4;
    m_BounceShader.ProjInv = FrmMatrixInverse( m_matProj );
    m_BounceShader.StepSize = FRMVECTOR2( 1.0f / FLOAT32( AOWidth ), 1.0f / FLOAT32( AOHeight ) );
    m_BlurShader.StepSize = m_BounceShader.StepSize;


    // To keep sizes consistent, scale the projection matrix in landscape orientation
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    m_ColorRT.CreateRT( m_nWidth, m_nHeight, TRUE );
    m_NormalRT.CreateRT( AOWidth, AOHeight, TRUE );
    m_AORT.CreateRT( AOWidth, AOHeight, FALSE );
    m_ScratchRT.CreateRT( AOWidth, AOHeight, FALSE );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample45::Destroy()
{
    // Free shader memory
    m_MaterialShader.Destroy();
    m_NormalShader.Destroy();
    m_BounceShader.Destroy();
    m_PreviewShader.Destroy();
    m_CombineShader.Destroy();
    m_BlurShader.Destroy();


    // Free texture memory
    if( m_JitterTexture ) m_JitterTexture->Release();

    // Free objects
    m_Object.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: GaussBlur()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample45::GaussBlur( OffscreenRT4& TargetRT )
{
    // Blur in the X direction
    {
        m_ScratchRT.SetFramebuffer();

        glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR2 StepSizeX = FRMVECTOR2( m_BlurShader.StepSize.x * m_GaussSpread, 0.0f );
        glUseProgram( m_BlurShader.ShaderId );
        glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
        glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
        glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeX.v );
        SetTexture( m_BlurShader.ColorTextureId, TargetRT.TextureHandle, 0 );

        RenderScreenAlignedQuad();

        m_ScratchRT.DetachFramebuffer();
    }

    // Then blur in the Y direction
    {
        TargetRT.SetFramebuffer();

        glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR2 StepSizeY = FRMVECTOR2( 0.0f, m_BlurShader.StepSize.y * m_GaussSpread );
        glUseProgram( m_BlurShader.ShaderId );
        glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
        glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
        glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeY.v );
        SetTexture( m_BlurShader.ColorTextureId, m_ScratchRT.TextureHandle, 0 );

        RenderScreenAlignedQuad();

        TargetRT.DetachFramebuffer();
    }
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample45::Update()
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


    if( nButtons & INPUT_KEY_2 )
    {
        m_BounceIntensity -= 5.0f * ElapsedTime;
        m_BounceIntensity = max( 0.25f, m_BounceIntensity );
    }

    if( nButtons & INPUT_KEY_3 )
    {
        m_BounceIntensity += 5.0f * ElapsedTime;
        m_BounceIntensity = min( 5.0f, m_BounceIntensity );
    }

    // toggle rotation
    if( nPressedButtons & INPUT_KEY_8 )
    {
        m_ShouldRotate = 1 - m_ShouldRotate;
    }

    // toggle RT preview
    if( nPressedButtons & INPUT_KEY_9 )
    {
        m_PreviewRT = 1 - m_PreviewRT;
    }


    // animate the camera
    static FLOAT32 TotalTime = 0.0f;
    if( m_ShouldRotate )
    {
        static FLOAT32 CameraSpeed = 1.0f;
        TotalTime += ElapsedTime * CameraSpeed;
    }

    FRMVECTOR3 UpDir = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    m_CameraPos = FRMVECTOR3( 1.5f, 3.35f, -0.9f );
    FRMVECTOR3 LookPos = m_CameraPos + FRMVECTOR3( 0.0f, 0.3f, -1.0f );

    // update light position
    m_LightPos = m_CameraPos + FRMVECTOR3( 0.0f, 0.3f, -1.0f );


    // Move cam around a little
    LookPos.x += FrmCos( TotalTime * 0.5f ) * 0.2f;


    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // update object
    m_Object.Update( ElapsedTime, m_matView, m_matProj, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample45::DrawObject( SimpleObject45* Object )
{
    FRMMATRIX4X4    MatModel;
    FRMMATRIX4X4    MatModelView;
    FRMMATRIX4X4    MatModelViewProj;
    FRMMATRIX4X4    MatNormal;
    FRMVECTOR3      Eye = m_CameraPos;

    if ( !Object || !Object->Drawable )
        return;


    FRMVECTOR4 AmbientLight = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 0.0f );


    MatModel         = Object->ModelMatrix;
    MatModelView     = FrmMatrixMultiply( MatModel, m_matView );
    MatModelViewProj = FrmMatrixMultiply( MatModelView, m_matProj );
    MatNormal        = FrmMatrixInverse( MatModel );

    glUseProgram( m_MaterialShader.ShaderId );
    glUniformMatrix4fv( m_MaterialShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_MaterialShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniform3fv( m_MaterialShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_MaterialShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_MaterialShader.m_AmbientColorId, 1, (FLOAT32*)&AmbientLight.v );
    glUniform1i( m_MaterialShader.m_DiffuseTextureId, 0 );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: DrawNormalsAndDepth()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample45::DrawNormalsAndDepth()
{
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );
    glFrontFace( GL_CW );
    glDepthFunc( GL_LESS );
    glDepthMask( TRUE );

    m_NormalRT.SetFramebuffer();

    // Clear color, depth
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClearDepthf( 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set shader variables
    glUseProgram( m_NormalShader.ShaderId );
    glUniformMatrix4fv( m_NormalShader.ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&m_Object.ModelViewProj );
    glUniformMatrix4fv( m_NormalShader.ModelViewMatrixId, 1, FALSE, (FLOAT32*)&m_Object.ModelView );

    m_Object.Drawable->Render();

    m_NormalRT.DetachFramebuffer();
    glDepthMask( FALSE );
}


//--------------------------------------------------------------------------------------
// Name: DrawColor()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample45::DrawColor()
{
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glDisable( GL_BLEND );
    glFrontFace( GL_CW );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
    glDepthMask( TRUE );

    m_ColorRT.SetFramebuffer();

    // Clear color, depth
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClearDepthf( 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    DrawObject( &m_Object );

    m_ColorRT.DetachFramebuffer();
    glDepthMask( FALSE );
}


//--------------------------------------------------------------------------------------
// Name: DrawCombinedScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample45::DrawAO()
{
    // Part 1: create AO
    {
        m_AORT.SetFramebuffer();

        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        glUseProgram( m_BounceShader.ShaderId );
        SetTexture( m_BounceShader.NormalTextureId, m_NormalRT.TextureHandle, 0 );
        SetTexture( m_BounceShader.DepthTextureId, m_NormalRT.DepthTextureHandle, 1 );
        SetTexture( m_BounceShader.JitterTextureId, m_JitterTexture->m_hTextureHandle, 2 );
        SetTexture( m_BounceShader.ColorTextureId, m_ColorRT.TextureHandle, 3 );
        glUniformMatrix4fv( m_BounceShader.ProjInvId, 1, FALSE, (FLOAT32*)&m_BounceShader.ProjInv );
        glUniform2fv( m_BounceShader.StepSizeId, 1, (FLOAT32*)&m_BounceShader.StepSize.v );
        glUniform1f( m_BounceShader.AOIntensityId, m_AOIntensity );
        glUniform1f( m_BounceShader.AOEdgeId, m_AOEdgeFinder );
        glUniform1f( m_BounceShader.JitterAmountId, m_JitterAmount * 0.1f );
        glUniform1f( m_BounceShader.BounceIntensityId, m_BounceIntensity );

        RenderScreenAlignedQuad();

        m_AORT.DetachFramebuffer();
    }

    // Part 2: blur
    GaussBlur( m_AORT );
    GaussBlur( m_AORT );
}


//--------------------------------------------------------------------------------------
// Name: DrawCombinedScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample45::DrawCombinedScene()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glDisable( GL_CULL_FACE );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );

    glUseProgram( m_CombineShader.ShaderId );
    SetTexture( m_CombineShader.AOTextureId, m_AORT.TextureHandle, 0 );
    SetTexture( m_CombineShader.ColorTextureId, m_ColorRT.TextureHandle, 3 );
    

    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: PreviewRT()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample45::PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle )
{
    // There is currently a bug in Qualcomm's shader compiler in that it can't
    // negate a value.  Therefore, change the shader and the negate y value here.
    TopLeft.y = TopLeft.y + Dims.y;
    Dims.y = -Dims.y;

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );

    glDisable( GL_CULL_FACE );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );

    glUseProgram( m_PreviewShader.ShaderId );
    SetTexture( m_PreviewShader.ColorTextureId, TextureHandle, 0 );

    RenderScreenAlignedQuad( TopLeft, TopLeft + Dims );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample45::Render()
{
    // Fill up color RT
    DrawColor();

    // Fill up normal RT and depth
    DrawNormalsAndDepth();

    // Calculate AO + Bounce lighting
    DrawAO();

    // Put everything together
    DrawCombinedScene();


    // Preview RTs
    if( m_PreviewRT )
    {
        FLOAT32 xPos = -0.975f;
        FLOAT32 yPos = -0.95f;
        FLOAT32 Width = 0.3f;
        FLOAT32 Height = 0.3f;
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_ColorRT.TextureHandle );
        xPos += ( Width + 0.025f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_NormalRT.TextureHandle );
        xPos += ( Width + 0.025f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_NormalRT.DepthTextureHandle );
        xPos += ( Width + 0.025f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_AORT.TextureHandle );
    }

    // Put the state back
    glDepthMask( TRUE );
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glFrontFace( GL_CCW );
    glEnable( GL_BLEND );


    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
