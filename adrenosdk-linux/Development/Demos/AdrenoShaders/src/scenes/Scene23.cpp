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
#include "Scene23.h"



#define MIN_PERLIN_VALUE (1.0f)
#define MAX_PERLIN_VALUE (20.0f)
#define DEFAULT_PERLIN_VALUE (2.0f)

static float ModelScale = 1.0f;

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MarbleShader::MarbleShader()
{
    ShaderId = 0;
    MarbleTexture = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MarbleShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
    if( MarbleTexture ) MarbleTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
PerlinNoiseShader::PerlinNoiseShader()
{
    ShaderId = 0;
    NoiseTexture = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID PerlinNoiseShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
    if( NoiseTexture ) NoiseTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject23::SimpleObject23()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 4.0f;
    Drawable = NULL;
    DiffuseTexture = NULL;
    BumpTexture = NULL;
    DiffuseColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f );
    SpecularColor = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 5.0f );
    ReflectionStrength = 0.1f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject23::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale, ModelScale, ModelScale );
    ModelMatrix = ScaleMat;

    FRMMATRIX4X4 RotateMat1 = FrmMatrixRotate( FRM_PI * 0.5f,  1.0f, 0.0f, 0.0f );
    FRMMATRIX4X4 RotateMat2 = FrmMatrixRotate( FRM_PI * 0.5f,  0.0f, 0.0f, 1.0f );
    FRMMATRIX4X4 RotateMat3 = FrmMatrixRotate( RotateTime, 0.0f, 1.0f, 0.0f );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat1 );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat2 );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, RotateMat3 );

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject23::Destroy()
{
    if( DiffuseTexture ) DiffuseTexture->Release();
    if( BumpTexture ) BumpTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: CSample23()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample23::CSample23( const CHAR* strName ) : CScene( strName )
{
    m_CubemapHandle = 0;
    m_Persistence = DEFAULT_PERLIN_VALUE;

    m_nWidth = 800;
    m_nHeight = 450;
}



//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample23::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/23_Textures.pak" ) )
        return FALSE;

    m_PerlinShader.NoiseTexture = resource.GetTexture( "Noise" );

    // Load the cube map for reflections
    if (!LoadCubeMap("23_Cubemap"))
        return FALSE;

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_Persistence, (char *)"Marble frequency" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, (char *)"Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Decrease marble frequency" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Increase marble frequency" );
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

    m_AmbientLight = FRMVECTOR4( 0.01f, 0.01f, 0.01f, 1.0f );

    // Set up the objects
    m_Object[0].DiffuseTexture = resource.GetTexture( "Marble1" );
    m_Object[0].BumpTexture = resource.GetTexture( "FlatBump" );
    m_Object[0].Drawable = &m_Mesh;
    m_Object[0].Position.x -= Split;
    m_Object[0].Position.y += yAdjust;
    m_Object[0].SpecularColor = FRMVECTOR4( 10.0f, 10.0f, 10.0f, 100.0f );
    m_Object[0].ReflectionStrength = 0.15f;

    m_Object[1].DiffuseTexture = resource.GetTexture( "Marble2" );
    m_Object[1].BumpTexture = resource.GetTexture( "FlatBump" );
    m_Object[1].Drawable = &m_Mesh;
    m_Object[1].Position.x += Split;
    m_Object[1].Position.y += yAdjust;
    m_Object[1].SpecularColor = FRMVECTOR4( 10.0f, 10.0f, 10.0f, 100.0f );
    m_Object[1].ReflectionStrength = 0.15f;

    InitPerlinNoise();

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitPerlinNoise()
// Desc: Creates the perlin noise texture
//--------------------------------------------------------------------------------------
VOID CSample23::InitPerlinNoise()
{
    UINT32 PositionId;
    UINT32 TexCoordId;

    struct PERLIN_VERTEX
    {
        FLOAT32 x, y;
        INT16   u, v;
    };


    m_PerlinNoiseRT.FreeRT();
    m_PerlinNoiseRT.CreateRT( 256, 256 );
    m_PerlinNoiseRT.SetFramebuffer();

    glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

    // Set the shader program
    glUseProgram( m_PerlinShader.ShaderId );

    SetTexture( m_PerlinShader.m_NoiseTextureId, m_PerlinShader.NoiseTexture->m_hTextureHandle, 1 );
    glUniform1f( m_PerlinShader.m_PersistenceId, m_Persistence );

    PositionId = glGetAttribLocation( m_PerlinShader.ShaderId, "g_PositionIn" );
    TexCoordId = glGetAttribLocation( m_PerlinShader.ShaderId, "g_TexCoordIn" );
    glEnableVertexAttribArray( PositionId );
    glEnableVertexAttribArray( TexCoordId );

    FLOAT32 w = 1.0f;
    FLOAT32 h = 1.0f;
    INT16 u0 = 0;
    INT16 u1 = 1;
    INT16 v0 = 0;
    INT16 v1 = 1;
    PERLIN_VERTEX Quad[4] =
    {
        { -w, -h, u0, v0 }, 
        { -w,  h, u0, v1 }, 
        {  w,  h, u1, v1 }, 
        {  w, -h, u1, v0 },
    };

    glVertexAttribPointer( PositionId, 2, GL_FLOAT, FALSE, sizeof(PERLIN_VERTEX), &Quad[0].x );
    glVertexAttribPointer( TexCoordId, 2, GL_SHORT, FALSE, sizeof(PERLIN_VERTEX), &Quad[0].u );
    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample23::InitShaders()
{
    // Create the marble shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 },
            { "g_NormalIn",   FRM_VERTEX_NORMAL },
            { "g_TangentIn",  FRM_VERTEX_TANGENT }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MarbleShader.vs", "Demos/AdrenoShaders/Shaders/MarbleShader.fs",
            &m_MarbleShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_MarbleShader.m_ModelMatrixId         = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_MatModel" );
        m_MarbleShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_MatModelViewProj" );
        m_MarbleShader.m_NormalMatrixId        = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_MatNormal" );
        m_MarbleShader.m_LightPositionId       = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_LightPos" );
        m_MarbleShader.m_EyePositionId         = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_EyePos" );
        m_MarbleShader.m_AmbientLightId        = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_AmbientLight" );
        m_MarbleShader.m_DiffuseColorId        = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_DiffuseColor" );
        m_MarbleShader.m_SpecularColorId       = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_SpecularColor" );
        m_MarbleShader.m_BumpTextureId         = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_BumpTexture" );
        m_MarbleShader.m_PerlinTextureId       = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_PerlinTexture" );
        m_MarbleShader.m_MarbleTextureId       = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_MarbleRamp" );
        m_MarbleShader.m_ReflectionStrId       = glGetUniformLocation( m_MarbleShader.ShaderId,  "g_ReflectionStrength" );
    }

    // Create the perlin noise shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PerlinNoise.vs", "Demos/AdrenoShaders/Shaders/PerlinNoise.fs",
            &m_PerlinShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_PerlinShader.m_NoiseTextureId     = glGetUniformLocation( m_PerlinShader.ShaderId, "g_NoiseTexture" );
        m_PerlinShader.m_PersistenceId      = glGetUniformLocation( m_PerlinShader.ShaderId, "g_Persistence" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample23::Resize()
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
VOID CSample23::Destroy()
{
    // Free shader memory
    m_MarbleShader.Destroy();
    m_PerlinShader.Destroy();

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
VOID CSample23::Update()
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

    if( nPressedButtons & INPUT_KEY_6 )
    {
        m_Persistence -= 1.0f;
        if( m_Persistence < MIN_PERLIN_VALUE )
        {
            m_Persistence = MAX_PERLIN_VALUE;
        }
        InitPerlinNoise();
    }

    if( nPressedButtons & INPUT_KEY_7 )
    {
        m_Persistence += 1.0f;
        if( m_Persistence > MAX_PERLIN_VALUE )
        {
            m_Persistence = MIN_PERLIN_VALUE;
        }
        InitPerlinNoise();
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
// Name: GetCameraPos23()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos23( FLOAT32 fTime )
{
    return FRMVECTOR3( 4.5f + 1.7f * FrmCos( 0.25f*fTime ), 
        2.0f + 1.1f * FrmSin( 0.15f*fTime ), 
        0.0f - 1.9f * FrmSin( 0.25f*fTime ) );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample23::DrawObject( SimpleObject23* Object )
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

    glUseProgram( m_MarbleShader.ShaderId );
    glUniformMatrix4fv( m_MarbleShader.m_ModelMatrixId, 1, FALSE, (FLOAT32*)&MatModel );
    glUniformMatrix4fv( m_MarbleShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&MatModelViewProj );
    glUniformMatrix4fv( m_MarbleShader.m_NormalMatrixId, 1, FALSE, (FLOAT32*)&MatNormal );
    glUniform3fv( m_MarbleShader.m_EyePositionId, 1, (FLOAT32*)&Eye.v );
    glUniform3fv( m_MarbleShader.m_LightPositionId, 1, (FLOAT32*)&m_LightPos.v );
    glUniform4fv( m_MarbleShader.m_AmbientLightId, 1, (FLOAT32*)&m_AmbientLight.v );
    glUniform4fv( m_MarbleShader.m_DiffuseColorId, 1, Object->DiffuseColor.v );
    glUniform4fv( m_MarbleShader.m_SpecularColorId, 1, Object->SpecularColor.v );
    glUniform1f( m_MarbleShader.m_ReflectionStrId, Object->ReflectionStrength );

    // note: the 2D samples start at index 1 when using a cube map.  It doesn't
    // matter if the cube sampler comes first or last in the shader.
    SetTexture( m_MarbleShader.m_MarbleTextureId, Object->DiffuseTexture->m_hTextureHandle, 1 );
    SetTexture( m_MarbleShader.m_PerlinTextureId, m_PerlinNoiseRT.TextureHandle, 2 );
    SetTexture( m_MarbleShader.m_BumpTextureId, Object->BumpTexture->m_hTextureHandle, 3 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_CubemapHandle );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample23::Render()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    glClearColor( 0.07f, 0.07f, 0.07f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );

    // Draw some objects
    for( int i = 0; i < 2; i++ )
        DrawObject( &m_Object[i] );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}
