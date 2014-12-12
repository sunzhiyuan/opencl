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
#include "Scene31.h"


#define DEFAULT_OCCLUDER_THRESHOLD  ( 3.93f )
#define DEFAULT_OCCLUDER_FADEOUT    ( 3.83f )
#define DEFAULT_RAY_LENGTH          ( 0.17f )
#define DEFAULT_RAY_OPACITY         ( 1.0f )

float ModelScale31 = 1.0f;


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader31::MaterialShader31()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader31::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
PreviewShader31::PreviewShader31()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID PreviewShader31::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CutoutShader::CutoutShader()
{
    ShaderId = 0;
    OccluderThreshold = DEFAULT_OCCLUDER_THRESHOLD;
    OccluderFadeout = DEFAULT_OCCLUDER_FADEOUT;
    //StepSize = FRMVECTOR2( 1.0f, 1.0f );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CutoutShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
DownsizeShader31::DownsizeShader31()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID DownsizeShader31::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
RayExtruderShader::RayExtruderShader()
{
    ShaderId = 0;
    RayLength = DEFAULT_RAY_LENGTH;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID RayExtruderShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CombineShader31::CombineShader31()
{
    ShaderId = 0;
    RayOpacity = DEFAULT_RAY_OPACITY;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CombineShader31::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
BlurShader31::BlurShader31()
{
    ShaderId = 0;
    StepSize = FRMVECTOR2( 1.0f, 1.0f );
    GaussSpread = 1.0f;
    GaussRho = 1.0f;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID BlurShader31::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject31::SimpleObject31()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject31::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
    if( ShouldRotate )
        RotateTime += ElapsedTime * 0.5f;

    // scale the object
    FRMMATRIX4X4 ScaleMat = FrmMatrixScale( ModelScale31, ModelScale31, ModelScale31 );
    ModelMatrix = ScaleMat;

    // position
    FRMMATRIX4X4 OffsetMat = FrmMatrixTranslate( Position );
    ModelMatrix = FrmMatrixMultiply( ModelMatrix, OffsetMat );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject31::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample31()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample31::CSample31( const CHAR* strName ) : CScene( strName )
{
    m_GodRayTexture = NULL;

    m_nWidth = 800;
    m_nHeight = 480;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample31::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/31_Textures.pak" ) )
        return FALSE;

    // Create textures
    m_GodRayTexture = resource.GetTexture( "GodRay1" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5, m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddFloatVariable( &m_RayExtruderShader.RayLength, (char *)"Ray length" );
    m_UserInterface.AddFloatVariable( &m_CutoutShader.OccluderThreshold, (char *)"Occluder pickup distance" );
    m_UserInterface.AddFloatVariable( &m_CutoutShader.OccluderFadeout, (char *)"Occluder fade distance" );
    m_UserInterface.AddFloatVariable( &m_CombineShader.RayOpacity, (char *)"Ray opacity" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, (char *)"Decrease ray length" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, (char *)"Increase ray length" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_3, (char *)"Decrease pickup dist" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_4, (char *)"Increase pickup dist" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Decrease pickup fade" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Increase pickup fade" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Decrease opacity" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Increase opacity" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_9, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/Map3.mesh" ) )
        return FALSE;
    m_CameraPos = FRMVECTOR3( 6.2f, 2.0f, 0.0f );
    float Split = 1.1f;
    float yAdjust = -0.05f;
    ModelScale31 = 1.0f;

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
BOOL CSample31::InitShaders()
{
    // material shader
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

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MaterialShader31.vs", "Demos/AdrenoShaders/Shaders/MaterialShader31.fs",
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

    // cutout shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CutoutShader.vs", "Demos/AdrenoShaders/Shaders/CutoutShader.fs",
            &m_CutoutShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CutoutShader.ColorTextureId           = glGetUniformLocation( m_CutoutShader.ShaderId, "g_ColorTexture" );
        m_CutoutShader.DepthTextureId           = glGetUniformLocation( m_CutoutShader.ShaderId, "g_DepthTexture" );
        m_CutoutShader.OccluderParamsId         = glGetUniformLocation( m_CutoutShader.ShaderId, "g_OccluderParams" );
        m_CutoutShader.NearQAspectId            = glGetUniformLocation( m_CutoutShader.ShaderId, "g_NearQAspect" );
        m_CutoutShader.ScreenLightPosId         = glGetUniformLocation( m_CutoutShader.ShaderId, "g_ScreenLightPos" );
    }

    // downsize shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/DownsizeShader31.vs", "Demos/AdrenoShaders/Shaders/DownsizeShader31.fs",
            &m_DownsizeShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_DownsizeShader.ColorTextureId = glGetUniformLocation( m_DownsizeShader.ShaderId, "g_ColorTexture" );
    }

    // ray extruder shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/RayExtruderShader.vs", "Demos/AdrenoShaders/Shaders/RayExtruderShader.fs",
            &m_RayExtruderShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_RayExtruderShader.RayTextureId     = glGetUniformLocation( m_RayExtruderShader.ShaderId, "g_RayTexture" );
        m_RayExtruderShader.ScreenLightPosId = glGetUniformLocation( m_RayExtruderShader.ShaderId, "g_ScreenLightPos" );
        m_RayExtruderShader.RayParamsId = glGetUniformLocation( m_RayExtruderShader.ShaderId, "g_RayParams" );
    }

    // combine shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CombineShader31.vs", "Demos/AdrenoShaders/Shaders/CombineShader31.fs",
            &m_CombineShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CombineShader.ColorTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_ColorTexture" );
        m_CombineShader.RaysTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_RaysTexture" );
        m_CombineShader.RayOpacityId = glGetUniformLocation( m_CombineShader.ShaderId, "g_RayOpacity" );
        m_CombineShader.RayColorId = glGetUniformLocation( m_CombineShader.ShaderId, "g_RayColor" );
    }

    // blur shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/BlurShader31.vs", "Demos/AdrenoShaders/Shaders/BlurShader31.fs",
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

    // preview shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PreviewShader31.vs", "Demos/AdrenoShaders/Shaders/PreviewShader31.fs",
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
BOOL CSample31::Resize()
{
    m_Aspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    FLOAT32 FOVy = FrmRadians( 86 );
    zNear = 0.25f;
    zFar = 20.0f;
    m_matProj = FrmMatrixPerspectiveFovRH( FOVy, m_Aspect, zNear, zFar );

    // Q = Zf / Zf - Zn
    m_CutoutShader.NearFarQ.x = zNear;
    m_CutoutShader.NearFarQ.y = zFar;
    m_CutoutShader.NearFarQ.z = ( zFar / ( zFar - zNear ) );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( m_Aspect > 1.0f )
    {
        m_matProj.M(0,0) *= m_Aspect;
        m_matProj.M(1,1) *= m_Aspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    INT32 HalfWidth = m_nWidth / 4;
    INT32 HalfHeight = m_nHeight / 4;
    m_OffscreenRT.CreateRT( m_nWidth / 2, m_nHeight / 2, TRUE );
    m_CutoutRT.CreateRT( m_nWidth / 2, m_nHeight / 2, TRUE );
    m_QuarterRT.CreateRT( HalfWidth, HalfHeight, FALSE );
    m_ScratchRT.CreateRT( HalfWidth, HalfHeight, FALSE );

    //m_CutoutShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)HalfWidth, 1.0f / (FLOAT32)HalfHeight );
    m_BlurShader.StepSize = FRMVECTOR2( 1.0f / (FLOAT32)HalfWidth, 1.0f / (FLOAT32)HalfHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample31::Destroy()
{
    // Free shader memory
    m_MaterialShader.Destroy();
    m_CutoutShader.Destroy();
    m_DownsizeShader.Destroy();
    m_RayExtruderShader.Destroy();
    m_CombineShader.Destroy();

    // Free texture memory
    if( m_GodRayTexture ) m_GodRayTexture->Release();

    // Free objects
    m_Object.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos31()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos31( FLOAT32 fTime )
{
    FRMVECTOR3 BaseOffset = FRMVECTOR3( -1.5f, 1.8f, 0.7f );
    FRMVECTOR3 CurPosition = FRMVECTOR3( 2.2f * FrmCos( 0.25f * fTime ), 0.2f * FrmSin( 0.123f * fTime ), 0.7f * FrmSin( 0.25f * fTime ) );
    return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample31::Update()
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

    // ray length
    if( nButtons & INPUT_KEY_1 )
    {
        m_RayExtruderShader.RayLength -= 0.2f * ElapsedTime;
        m_RayExtruderShader.RayLength = max( 0.0f, m_RayExtruderShader.RayLength );
    }
    if( nButtons & INPUT_KEY_2 )
    {
        m_RayExtruderShader.RayLength += 0.2f * ElapsedTime;
        m_RayExtruderShader.RayLength = min( 0.2f, m_RayExtruderShader.RayLength );
    }

    // occluder threshold
    if( nButtons & INPUT_KEY_3 )
    {
        m_CutoutShader.OccluderThreshold -= 5.0f * ElapsedTime;
        m_CutoutShader.OccluderThreshold = max( 0.1f, m_CutoutShader.OccluderThreshold );
    }

    if( nButtons & INPUT_KEY_4 )
    {
        m_CutoutShader.OccluderThreshold += 5.0f * ElapsedTime;
        m_CutoutShader.OccluderThreshold = min( 10.0f, m_CutoutShader.OccluderThreshold );
    }

    // occluder fadeout
    if( nButtons & INPUT_KEY_5 )
    {
        m_CutoutShader.OccluderFadeout -= 5.0f * ElapsedTime;
        m_CutoutShader.OccluderFadeout = max( 0.0f, m_CutoutShader.OccluderFadeout );
    }

    if( nButtons & INPUT_KEY_6 )
    {
        m_CutoutShader.OccluderFadeout += 5.0f * ElapsedTime;
        m_CutoutShader.OccluderFadeout = min( 5.0f, m_CutoutShader.OccluderFadeout );
    }

    // ray opacity
    if( nButtons & INPUT_KEY_7 )
    {
        m_CombineShader.RayOpacity -= 1.0f * ElapsedTime;
        m_CombineShader.RayOpacity = max( 0.1f, m_CombineShader.RayOpacity );
    }

    if( nButtons & INPUT_KEY_8 )
    {
        m_CombineShader.RayOpacity += 1.0f * ElapsedTime;
        m_CombineShader.RayOpacity = min( 1.0f, m_CombineShader.RayOpacity );
    }

    // toggle rotation
    if( nPressedButtons & INPUT_KEY_9 )
    {
        m_ShouldRotate = 1 - m_ShouldRotate;
    }


    // update light position
    m_LightPos = FRMVECTOR3( -1.5f, 2.0f, -4.0f );

    // animate the camera
    static FLOAT32 TotalTime = 12.0f;
    if( m_ShouldRotate )
    {
        static FLOAT32 CameraSpeed = 1.0f;
        TotalTime += ElapsedTime * CameraSpeed;
    }

    // build camera transforms
    FRMVECTOR3 UpDir = FRMVECTOR3( 0.0f, 1.0f, 0.0f );
    FRMVECTOR3 LookPos = m_LightPos - FRMVECTOR3( 0.0, 1.0, 0.0 );

    m_CameraPos   = GetCameraPos31( TotalTime );
    m_matView     = FrmMatrixLookAtRH( m_CameraPos, LookPos, UpDir );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );


    // update objects
    m_Object.Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample31::DrawObject( SimpleObject31* Object )
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
// Name: GaussianDistribution31()
// Desc: 
//--------------------------------------------------------------------------------------
FLOAT32 GaussianDistribution31( FLOAT32 X, FLOAT32 Y, FLOAT32 Rho )
{
    FLOAT32 Gauss = 1.0f / sqrtf( 2.0f * FRM_PI * Rho * Rho );
    Gauss *= expf( -( X * X + Y * Y ) / ( 2.0f * Rho * Rho ) );

    return Gauss;
}


//--------------------------------------------------------------------------------------
// Name: SetupGaussWeights()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample31::SetupGaussWeights()
{
    FLOAT32 GaussSum = 0.0f;
    for( int i = 0; i < 4; i++ )
    {
        // normalize to -1..1
        FLOAT32 X = ( FLOAT32( i ) / 3.0f );
        X = ( X - 0.5f ) * 2.0f;

        // spread is tunable
        X *= m_BlurShader.GaussSpread;

        m_BlurShader.GaussWeight[i] = GaussianDistribution31( X, 0.0f, m_BlurShader.GaussRho );

        GaussSum += m_BlurShader.GaussWeight[i];
    }

    m_BlurShader.GaussInvSum = ( 1.0f / GaussSum );
}


//--------------------------------------------------------------------------------------
// Name: GaussBlur()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample31::GaussBlur( OffscreenRT4& TargetRT )
{
    // Blur in the X direction
    {
        m_ScratchRT.SetFramebuffer();

        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR2 StepSizeX = FRMVECTOR2( m_BlurShader.StepSize.x * m_BlurShader.GaussSpread, 0.0f );
        glUseProgram( m_BlurShader.ShaderId );
        glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
        glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
        glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeX.v );
        SetTexture( m_BlurShader.ColorTextureId, TargetRT.TextureHandle, 0 );

        RenderScreenAlignedQuadInv();
        
        m_ScratchRT.DetachFramebuffer(); 
    }

    // Then blur in the Y direction
    {
        TargetRT.SetFramebuffer();

        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR2 StepSizeY = FRMVECTOR2( 0.0f, m_BlurShader.StepSize.y * m_BlurShader.GaussSpread );
        glUseProgram( m_BlurShader.ShaderId );
        glUniform1fv( m_BlurShader.GaussWeightId, 4, m_BlurShader.GaussWeight );
        glUniform1f( m_BlurShader.GaussInvSumId, m_BlurShader.GaussInvSum );
        glUniform2fv( m_BlurShader.StepSizeId, 1, StepSizeY.v );
        SetTexture( m_BlurShader.ColorTextureId, m_ScratchRT.TextureHandle, 0 );

        RenderScreenAlignedQuadInv();
        
        TargetRT.DetachFramebuffer();
    }
}


//--------------------------------------------------------------------------------------
// Name: DrawSceneToBackbuffer()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample31::DrawSceneToBackbuffer()
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
// Name: DrawGodRays()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample31::DrawGodRays()
{
    FRMMATRIX4X4 MatModel            = FrmMatrixIdentity();
    FRMMATRIX4X4 MatModelView        = FrmMatrixMultiply( MatModel, m_matView );
    FRMMATRIX4X4 MatModelViewProj    = FrmMatrixMultiply( MatModelView, m_matProj );
    FRMMATRIX4X4 MatModelViewProjInv = FrmMatrixInverse( MatModelViewProj );

    FRMVECTOR3 LightPos = m_LightPos;
    FRMVECTOR4 ScreenLightPos = FrmVector3Transform( LightPos, MatModelViewProj );
    ScreenLightPos.x /= ScreenLightPos.w;
    ScreenLightPos.y /= ScreenLightPos.w;
    ScreenLightPos.z /= ScreenLightPos.w;

    // First, if the camera is pointed away from the light, don't draw light rays.
    // Screen space light rays don't work when the light is behind the camera.
    FRMVECTOR3 CamForward = FRMVECTOR3( MatModelViewProj.m[2][0], MatModelViewProj.m[2][1], MatModelViewProj.m[2][2] );
    FRMVECTOR3 Temp = m_LightPos - m_CameraPos;
    FRMVECTOR3 CamLightVec = FrmVector3Normalize( Temp );
    FLOAT32 FrontFace = FrmVector3Dot( CamForward, CamLightVec );

    // Only draw light rays if we're facing the light source
    if( FrontFace > 0.0f )
    {

        // Part 1: downsize the frame buffer to a smaller render target and
        // discriminate god ray occlusion shapes.
        {
            m_CutoutRT.SetFramebuffer();

            // Render a full-screen quad
            glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glDisable( GL_CULL_FACE );
            glDisable( GL_BLEND );
            glDisable( GL_DEPTH_TEST );

            FRMVECTOR3 Temp2 = m_CameraPos - m_LightPos;
            FLOAT32 DistanceToLight = FrmVector3Length( Temp2 );
            FLOAT32 DistanceThreshold = DistanceToLight + m_CutoutShader.OccluderThreshold;

            // attenuation
            FLOAT32 LightRadius = 0.5f;
            FLOAT32 LightPenumbra = 0.3f;

            // convert light radius to a screen space length
            FRMVECTOR3 Up = FRMVECTOR3( MatModelView.m[1][0], MatModelView.m[1][1], MatModelView.m[1][2] );
            FRMVECTOR3 LightLimbPos = m_LightPos + Up;
            FRMVECTOR4 ScreenLimbPos = FrmVector3Transform( LightLimbPos, MatModelViewProj );
            ScreenLimbPos.x /= ScreenLimbPos.w;
            ScreenLimbPos.y /= ScreenLimbPos.w;
            ScreenLimbPos.z /= ScreenLimbPos.w;
            FLOAT32 LightRadiusScreen = abs( ScreenLimbPos.y - ScreenLightPos.y );

            glUseProgram( m_CutoutShader.ShaderId );
            SetTexture( m_CutoutShader.DepthTextureId, m_OffscreenRT.DepthTextureHandle, 0 );
            SetTexture( m_CutoutShader.ColorTextureId, m_GodRayTexture->m_hTextureHandle, 1 );
            glUniform3f( m_CutoutShader.NearQAspectId, m_CutoutShader.NearFarQ.x, m_CutoutShader.NearFarQ.z, m_Aspect );
            glUniform4f( m_CutoutShader.OccluderParamsId, DistanceThreshold, m_CutoutShader.OccluderFadeout, LightRadiusScreen, LightPenumbra );
            glUniform2f( m_CutoutShader.ScreenLightPosId, ScreenLightPos.x, ScreenLightPos.y );

            RenderScreenAlignedQuadInv();
            
            m_CutoutRT.DetachFramebuffer();
        }

        // Part 2: downsize to a smaller render target.  this also blurs the cutout image as a bonus.
        {
            m_QuarterRT.SetFramebuffer();

            glDisable( GL_CULL_FACE );
            glDisable( GL_BLEND );
            glDisable( GL_DEPTH_TEST );

            glUseProgram( m_DownsizeShader.ShaderId );
            SetTexture( m_DownsizeShader.ColorTextureId, m_CutoutRT.TextureHandle, 0 );

            RenderScreenAlignedQuadInv();
            
            m_QuarterRT.DetachFramebuffer();
        }

        // Part 3: smear to create light rays
        GaussBlur( m_QuarterRT );
        {
            m_ScratchRT.SetFramebuffer();

            glUseProgram( m_RayExtruderShader.ShaderId );
            SetTexture( m_RayExtruderShader.RayTextureId, m_QuarterRT.TextureHandle, 0 );
            glUniform2f( m_RayExtruderShader.ScreenLightPosId, ScreenLightPos.x, ScreenLightPos.y );
            glUniform3f( m_RayExtruderShader.RayParamsId, m_Aspect, 0.0f, m_RayExtruderShader.RayLength * 0.25f );
            RenderScreenAlignedQuadInv();
            
            m_ScratchRT.DetachFramebuffer();
        }
        {
            m_QuarterRT.SetFramebuffer();

            glUseProgram( m_RayExtruderShader.ShaderId );
            SetTexture( m_RayExtruderShader.RayTextureId, m_ScratchRT.TextureHandle, 0 );
            glUniform2f( m_RayExtruderShader.ScreenLightPosId, ScreenLightPos.x, ScreenLightPos.y );
            glUniform3f( m_RayExtruderShader.RayParamsId, m_Aspect, 0.0f, m_RayExtruderShader.RayLength * 0.5f );
            RenderScreenAlignedQuadInv();
            
            m_QuarterRT.DetachFramebuffer();
        }
        {
            m_ScratchRT.SetFramebuffer();

            glUseProgram( m_RayExtruderShader.ShaderId );
            SetTexture( m_RayExtruderShader.RayTextureId, m_QuarterRT.TextureHandle, 0 );
            glUniform2f( m_RayExtruderShader.ScreenLightPosId, ScreenLightPos.x, ScreenLightPos.y );
            glUniform3f( m_RayExtruderShader.RayParamsId, m_Aspect, 0.0f, m_RayExtruderShader.RayLength * 0.75f );
            RenderScreenAlignedQuadInv();
            
            m_ScratchRT.DetachFramebuffer();
        }
        {
            m_QuarterRT.SetFramebuffer();

            glUseProgram( m_RayExtruderShader.ShaderId );
            SetTexture( m_RayExtruderShader.RayTextureId, m_ScratchRT.TextureHandle, 0 );
            glUniform2f( m_RayExtruderShader.ScreenLightPosId, ScreenLightPos.x, ScreenLightPos.y );
            glUniform3f( m_RayExtruderShader.RayParamsId, m_Aspect, 0.0f, m_RayExtruderShader.RayLength );
            RenderScreenAlignedQuadInv();
            
            m_QuarterRT.DetachFramebuffer();
        }
    }
    else
    {
        // The light is behind the camera.  Clear the quarter RT, then run the combine
        // pass as usual.

        m_QuarterRT.SetFramebuffer();
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        m_QuarterRT.DetachFramebuffer();
    }


    // Part 3: combine rays with the original frame buffer
    {
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glViewport( 0, 0, m_nWidth, m_nHeight );

        glClearColor( 0.04f, 0.04f, 0.04f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDisable( GL_CULL_FACE );
        glDisable( GL_BLEND );
        glDisable( GL_DEPTH_TEST );

        FRMVECTOR4 RayColor = FRMVECTOR4( 0.9f, 0.9f, 1.0f, 1.0f );

        glUseProgram( m_CombineShader.ShaderId );
        SetTexture( m_CombineShader.ColorTextureId, m_OffscreenRT.TextureHandle, 0 );
        SetTexture( m_CombineShader.RaysTextureId, m_QuarterRT.TextureHandle, 1 );
        glUniform1f( m_CombineShader.RayOpacityId, m_CombineShader.RayOpacity );
        glUniform4fv( m_CombineShader.RayColorId, 1, RayColor.v );

        RenderScreenAlignedQuadInv();
    }
}


VOID CSample31::PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle )
{
    TopLeft.y = -TopLeft.y;
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
VOID CSample31::Render()
{
    // Draw regular stuff to the offscreen framebuffer
    DrawSceneToBackbuffer();

    // Add god rays in a post pass
    DrawGodRays();
/*
    {
        FLOAT32 xPos = 0.0f;
        FLOAT32 yPos = -0.95f;
        FLOAT32 Width = 0.4f;
        FLOAT32 Height = 0.4f;

        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_OffscreenRT.TextureHandle );
		xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_QuarterRT.TextureHandle );
	    xPos += ( Width + 0.05f );
	}    
*/

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
