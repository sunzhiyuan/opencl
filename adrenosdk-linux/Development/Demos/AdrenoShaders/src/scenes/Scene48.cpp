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
#include "Scene48.h"

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
MaterialShader48::MaterialShader48()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID MaterialShader48::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
NormalShader48::NormalShader48()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID NormalShader48::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
PreviewShader::PreviewShader()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID PreviewShader::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
LightShader48::LightShader48()
{
    ShaderId = 0;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID LightShader48::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
CombineShader48::CombineShader48()
{
    ShaderId = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CombineShader48::Destroy()
{
    if( ShaderId ) glDeleteProgram( ShaderId );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
SimpleObject48::SimpleObject48()
{
    Position = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    RotateTime = 0.0f;
    Drawable = NULL;
    BumpTexture = NULL;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject48::Update( FLOAT ElapsedTime, BOOL ShouldRotate )
{
}


//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID SimpleObject48::Destroy()
{
}


//--------------------------------------------------------------------------------------
// Name: CSample48()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample48::CSample48( const CHAR* strName ) : CScene( strName )
{
    m_pLogoTexture = NULL;
    m_RayGunTexture = NULL;
    m_ShowLight = MAX_LIGHTS;

    m_nWidth = 800;
    m_nHeight = 480;
 //   m_nWidthLightRT = 800;
  //  m_nHeightLightRT = 480;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample48::Initialize(CFrmFontGLES &m_Font, CFrmTexture* m_pLogoTexture)
{
    m_ShouldRotate = TRUE;
    m_Preview = false;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/AdrenoShaders/Textures/48_Textures.pak" ) )
        return FALSE;

    // Create the ray gun texture
    m_RayGunTexture = resource.GetTexture( "RayGun" );


    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5, m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( (char *)"Press \200 for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_ShouldRotate, (char *)"Should Rotate" );
    m_UserInterface.AddIntVariable( &m_ShowLight, (char *)"Isolate light (6 == draw all)" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, (char *)"Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_5, (char *)"Toggle RT previews" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_6, (char *)"Cycle through lights -" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_7, (char *)"Cycle through lights +" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_8, (char *)"Toggle Rotation" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Demos/AdrenoShaders/Meshes/RayGun.mesh" ) )
        return FALSE;

    if( FALSE == m_Mesh.MakeDrawable( &resource ) )
        return FALSE;

	// Initialize the shaders
	if( FALSE == InitShaders() )
	{
		return FALSE;
	}

    m_AmbientLight = FRMVECTOR4( 0.1f, 0.1f, 0.1f, 0.0f );

    // Set up the objects
    m_Object.Drawable = &m_Mesh;

    // make some lights ... please note we specify in screen-space here ... not world space
    int i = 0;
    m_Lights[i].Position = FRMVECTOR3(0.0f, 0.0f, -0.15f );
    m_Lights[i].Radius = 0.5f;
    m_Lights[i].Color = FRMVECTOR3( 1.0f, 1.0f, 0.0f );

    i++;
    m_Lights[i].Position = FRMVECTOR3( 0.2f, 0.0f, -0.15f );
    m_Lights[i].Radius = 0.5f;
    m_Lights[i].Color = FRMVECTOR3( 1.0f, 0.0f, 1.0f );

    i++;
    m_Lights[i].Position = FRMVECTOR3( -0.2f, 0.0f, -0.15f );
    m_Lights[i].Radius = 0.5f;
    m_Lights[i].Color = FRMVECTOR3( 0.0f, 1.0f, 1.0f );

    i++;
    m_Lights[i].Position = FRMVECTOR3( -0.2f, -0.2f, -0.15f);
    m_Lights[i].Radius = 0.5f;
    m_Lights[i].Color = FRMVECTOR3( 0.0f, 0.0f, 1.0f );

    i++;
    m_Lights[i].Position = FRMVECTOR3( -0.2f, 0.2f, -0.15f );
    m_Lights[i].Radius = 0.5f;
    m_Lights[i].Color = FRMVECTOR3( 1.0f, 0.0f, 0.0f );

    i++;
    m_Lights[i].Position = FRMVECTOR3( 0.2f, 0.15f, -0.15f );
    m_Lights[i].Radius = 0.5f;
    m_Lights[i].Color = FRMVECTOR3( 1.0f, 0.5f, 1.0f );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample48::InitShaders()
{
    // Create the material shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_PositionIn", FRM_VERTEX_POSITION },
            { "g_TexCoordIn", FRM_VERTEX_TEXCOORD0 }
        };

        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/MaterialShader48.vs", "Demos/AdrenoShaders/Shaders/MaterialShader48.fs",
            &m_MaterialShader.ShaderId,
            pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_MaterialShader.m_ModelViewProjMatrixId = glGetUniformLocation( m_MaterialShader.ShaderId, "g_MatModelViewProj" );
        m_MaterialShader.m_DiffuseTextureId      = glGetUniformLocation( m_MaterialShader.ShaderId, "g_DiffuseTexture" );
    }

    // normal shader
	{
		FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
		{ 
			{ "g_vPositionOS", FRM_VERTEX_POSITION },
			{ "g_vNormalOS",   FRM_VERTEX_NORMAL }
		};
		const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

		if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/NormalShader48.vs", "Demos/AdrenoShaders/Shaders/NormalShader48.fs",
			&m_NormalShader.ShaderId,
			pShaderAttributes, nNumShaderAttributes ) )
			return FALSE;

		// Make a record of the location for each shader constant
		m_NormalShader.ModelViewProjMatrixLoc = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matModelViewProj" );
		m_NormalShader.NormalMatrixLoc        = glGetUniformLocation( m_NormalShader.ShaderId,  "g_matNormal" );
	}

    // preview shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/PreviewShader.vs", "Demos/AdrenoShaders/Shaders/PreviewShader.fs",
            &m_PreviewShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_PreviewShader.ColorTextureId = glGetUniformLocation( m_PreviewShader.ShaderId, "g_ColorTexture" );
    }
    
    // light shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/LightShader48.vs", "Demos/AdrenoShaders/Shaders/LightShader48.fs",
            &m_LightShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_LightShader.NormalTextureId = glGetUniformLocation( m_LightShader.ShaderId, "g_NormalTexture" );
        m_LightShader.DepthTextureId = glGetUniformLocation( m_LightShader.ShaderId, "g_DepthTexture" );
        m_LightShader.LightPosId = glGetUniformLocation( m_LightShader.ShaderId, "g_LightPos" );
        m_LightShader.ModelViewProjInvMatrixId = glGetUniformLocation( m_LightShader.ShaderId, "g_MatModelViewProjInv" );
        m_LightShader.LightColorId = glGetUniformLocation( m_LightShader.ShaderId, "g_LightColor" );
    }

    // combine shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_Vertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/AdrenoShaders/Shaders/CombineShader48.vs", "Demos/AdrenoShaders/Shaders/CombineShader48.fs",
            &m_CombineShader.ShaderId, pShaderAttributes,
            nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_CombineShader.ColorTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_ColorTexture" );
        m_CombineShader.LightTextureId = glGetUniformLocation( m_CombineShader.ShaderId, "g_LightTexture" );
    }


    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample48::Resize()
{
    m_Aspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    FLOAT32 FOVy = FrmRadians( 86 );
    FLOAT32 zNear = 0.25f;
    FLOAT32 zFar = 20.0f;
    m_matProj = FrmMatrixPerspectiveFovRH( FOVy, m_Aspect, zNear, zFar );

    // To keep sizes consistent, scale the projection matrix in landscape orientation
    if( m_Aspect > 1.0f )
    {
        m_matProj.M(0,0) *= m_Aspect;
        m_matProj.M(1,1) *= m_Aspect;
    }

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    m_ColorRT.CreateRT( m_nWidth / 2, m_nHeight / 2, TRUE );
    m_NormalRT.CreateRT( m_nWidth / 2, m_nHeight / 2, FALSE );
    m_LightRT.CreateRT( m_nWidth / 2, m_nHeight / 2, FALSE );
    m_NormalRT.DepthTextureHandle = m_ColorRT.DepthTextureHandle;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample48::Destroy()
{
    // Free shader memory
    m_MaterialShader.Destroy();
    m_NormalShader.Destroy();
    m_CombineShader.Destroy();
    m_PreviewShader.Destroy();
    m_LightShader.Destroy();
 
    if( m_NormalShader.ShaderId ) glDeleteProgram( m_NormalShader.ShaderId );


    // Free texture memory
    if( m_pLogoTexture ) m_pLogoTexture->Release();
    if( m_RayGunTexture) m_RayGunTexture->Release();

    // Free objects
    m_Object.Destroy();

    // Free mesh memory
    m_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Name: GetCameraPos48()
// Desc: 
//--------------------------------------------------------------------------------------
FRMVECTOR3 GetCameraPos48( FLOAT32 fTime )
{
    FRMVECTOR3 BaseOffset = FRMVECTOR3( 4.5f, 2.0f, 0.0f );
    FRMVECTOR3 CurPosition = FRMVECTOR3( 1.7f * FrmCos( 0.25f * fTime ), 0.5f * FrmSin( 0.15f * fTime ), -1.9f * FrmSin( 0.25f * fTime ) );
    return BaseOffset + CurPosition;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample48::Update()
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

    // toggle previews
    if( nPressedButtons & INPUT_KEY_5 )
    {
        m_Preview = 1 - m_Preview;
    }

    // cycle through lights
    if( nPressedButtons & INPUT_KEY_6 )
    {
        if( m_ShowLight > 0 )
            m_ShowLight--;
        else
            m_ShowLight = MAX_LIGHTS;
    }

    if( nPressedButtons & INPUT_KEY_7 )
    {
        if( m_ShowLight < MAX_LIGHTS )
            m_ShowLight++;
        else
            m_ShowLight = 0;
    }

    // update light position
    m_LightPos = FRMVECTOR3( 3.3f, 2.0f, -0.42f );

    // animate the camera
    static FLOAT32 TotalTime = 0.0f;
    if( m_ShouldRotate )
    {
        static FLOAT32 CameraSpeed = 1.0f;
        TotalTime += ElapsedTime * CameraSpeed;
    }


    // Rotate the object
    FRMMATRIX4X4 matRotate1 = FrmMatrixRotate( TotalTime, 0.0f, 1.0f, 0.0f );
    FRMMATRIX4X4 matRotate2 = FrmMatrixRotate( 0.3f,  1.0f, 0.0f, 0.0f );
    static FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.2f, 0.25f );
    static FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.1f, 0.0f );

    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );
    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // Build the matrices
    m_matModel         = FrmMatrixMultiply( matRotate2,     matRotate1 );
    m_matModelView     = FrmMatrixMultiply( m_matModel,     m_matView );
    m_matModelViewProj = FrmMatrixMultiply( m_matModelView, m_matProj );
    m_matNormal        = FrmMatrixNormal( m_matModel );
    m_matViewProjInv   = FrmMatrixInverse( m_matViewProj );

    // update objects
    m_Object.Update( ElapsedTime, m_ShouldRotate );
}


//--------------------------------------------------------------------------------------
// Name: DrawObject()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample48::DrawObject( SimpleObject48* Object )
{
    if ( !Object || !Object->Drawable )
        return;

    glUseProgram( m_MaterialShader.ShaderId );
    glUniformMatrix4fv( m_MaterialShader.m_ModelViewProjMatrixId, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    SetTexture( m_MaterialShader.m_DiffuseTextureId, m_RayGunTexture->m_hTextureHandle, 0 );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: DrawObjectNormals()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample48::DrawObjectNormals( SimpleObject48* Object )
{
    if ( !Object || !Object->Drawable )
        return;

    // Set shader variables
    glUseProgram( m_NormalShader.ShaderId );
    glUniformMatrix4fv( m_NormalShader.ModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniformMatrix3fv( m_NormalShader.NormalMatrixLoc, 1, FALSE, (FLOAT32*)&m_matNormal );

    Object->Drawable->Render();
}


//--------------------------------------------------------------------------------------
// Name: RenderSceneColor()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample48::RenderSceneColor(SimpleObject48* Object)
{
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClearDepthf( 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    DrawObject( &m_Object );
}


//--------------------------------------------------------------------------------------
// Name: RenderSceneNormals()
// Desc: Draws the scene normals to an off-screen render target
//--------------------------------------------------------------------------------------
VOID CSample48::RenderSceneNormals(SimpleObject48* Object)
{
    // Clear the frame
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
     
    DrawObjectNormals( &m_Object );
}


//--------------------------------------------------------------------------------------
// Name: PreviewRT()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample48::PreviewRT( FRMVECTOR2 TopLeft, FRMVECTOR2 Dims, UINT32 TextureHandle )
{
    // There is currently a bug in Qualcomm's shader compiler in that it can't
    // negate a value.  Therefore, change the shader and the negate y value here.
    //TopLeft.y = TopLeft.y + Dims.y;
    //Dims.y = -Dims.y;

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
// Name: LightBlit()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample48::LightBlitMinMax( const LightObject48& Light, FRMVECTOR2 TopLeft, FRMVECTOR2 BottomRight )
{
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );

    FRMVECTOR4 LightData = FRMVECTOR4( Light.Position.x, Light.Position.y, Light.Position.z, Light.Radius );

    glUseProgram( m_LightShader.ShaderId );
    SetTexture( m_LightShader.NormalTextureId, m_NormalRT.TextureHandle, 0 );
    SetTexture( m_LightShader.DepthTextureId, m_NormalRT.DepthTextureHandle, 1 );
    glUniformMatrix4fv( m_LightShader.ModelViewProjInvMatrixId, 1, FALSE, (FLOAT32*)&m_matViewProjInv );
    glUniform4fv( m_LightShader.LightPosId, 1, LightData.v );
    glUniform3fv( m_LightShader.LightColorId, 1, Light.Color.v );


	RenderScreenAlignedQuad(TopLeft, BottomRight );
    //RenderScreenAlignedQuad();
}

VOID CSample48::LightBlit( const LightObject48& Light )
{
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );

    FRMVECTOR4 LightData = FRMVECTOR4( Light.Position.x, Light.Position.y, Light.Position.z, Light.Radius );

    glUseProgram( m_LightShader.ShaderId );
    SetTexture( m_LightShader.NormalTextureId, m_NormalRT.TextureHandle, 0 );
    SetTexture( m_LightShader.DepthTextureId, m_NormalRT.DepthTextureHandle, 1 );
    glUniformMatrix4fv( m_LightShader.ModelViewProjInvMatrixId, 1, FALSE, (FLOAT32*)&m_matViewProjInv );
    glUniform4fv( m_LightShader.LightPosId, 1, LightData.v );
    glUniform3fv( m_LightShader.LightColorId, 1, Light.Color.v );


    RenderScreenAlignedQuad();
}



float clamp(float X, float Min, float Max)
 {
   if( X > Max )
     X = Max;
   else if( X < Min )
     X = Min;
 
   return X;
 }

void CSample48::DeterminePointLightClipBox(const FRMVECTOR3 position, const float range, FRMVECTOR3 &minVec, FRMVECTOR3 &maxVec)
{
	FRMVECTOR3 bbox3D[8];
	
	// construct the 3D box
	bbox3D[0].x = position.x - range;	bbox3D[0].y = position.y + range;	bbox3D[0].z = position.z - range;
	bbox3D[1].x = position.x + range;	bbox3D[1].y = position.y + range;	bbox3D[1].z = position.z - range;
	bbox3D[2].x = position.x - range;	bbox3D[2].y = position.y - range;	bbox3D[2].z = position.z - range;
	bbox3D[3].x = position.x + range;	bbox3D[3].y = position.y - range;	bbox3D[3].z = position.z - range;
	bbox3D[4].x = position.x - range;	bbox3D[4].y = position.y + range;	bbox3D[4].z = position.z + range;
	bbox3D[5].x = position.x + range;	bbox3D[5].y = position.y + range;	bbox3D[5].z = position.z + range;
	bbox3D[6].x = position.x - range;	bbox3D[6].y = position.y - range;	bbox3D[6].z = position.z + range;
	bbox3D[7].x = position.x + range;	bbox3D[7].y = position.y - range;	bbox3D[7].z = position.z + range;

	//
	// you might want to check here if we are in the bounding volume of the light volume to be able to "react" on this
	//

	FRMVECTOR3 projBox[8];
	for (int i = 0; i < 8; ++i)
	{
	   FRMVECTOR4 projPoint;

	   // project the box into screen-space
	   // in this demo environment we actually provide screen-space values and not world-space ... so this is not really necessary
	   FRMMATRIX4X4 IdentityMatrix = FrmMatrixIdentity();
	   projPoint = FrmVector3Transform(bbox3D[i], IdentityMatrix);

	   projBox[i].x = projPoint.x / projPoint.w;
	   projBox[i].y = projPoint.y / projPoint.w;
	   projBox[i].z = projPoint.z / projPoint.w;

	   // check if we went past the front clipping plane
	   // Negative W indicates that we went past the front clipping plane.
	   // if (projPoint.w <= 0.0f)
	   // {
	   //   projBox[i].z = -1.0f;
	   // }

	   // clip to extents
	   projBox[i].x = clamp(projBox[i].x, -1.0f, 1.0f);
	   projBox[i].y = clamp(projBox[i].y, -1.0f, 1.0f);
	   projBox[i].z = clamp(projBox[i].z, -1.0f, 1.0f);
	}

	minVec = maxVec = projBox[0];

	// build a 3D box ... in case you can use depth bounds in the future or just want to use a geometry shader to clip this
	for (int i = 1; i < 8; ++i)
	{
		   minVec.x = min(minVec.x, projBox[i].x);
		   minVec.y = min(minVec.y, projBox[i].y);
		   minVec.z = min(minVec.z, projBox[i].z);
		   maxVec.x = max(maxVec.x, projBox[i].x);
		   maxVec.y = max(maxVec.y, projBox[i].y);
		   maxVec.z = max(maxVec.z, projBox[i].z);
	}
}	

//--------------------------------------------------------------------------------------
// Name: DrawScene()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample48::DrawScene()
{
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
    glEnable( GL_DEPTH_TEST );
    glDisable( GL_BLEND );

    // color + depth
    m_ColorRT.SetFramebuffer();
    glDepthFunc( GL_LESS );
    RenderSceneColor( &m_Object );
    m_ColorRT.DetachFramebuffer();

    // normals
    m_NormalRT.SetFramebuffer();
    glDepthFunc( GL_LEQUAL );
    RenderSceneNormals( &m_Object );
    m_NormalRT.DetachFramebuffer();

    // lights	
    m_LightRT.SetFramebuffer();
  
  //  glViewport( 0, 0, m_nWidthLightRT, m_nHeightLightRT);
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    glEnable( GL_BLEND );
    glBlendFunc( GL_ONE, GL_ONE );
    
    if( m_ShowLight == MAX_LIGHTS )
    {

        // draw them all
        for( int i = 0; i < MAX_LIGHTS; i++ )
        {
			glEnable(GL_SCISSOR_TEST);

			// get scissor rectangle
			FRMVECTOR3 minVec, maxVec;
			DeterminePointLightClipBox(m_Lights[i].Position, m_Lights[i].Radius, minVec,  maxVec);
			
			FRMVECTOR3 half = FRMVECTOR3(0.5f, 0.5f, 0.5f);
			FRMVECTOR3 minV = minVec * 0.5f + half;
			FRMVECTOR3 maxV = maxVec * 0.5f + half;
			
			// we halfed the light buffer
			INT32 w = m_nWidth / 2;
			INT32 h = m_nHeight / 2;

			//
			// glScissor understands only int values and goes from 0..width
			//
			// convert the -1.0..1.0 float values to integer values going from 0..width
			GLint x = (GLint) (w * minV.x);
			GLint y = (GLint) (h * minV.y);
			
			GLint width = (GLint) (w * (maxV.x - minV.x));
			GLint height = (GLint) (h * (maxV.y - minV.y));

			glScissor(x, y, width, height);

			//FRMVECTOR2 TopLeft = FRMVECTOR2(minVec.x, minVec.y);
			//FRMVECTOR2 BottomRight = FRMVECTOR2(maxVec.x, maxVec.y);

			LightBlit( m_Lights[i]);
			
			glDisable(GL_SCISSOR_TEST);
        }
        
    }
    else
    {    
        LightBlit( m_Lights[m_ShowLight] );
    }
    m_LightRT.DetachFramebuffer();

    // combine
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nWidth, m_nHeight );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT );
    glDisable( GL_CULL_FACE );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );
    glUseProgram( m_CombineShader.ShaderId );
    SetTexture( m_CombineShader.ColorTextureId, m_ColorRT.TextureHandle, 0 );
    SetTexture( m_CombineShader.LightTextureId, m_LightRT.TextureHandle, 1 );
    RenderScreenAlignedQuad();
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample48::Render()
{
    // Draw the scene with deferred lighting
    DrawScene();


    // Show render target previews
    if( m_Preview )
    {
        FLOAT32 xPos = -0.95f;
        FLOAT32 yPos = -0.95f;
        FLOAT32 Width = 0.3f;
        FLOAT32 Height = 0.3f;
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_ColorRT.TextureHandle );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_NormalRT.TextureHandle );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_NormalRT.DepthTextureHandle );
        xPos += ( Width + 0.05f );
        PreviewRT( FRMVECTOR2( xPos, yPos ), FRMVECTOR2( Width, Height ), m_LightRT.TextureHandle );
    }


    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );

    // Put the state back
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glFrontFace( GL_CCW );
}
