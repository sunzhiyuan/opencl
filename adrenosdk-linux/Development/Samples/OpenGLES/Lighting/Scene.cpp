//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <FrmUtils.h>
#include "Scene.h"


//--------------------------------------------------------------------------------------
// Name: MaterialList[]
// Desc: Global list of materials
//--------------------------------------------------------------------------------------
struct MATERIAL
{
    const CHAR* strMaterial;
    FRMVECTOR4  vAmbient;
    FRMVECTOR4  vDiffuse;
    FRMVECTOR3  vSpecular;
    FLOAT32     fShininess;
};

static const CHAR* LIGHTING_TYPE[] = { "Per Vertex", "Per Pixel" };

static const MATERIAL MaterialList[] =
{
    {   "Default Material",
        FRMVECTOR4( 0.0f,      0.1f,      0.25f,     1.0f ),
        FRMVECTOR4( 0.0f,      0.4f,      1.0f,      1.0f ),
        FRMVECTOR3( 1.0f,      1.0f,      1.0f ),
        20.0f
    },
    {   "Emerald",
        FRMVECTOR4( 0.0215f,   0.1745f,   0.0215f,   1.0f ),
        FRMVECTOR4( 0.07568f,  0.61424f,  0.07568f,  1.0f ),
        FRMVECTOR3( 0.633f,    0.727811f, 0.633f ),
        76.8f,
    },
    {   "Jade",
        FRMVECTOR4( 0.135f,    0.2225f,   0.1575f,   1.0f ),
        FRMVECTOR4( 0.54f,     0.89f,     0.63f,     1.0f ),
        FRMVECTOR3( 0.316228f, 0.316228f, 0.316228f ),
        12.8f,
    },
    {   "Pearl",
        FRMVECTOR4( 0.25f,     0.20725f,  0.20725f,  1.0f ),
        FRMVECTOR4( 1.0f,      0.829f,    0.829f,    1.0f ),
        FRMVECTOR3( 0.296648f, 0.296648f, 0.2966f ),
        11.264f,
    },
    {   "Ruby",
        FRMVECTOR4( 0.1745f,   0.01175f,  0.01175f,  1.0f ),
        FRMVECTOR4( 0.61424f,  0.04136f,  0.04136f,  1.0f ),
        FRMVECTOR3( 0.727811f, 0.626959f, 0.626959f ),
        76.8f,
    },
    {   "Turquoise",
        FRMVECTOR4( 0.1f,      0.18725f,  0.1745f,   1.0f ),
        FRMVECTOR4( 0.396f,    0.74151f,  0.69102f,  1.0f ),
        FRMVECTOR3( 0.297254f, 0.30829f,  0.306678f ),
        12.8f,
    },
    {   "Brass",
        FRMVECTOR4( 0.329412f, 0.223529f, 0.027451f, 1.0f ),
        FRMVECTOR4( 0.780382f, 0.568627f, 0.113725f, 1.0f ),
        FRMVECTOR3( 0.992157f, 0.941176f, 0.807843f ),
        27.897f,
    },
    {   "Bronze",
        FRMVECTOR4( 0.2125f,   0.1275f,   0.054f,    1.0f ),
        FRMVECTOR4( 0.714f,    0.4284f,   0.18144f,  1.0f ),
        FRMVECTOR3( 0.393548f, 0.271906f, 0.166721f ),
        25.6f,
    },
    {   "Copper",
        FRMVECTOR4( 0.19125f,  0.0735f,   0.0225f,   1.0f ),
        FRMVECTOR4( 0.7038f,   0.27048f,  0.0828f,   1.0f ),
        FRMVECTOR3( 0.256777f, 0.137622f, 0.086014f ),
        12.8f,
    },
    {   "Gold",
        FRMVECTOR4( 0.24725f,  0.1995f,   0.0745f,   1.0f ),
        FRMVECTOR4( 0.75164f,  0.60648f,  0.22648f,  1.0f ),
        FRMVECTOR3( 0.628281f, 0.555802f, 0.366065f ),
        51.2f,
    },
    {   "Silver",
        FRMVECTOR4( 0.19225f,  0.19225f,  0.19225f,  1.0f ),
        FRMVECTOR4( 0.50754f,  0.50754f,  0.50754f,  1.0f ),
        FRMVECTOR3( 0.508273f, 0.508273f, 0.508273f ),
        51.2f,
    },
    {
        "Yellow Plastic",
        FRMVECTOR4( 0.0f,      0.0f,      0.0f,      1.0f ),
        FRMVECTOR4( 0.5f,      0.5f,      0.0f,      1.0f ),
        FRMVECTOR3( 0.6f,      0.6f,      0.5f ),
        32.0f,
    },
    {
        "Cyan Rubber",
        FRMVECTOR4( 0.0f,      0.05f,     0.05f,     1.0f ), 
        FRMVECTOR4( 0.4f,      0.5f,      0.5f,      1.0f ),
        FRMVECTOR3( 0.04f,     0.7f,      0.7f ),
        10.0f,
    }
};

const int NUM_MATERIALS = sizeof(MaterialList) / sizeof(MaterialList[0]);


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Lighting" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_hPerPixelLightingShader  = 0;
    m_hPerVertexLightingShader = 0;
    m_pLogoTexture      = NULL;
    m_qMeshRotation     = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
    m_qRotationDelta    = FRMVECTOR4( 0.0f, 0.01f, 0.0f, 1.0f );
    m_bPerPixelLighting = TRUE;
    m_strLightingType   = LIGHTING_TYPE[ m_bPerPixelLighting ];
    m_nMaterialIndex    = 0;
    m_strMaterialName   = MaterialList[ m_nMaterialIndex ].strMaterial;
    m_vLightPosition.x  = 2.0f;
    m_vLightPosition.y  = 1.0f;
    m_vLightPosition.z  = 1.0f;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the PerPixelLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vPositionOS", FRM_VERTEX_POSITION },
            { "g_vNormalOS",   FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PerPixelLighting.vs",
                                                      "Samples/Shaders/PerPixelLighting.fs",
                                                      &m_hPerPixelLightingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hPPLModelViewMatrixLoc     = glGetUniformLocation( m_hPerPixelLightingShader, "g_matModelView" );
        m_hPPLModelViewProjMatrixLoc = glGetUniformLocation( m_hPerPixelLightingShader, "g_matModelViewProj" );
        m_hPPLNormalMatrixLoc        = glGetUniformLocation( m_hPerPixelLightingShader, "g_matNormal" );
        m_hPPLLightPositionLoc       = glGetUniformLocation( m_hPerPixelLightingShader, "g_vLightPos" );
        m_hPPLMaterialAmbientLoc     = glGetUniformLocation( m_hPerPixelLightingShader, "g_Material.vAmbient" );
        m_hPPLMaterialDiffuseLoc     = glGetUniformLocation( m_hPerPixelLightingShader, "g_Material.vDiffuse" );
        m_hPPLMaterialSpecularLoc    = glGetUniformLocation( m_hPerPixelLightingShader, "g_Material.vSpecular" );
    }


    // Create the PerVertexLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vPositionOS", FRM_VERTEX_POSITION },
            { "g_vNormalOS",   FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PerVertexLighting.vs",
                                                      "Samples/Shaders/PerVertexLighting.fs",
                                                      &m_hPerVertexLightingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hPVLModelViewMatrixLoc     = glGetUniformLocation( m_hPerVertexLightingShader, "g_matModelView" );
        m_hPVLModelViewProjMatrixLoc = glGetUniformLocation( m_hPerVertexLightingShader, "g_matModelViewProj" );
        m_hPVLNormalMatrixLoc        = glGetUniformLocation( m_hPerVertexLightingShader, "g_matNormal" );
        m_hPVLLightPositionLoc       = glGetUniformLocation( m_hPerVertexLightingShader, "g_vLightPos" );
        m_hPVLMaterialAmbientLoc     = glGetUniformLocation( m_hPerVertexLightingShader, "g_Material.vAmbient" );
        m_hPVLMaterialDiffuseLoc     = glGetUniformLocation( m_hPerVertexLightingShader, "g_Material.vDiffuse" );
        m_hPVLMaterialSpecularLoc    = glGetUniformLocation( m_hPerVertexLightingShader, "g_Material.vSpecular" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/Lighting.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddStringVariable( &m_strMaterialName, "Material" );
    m_UserInterface.AddStringVariable( &m_strLightingType, "Type" );
    m_UserInterface.AddFloatVariable( &m_vLightPosition.x, "Light Pos" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Next Material" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Toggle Per-Pixel Lighting" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_DPAD, "Move Light" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Samples/Meshes/Phone.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh.MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;
    
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.1f, 0.5f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );

    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 0.1f, 10.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Resize the arc ball control
    m_ArcBall.Resize( m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_hPerPixelLightingShader )  glDeleteProgram( m_hPerPixelLightingShader );
    if( m_hPerVertexLightingShader ) glDeleteProgram( m_hPerVertexLightingShader );

    if( m_pLogoTexture ) m_pLogoTexture->Release();
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();

    // Process input
    {
        // Get button input since the last frame
        UINT32 nButtons;
        UINT32 nPressedButtons;
        m_Input.GetButtonState( &nButtons, &nPressedButtons );

        // Get pointer input since the last frame
        UINT32     nPointerState;
        FRMVECTOR2 vPointerPosition;
        m_Input.GetPointerState( &nPointerState, &vPointerPosition );

        // Conditionally advance the UI state
        m_UserInterface.HandleInput( m_nWidth, m_nHeight, nPressedButtons,
                                     nPointerState, vPointerPosition );

        // Use the pointer input to rotate the object
        FRMVECTOR4 qArcBallDelta = m_ArcBall.HandleInput( vPointerPosition, nPointerState & FRM_INPUT::POINTER_DOWN );
        if( m_ArcBall.IsDragging() )
            m_qRotationDelta = qArcBallDelta;
        FRMVECTOR4 qDelta = nPointerState ? qArcBallDelta : m_qRotationDelta;
        
        qDelta = FrmVector4Normalize( qDelta );
        m_qMeshRotation = FrmQuaternionMultiply( m_qMeshRotation, qDelta );

        if( nPressedButtons & FRM_INPUT::KEY_1 )
        {
            m_nMaterialIndex  = ( m_nMaterialIndex + 1 ) % NUM_MATERIALS;
            m_strMaterialName = MaterialList[ m_nMaterialIndex ].strMaterial;
        }

        if( nPressedButtons & FRM_INPUT::KEY_2 )
        {
            // Toggle between per-vertex and per-fragment lighting
            m_bPerPixelLighting = !m_bPerPixelLighting;

            m_strLightingType = LIGHTING_TYPE[ m_bPerPixelLighting ];
        }

        // Move the light up/down/left/right
        if( nButtons & FRM_INPUT::DPAD_UP )
            m_vLightPosition.y = FrmMin( +5.0f, m_vLightPosition.y + 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_DOWN )
            m_vLightPosition.y = FrmMax( -5.0f, m_vLightPosition.y - 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_LEFT )
            m_vLightPosition.x = FrmMax( -5.0f, m_vLightPosition.x - 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_RIGHT )
            m_vLightPosition.x = FrmMin( +5.0f, m_vLightPosition.x + 5.0f * fElapsedTime );
    }

    // Orientate the object
    FRMMATRIX4X4 matRotate    = FrmMatrixRotate( m_qMeshRotation );
    FRMMATRIX4X4 matTranslate = FrmMatrixTranslate( 0.0f, -0.07f, 0.0f );

    // Build the matrices
    FRMMATRIX4X4 matModel = FrmMatrixMultiply( matTranslate,   matRotate );
    m_matModelView        = FrmMatrixMultiply( matModel,       m_matView );
    m_matModelViewProj    = FrmMatrixMultiply( m_matModelView, m_matProj );
    m_matNormal           = FrmMatrixNormal( m_matModelView );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    // Clear the backbuffer and depth-buffer
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if( m_bPerPixelLighting )
    {
        glUseProgram( m_hPerPixelLightingShader );
        glUniform4fv( m_hPPLMaterialAmbientLoc,  1, (FLOAT32*)&MaterialList[m_nMaterialIndex].vAmbient );
        glUniform4fv( m_hPPLMaterialDiffuseLoc,  1, (FLOAT32*)&MaterialList[m_nMaterialIndex].vDiffuse );
        glUniform4fv( m_hPPLMaterialSpecularLoc, 1, (FLOAT32*)&MaterialList[m_nMaterialIndex].vSpecular );
        glUniformMatrix4fv( m_hPPLModelViewMatrixLoc,     1, FALSE, (FLOAT32*)&m_matModelView );
        glUniformMatrix4fv( m_hPPLModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
        glUniformMatrix3fv( m_hPPLNormalMatrixLoc,        1, FALSE, (FLOAT32*)&m_matNormal );
        glUniform3fv( m_hPPLLightPositionLoc, 1, (FLOAT32*)&m_vLightPosition );
    }
    else
    {
        glUseProgram( m_hPerVertexLightingShader );
        glUniform4fv( m_hPVLMaterialAmbientLoc,  1, (FLOAT32*)&MaterialList[m_nMaterialIndex].vAmbient );
        glUniform4fv( m_hPVLMaterialDiffuseLoc,  1, (FLOAT32*)&MaterialList[m_nMaterialIndex].vDiffuse );
        glUniform4fv( m_hPVLMaterialSpecularLoc, 1, (FLOAT32*)&MaterialList[m_nMaterialIndex].vSpecular );
        glUniformMatrix4fv( m_hPVLModelViewMatrixLoc,     1, FALSE, (FLOAT32*)&m_matModelView );
        glUniformMatrix4fv( m_hPVLModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
        glUniformMatrix3fv( m_hPVLNormalMatrixLoc,        1, FALSE, (FLOAT32*)&m_matNormal );
        glUniform3fv( m_hPVLLightPositionLoc, 1, (FLOAT32*)&m_vLightPosition );
    }

    m_Mesh.Render();

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

