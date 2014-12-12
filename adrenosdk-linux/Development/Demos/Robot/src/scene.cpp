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
#include "scene.h"

//--------------------------------------------------------------------------------------
// Helper function to render a texture in screenspace
//--------------------------------------------------------------------------------------
class CFrmTexture; // Predeclaration
VOID FrmRenderTextureToScreen_GLES( FLOAT32 sx, FLOAT32 sy, FLOAT32 fScale, 
	CFrmTexture* pTexture, INT32 hShader, 
	INT32 hScreenSizeLoc );
VOID FrmRenderTextureToScreen_GLES( FLOAT32 sx, FLOAT32 sy, FLOAT32 w, FLOAT32 h,
	UINT32 hTexture, INT32 hShader, 
	INT32 hScreenSizeLoc );

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "Robot Demo" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_hOverlayShader		 = 0;
    m_hScreenSizeLoc         = 0;
    m_pLogoTexture			 = NULL;

    m_pScene				 = NULL;
    FrmMemset( m_Meshes, 0, sizeof(m_Meshes) );

    m_pWhiteTexture			 = NULL;
    m_pSilverSphereMap		 = NULL;
    m_pRedSphereMap			 = NULL;

    m_pRobotWalkAnimation	 = NULL;
    m_pRobotBeserkAnimation	 = NULL;
    m_pRobotCurrentAnimation = NULL;
    m_pAnimBlender			 = NULL;

    // Override the display size
    m_nWidth     = 512;
    m_nHeight    = 350;
//    m_nAltWidth  = 640;
//    m_nAltHeight = 480;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the shader program for rendering the robot mesh
    {
        FRM_VERTEX_ELEMENT VertexLayout[] = 
        {
            { FRM_VERTEX_POSITION,  3, GL_FLOAT, FALSE, 3*sizeof(FLOAT32) },
            { FRM_VERTEX_NORMAL,    3, GL_FLOAT, FALSE, 3*sizeof(FLOAT32) },
            { FRM_VERTEX_TEXCOORD0, 2, GL_FLOAT, FALSE, 2*sizeof(FLOAT32) },
            NULL
        };

        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "In_Pos",    FRM_VERTEX_POSITION },
            { "In_Normal", FRM_VERTEX_NORMAL },
            { "In_Tex0",   FRM_VERTEX_TEXCOORD0 },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/Robot/Shaders/Robot.vs",
                                                      "Demos/Robot/Shaders/Robot.fs",
                                                      &m_RobotShaderSet.hShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_RobotShaderSet.slotLightDir = glGetUniformLocation( m_RobotShaderSet.hShader,  "g_vLightDir" );
        m_RobotShaderSet.slotMtrlDif  = glGetUniformLocation( m_RobotShaderSet.hShader,  "g_vMaterialDiffuse" );
        m_RobotShaderSet.slotMtrlSpc  = glGetUniformLocation( m_RobotShaderSet.hShader,  "g_vMaterialSpecular" );
        m_RobotShaderSet.slotMtrlAmb  = glGetUniformLocation( m_RobotShaderSet.hShader,  "g_vMaterialAmbient" );
        m_RobotShaderSet.slotMtrlEms  = glGetUniformLocation( m_RobotShaderSet.hShader,  "g_vMaterialEmissive" );
        m_RobotShaderSet.slotNormal   = glGetUniformLocation( m_RobotShaderSet.hShader,  "g_matNormal" );
        m_RobotShaderSet.slotWorld    = glGetUniformLocation( m_RobotShaderSet.hShader,  "g_matWorld" );
        m_RobotShaderSet.slotView     = glGetUniformLocation( m_RobotShaderSet.hShader,  "g_matView" );
        m_RobotShaderSet.slotProj     = glGetUniformLocation( m_RobotShaderSet.hShader,  "g_matProj" );
        m_RobotShaderSet.slotBaseTexture     = glGetUniformLocation( m_RobotShaderSet.hShader,  "BaseTexture" );
        m_RobotShaderSet.slotLightingTexture = glGetUniformLocation( m_RobotShaderSet.hShader,  "LightingTexture" );
        m_RobotShaderSet.bIsDecal            = FALSE;
    }

    // Create the shader program for rendering the room mesh
    {
        FRM_VERTEX_ELEMENT VertexLayout[] = 
        {
            { FRM_VERTEX_POSITION,  3, GL_FLOAT, FALSE, 3*sizeof(FLOAT32) },
            { FRM_VERTEX_NORMAL,    3, GL_FLOAT, FALSE, 3*sizeof(FLOAT32) },
            { FRM_VERTEX_TEXCOORD0, 2, GL_FLOAT, FALSE, 2*sizeof(FLOAT32) },
            NULL
        };

        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "In_Pos",    FRM_VERTEX_POSITION },
            { "In_Normal", FRM_VERTEX_NORMAL },
            { "In_Tex0",   FRM_VERTEX_TEXCOORD0 },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/Robot/Shaders/Room.vs",
                                                      "Demos/Robot/Shaders/Room.fs",
                                                      &m_RoomShaderSet.hShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_RoomShaderSet.slotLightDir = glGetUniformLocation( m_RoomShaderSet.hShader,  "g_vLightDir" );
        m_RoomShaderSet.slotMtrlDif  = glGetUniformLocation( m_RoomShaderSet.hShader,  "g_vMaterialDiffuse" );
        m_RoomShaderSet.slotMtrlSpc  = glGetUniformLocation( m_RoomShaderSet.hShader,  "g_vMaterialSpecular" );
        m_RoomShaderSet.slotMtrlAmb  = glGetUniformLocation( m_RoomShaderSet.hShader,  "g_vMaterialAmbient" );
        m_RoomShaderSet.slotMtrlEms  = glGetUniformLocation( m_RoomShaderSet.hShader,  "g_vMaterialEmissive" );
        m_RoomShaderSet.slotNormal   = glGetUniformLocation( m_RoomShaderSet.hShader,  "g_matNormal" );
        m_RoomShaderSet.slotWorld    = glGetUniformLocation( m_RoomShaderSet.hShader,  "g_matWorld" );
        m_RoomShaderSet.slotView     = glGetUniformLocation( m_RoomShaderSet.hShader,  "g_matView" );
        m_RoomShaderSet.slotProj     = glGetUniformLocation( m_RoomShaderSet.hShader,  "g_matProj" );
        m_RoomShaderSet.slotBaseTexture     = glGetUniformLocation( m_RoomShaderSet.hShader,  "BaseTexture" );
        m_RoomShaderSet.slotLightingTexture = glGetUniformLocation( m_RoomShaderSet.hShader,  "LightingTexture" );
        m_RoomShaderSet.bIsDecal            = FALSE;
    }

    // Create the shader program for rendering the tennis ball mesh
    {
        FRM_VERTEX_ELEMENT VertexLayout[] = 
        {
            { FRM_VERTEX_POSITION,  3, GL_FLOAT, FALSE, 3*sizeof(FLOAT32) },
            { FRM_VERTEX_NORMAL,    3, GL_FLOAT, FALSE, 3*sizeof(FLOAT32) },
            { FRM_VERTEX_TEXCOORD0, 2, GL_FLOAT, FALSE, 2*sizeof(FLOAT32) },
            NULL
        };

        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "In_Pos",    FRM_VERTEX_POSITION },
            { "In_Normal", FRM_VERTEX_NORMAL },
            { "In_Tex0",   FRM_VERTEX_TEXCOORD0 },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/Robot/Shaders/TennisBall.vs",
                                                      "Demos/Robot/Shaders/TennisBall.fs",
                                                      &m_TennisBallShaderSet.hShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_TennisBallShaderSet.slotLightDir = glGetUniformLocation( m_TennisBallShaderSet.hShader,  "g_vLightDir" );
        m_TennisBallShaderSet.slotMtrlDif  = glGetUniformLocation( m_TennisBallShaderSet.hShader,  "g_vMaterialDiffuse" );
        m_TennisBallShaderSet.slotMtrlSpc  = glGetUniformLocation( m_TennisBallShaderSet.hShader,  "g_vMaterialSpecular" );
        m_TennisBallShaderSet.slotMtrlAmb  = glGetUniformLocation( m_TennisBallShaderSet.hShader,  "g_vMaterialAmbient" );
        m_TennisBallShaderSet.slotMtrlEms  = glGetUniformLocation( m_TennisBallShaderSet.hShader,  "g_vMaterialEmissive" );
        m_TennisBallShaderSet.slotNormal   = glGetUniformLocation( m_TennisBallShaderSet.hShader,  "g_matNormal" );
        m_TennisBallShaderSet.slotWorld    = glGetUniformLocation( m_TennisBallShaderSet.hShader,  "g_matWorld" );
        m_TennisBallShaderSet.slotView     = glGetUniformLocation( m_TennisBallShaderSet.hShader,  "g_matView" );
        m_TennisBallShaderSet.slotProj     = glGetUniformLocation( m_TennisBallShaderSet.hShader,  "g_matProj" );
        m_TennisBallShaderSet.slotBaseTexture     = glGetUniformLocation( m_TennisBallShaderSet.hShader,  "BaseTexture" );
        m_TennisBallShaderSet.slotLightingTexture = glGetUniformLocation( m_TennisBallShaderSet.hShader,  "LightingTexture" );
        m_TennisBallShaderSet.bIsDecal            = FALSE;
    }
    
    // Create the shader program for rendering the shadow meshes
    {
        FRM_VERTEX_ELEMENT VertexLayout[] = 
        {
            { FRM_VERTEX_POSITION,  3, GL_FLOAT, FALSE, 3*sizeof(FLOAT32) },
            { FRM_VERTEX_NORMAL,    3, GL_FLOAT, FALSE, 3*sizeof(FLOAT32) },
            { FRM_VERTEX_TEXCOORD0, 2, GL_FLOAT, FALSE, 2*sizeof(FLOAT32) },
            NULL
        };

        FRM_SHADER_ATTRIBUTE pShaderAttributes[] = 
        {
            { "In_Pos",    FRM_VERTEX_POSITION },
            { "In_Normal", FRM_VERTEX_NORMAL },
            { "In_Tex0",   FRM_VERTEX_TEXCOORD0 },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/Robot/Shaders/Room.vs",
                                                      "Demos/Robot/Shaders/Room.fs",
                                                      &m_ShadowShaderSet.hShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_ShadowShaderSet.slotLightDir = glGetUniformLocation( m_ShadowShaderSet.hShader,  "g_vLightDir" );
        m_ShadowShaderSet.slotMtrlDif  = glGetUniformLocation( m_ShadowShaderSet.hShader,  "g_vMaterialDiffuse" );
        m_ShadowShaderSet.slotMtrlSpc  = glGetUniformLocation( m_ShadowShaderSet.hShader,  "g_vMaterialSpecular" );
        m_ShadowShaderSet.slotMtrlAmb  = glGetUniformLocation( m_ShadowShaderSet.hShader,  "g_vMaterialAmbient" );
        m_ShadowShaderSet.slotMtrlEms  = glGetUniformLocation( m_ShadowShaderSet.hShader,  "g_vMaterialEmissive" );
        m_ShadowShaderSet.slotNormal   = glGetUniformLocation( m_ShadowShaderSet.hShader,  "g_matNormal" );
        m_ShadowShaderSet.slotWorld    = glGetUniformLocation( m_ShadowShaderSet.hShader,  "g_matWorld" );
        m_ShadowShaderSet.slotView     = glGetUniformLocation( m_ShadowShaderSet.hShader,  "g_matView" );
        m_ShadowShaderSet.slotProj     = glGetUniformLocation( m_ShadowShaderSet.hShader,  "g_matProj" );
        m_ShadowShaderSet.slotBaseTexture     = glGetUniformLocation( m_ShadowShaderSet.hShader,  "BaseTexture" );
        m_ShadowShaderSet.slotLightingTexture = glGetUniformLocation( m_ShadowShaderSet.hShader,  "LightingTexture" );
        m_ShadowShaderSet.bIsDecal            = TRUE;
    }

    // Create the shader program for rendering the overlay
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        { 
            { "g_vVertex", 0 }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Demos/Robot/Shaders/Overlay.vs",
                                                      "Demos/Robot/Shaders/Overlay.fs",
                                                      &m_hOverlayShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hScreenSizeLoc = glGetUniformLocation( m_hOverlayShader, "g_vScreenSize" );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: LoadMesh()
// Desc: Small helper function to load a mesh
//--------------------------------------------------------------------------------------
BOOL LoadMesh( const CHAR* strFileName, CFrmMesh* pMesh, CFrmPackedResourceGLES* pResource )
{
    // Load the mesh
    if( FALSE == pMesh->Load( strFileName ) )
    {
        FrmLogMessage( "ERROR: Couldn't load ", strFileName, "\n" );
        return FALSE;
    }

    // Initialize the drawable buffers
    if( FALSE == pMesh->MakeDrawable( pResource ) )
        return FALSE;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize the sample
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Seed the random number generator
    FrmSrand( (UINT32)( 65535.0f * m_Timer.GetTime() ) );

    // Create a log file, to debug initialization problems
    FrmLogMessage( "Robot Sample Log File\n" );
    FrmLogMessage( "-------------------\n" );

    // Create the font
    if( FALSE == m_Font.Create( "Demos/Robot/Fonts/RobotFont16.pak" ) )
    {
        FrmLogMessage( "ERROR: Couldn't load font\n" );
        return FALSE;
    }
	

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Demos/Robot/Textures/Textures.pak" ) )
    {
        FrmLogMessage( "ERROR: Couldn't load texture resource\n" );
        return FALSE;
    }

    m_pLogoTexture     = resource.GetTexture( "Logo" );
    m_pWhiteTexture    = resource.GetTexture( "White" );
    m_pSilverSphereMap = resource.GetTexture( "SphereMap_Silver.png" );
    m_pRedSphereMap    = resource.GetTexture( "SphereMap_Red.png" );

    // Create the shaders
    if( FALSE == InitShaders() )
    {
        FrmLogMessage( "ERROR: InitShaders() failed\n" );
        return FALSE;
    }

    // Read the inidividual meshes from the .mesh files
    {
        // Use a helper function to load the meshes
        if( FALSE == LoadMesh( "Demos/Robot/Meshes/Bedroom.mesh", &m_Meshes[ROOM_ROOT], &resource ) )
            return FALSE;
        if( FALSE == LoadMesh( "Demos/Robot/Meshes/TennisBall.mesh", &m_Meshes[TENNISBALL_ROOT], &resource ) )
            return FALSE;
        if( FALSE == LoadMesh( "Demos/Robot/Meshes/Robot.mesh", &m_Meshes[ROBOT_ROOT], &resource ) )
            return FALSE;
        if( FALSE == LoadMesh( "Demos/Robot/Meshes/RobotShadow.mesh", &m_Meshes[ROBOTSHADOW_ROOT], &resource ) )
            return FALSE;
        if( FALSE == LoadMesh( "Demos/Robot/Meshes/BallShadow.mesh", &m_Meshes[BALLSHADOW_ROOT], &resource ) )
            return FALSE;
    }

    // Link the meshes in one big frame hierarchy for the whole scene
    {
        m_pScene = new FRM_MESH_FRAME[NUM_ROOT_FRAMES];
        FrmMemset( m_pScene, 0, NUM_ROOT_FRAMES * sizeof(FRM_MESH_FRAME) );
        
        FrmStrcpy( m_pScene[SCENE_ROOT].m_strName, "Scene" );
        m_pScene[SCENE_ROOT].m_matTransform       = FrmMatrixIdentity();
        m_pScene[SCENE_ROOT].m_pChild             = &m_pScene[ROOM_ROOT];
        m_pScene[ROOM_ROOT].m_pNext               = NULL;

        FrmStrcpy( m_pScene[ROOM_ROOT].m_strName, "Room_Root" );
        m_pScene[ROOM_ROOT].m_matTransform        = FrmMatrixIdentity();
        m_pScene[ROOM_ROOT].m_pChild              = m_Meshes[ROOM_ROOT].m_pFrames;
        m_pScene[ROOM_ROOT].m_pNext               = &m_pScene[TENNISBALL_ROOT];

        FrmStrcpy( m_pScene[TENNISBALL_ROOT].m_strName, "TennisBall_Root" );
        m_pScene[TENNISBALL_ROOT].m_matTransform  = FrmMatrixIdentity();
        m_pScene[TENNISBALL_ROOT].m_pChild        = m_Meshes[TENNISBALL_ROOT].m_pFrames;
        m_pScene[TENNISBALL_ROOT].m_pNext         = &m_pScene[BALLSHADOW_ROOT];

        FrmStrcpy( m_pScene[BALLSHADOW_ROOT].m_strName, "BallShadow_Root" );
        m_pScene[BALLSHADOW_ROOT].m_matTransform  = FrmMatrixIdentity();
        m_pScene[BALLSHADOW_ROOT].m_pChild        = m_Meshes[BALLSHADOW_ROOT].m_pFrames;
        m_pScene[BALLSHADOW_ROOT].m_pNext         = &m_pScene[ROBOT_ROOT];

        FrmStrcpy( m_pScene[ROBOT_ROOT].m_strName, "Robot_Root" );
        m_pScene[ROBOT_ROOT].m_matTransform       = FrmMatrixIdentity();
        m_pScene[ROBOT_ROOT].m_pChild             = m_Meshes[ROBOT_ROOT].m_pFrames;
        m_pScene[ROBOT_ROOT].m_pNext              = &m_pScene[ROBOTSHADOW_ROOT];

        FrmStrcpy( m_pScene[ROBOTSHADOW_ROOT].m_strName, "RobotShadow_Root" );
        m_pScene[ROBOTSHADOW_ROOT].m_matTransform = FrmMatrixIdentity();
        m_pScene[ROBOTSHADOW_ROOT].m_pChild       = m_Meshes[ROBOTSHADOW_ROOT].m_pFrames;
        m_pScene[ROBOTSHADOW_ROOT].m_pNext        = NULL;
    }

    // Bind the meshes to the appropriate shader sets, using the m_nReserved field as a place
    // to store the pointer to the shader data
    {
        // All the room meshes use the m_RoomShaderSet
        for( UINT32 i=0; i<m_Meshes[ROOM_ROOT].m_nNumFrames; i++ )
        {
            if( m_Meshes[ROOM_ROOT].m_pFrames[i].m_pMesh )
                m_Meshes[ROOM_ROOT].m_pFrames[i].m_pMesh->m_Reserved = (UINT32)&m_RoomShaderSet;
        }

        // All the tennis ball meshes use the m_TennisBallShaderSet
        for( UINT32 i=0; i<m_Meshes[TENNISBALL_ROOT].m_nNumFrames; i++ )
        {
            if( m_Meshes[TENNISBALL_ROOT].m_pFrames[i].m_pMesh )
                m_Meshes[TENNISBALL_ROOT].m_pFrames[i].m_pMesh->m_Reserved = (UINT32)&m_TennisBallShaderSet;
        }

        // The shiny robot meshes use the m_RobotShaderSet
        // The dull robot meshes use the m_RoomShaderSet
        for( UINT32 i=0; i<m_Meshes[ROBOT_ROOT].m_nNumFrames; i++ )
        {
            if( m_Meshes[ROBOT_ROOT].m_pFrames[i].m_pMesh )
            {
                if( !FrmStricmp( m_Meshes[ROBOT_ROOT].m_pFrames[i].m_strName, "Robot_DullParts" ) )
                    m_Meshes[ROBOT_ROOT].m_pFrames[i].m_pMesh->m_Reserved = (UINT32)&m_RoomShaderSet;
                else if( !FrmStricmp( m_Meshes[ROBOT_ROOT].m_pFrames[i].m_strName, "Robot_Head" ) )
                    m_Meshes[ROBOT_ROOT].m_pFrames[i].m_pMesh->m_Reserved = (UINT32)&m_RoomShaderSet;
                else
                    m_Meshes[ROBOT_ROOT].m_pFrames[i].m_pMesh->m_Reserved = (UINT32)&m_RobotShaderSet;
            }
        }

        // The robot shadow mesh uses the m_ShadowShaderSet
        for( UINT32 i=0; i<m_Meshes[ROBOTSHADOW_ROOT].m_nNumFrames; i++ )
        {
            if( m_Meshes[ROBOTSHADOW_ROOT].m_pFrames[i].m_pMesh )
                m_Meshes[ROBOTSHADOW_ROOT].m_pFrames[i].m_pMesh->m_Reserved = (UINT32)&m_ShadowShaderSet;
        }

        // The ball shadow mesh uses the m_ShadowShaderSet
        for( UINT32 i=0; i<m_Meshes[BALLSHADOW_ROOT].m_nNumFrames; i++ )
        {
            if( m_Meshes[BALLSHADOW_ROOT].m_pFrames[i].m_pMesh )
                m_Meshes[BALLSHADOW_ROOT].m_pFrames[i].m_pMesh->m_Reserved = (UINT32)&m_ShadowShaderSet;
        }
    }

    // Hide the tennis ball (make it transparent) until it's needed
    for( UINT32 i=0; i<m_Meshes[TENNISBALL_ROOT].m_nNumFrames; i++ )
    {
        if( m_Meshes[TENNISBALL_ROOT].m_pFrames[i].m_pMesh )
        {
            m_Meshes[TENNISBALL_ROOT].m_pFrames[i].m_pMesh->m_pSubsets[0].m_vDiffuseColor.a = 0.0f;
            m_Meshes[BALLSHADOW_ROOT].m_pFrames[i].m_pMesh->m_pSubsets[0].m_vDiffuseColor.a = 0.0f;
        }
    }

    // Read the robot animations from a file
    {
        if( FALSE == FrmReadAnimation( "Demos/Robot/Meshes/RobotWalkCycle.anim", &m_pRobotWalkAnimation ) )
        {
            FrmLogMessage( "ERROR: Couldn't load RobotWalkCycle.anim\n" );
            return FALSE;
        }
        if( FALSE == FrmReadAnimation( "Demos/Robot/Meshes/RobotMadCycle.anim",  &m_pRobotBeserkAnimation ) )
        {
            FrmLogMessage( "ERROR: Couldn't load RobotMadCycle.anim\n" );
            return FALSE;
        }

        // Remove ground-plane translations from the walk cycle
        for( UINT32 i=0; i<m_pRobotWalkAnimation->m_pAnimations[0].m_nNumKeys; i++ )
        {
            m_pRobotWalkAnimation->m_pAnimations[0].m_pKeyValues[i].m_vTranslate.x = 0.0f;
            m_pRobotWalkAnimation->m_pAnimations[0].m_pKeyValues[i].m_vTranslate.z = 0.0f;
        }

        // Create an animation blender
		// corresponds to BindAnimationDataToMesh
        FrmCreateAnimationBlender( m_pRobotWalkAnimation->m_nNumAnimations, m_pRobotWalkAnimation, 
                                   m_pScene[ROBOT_ROOT].m_pChild, m_Meshes[ROBOT_ROOT].m_nNumFrames, 
                                   &m_pAnimBlender );

        // The robot starts out walking
        m_pRobotCurrentAnimation = m_pRobotWalkAnimation;
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: Handle a resize of the display
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    // Initialize the viewport
    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4, fAspect, 0.1f, 100.0f );
    m_matProj.M(0,0) = -m_matProj.M(0,0);

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_pLogoTexture )     m_pLogoTexture->Release();
    if( m_pWhiteTexture )    m_pWhiteTexture->Release();
    if( m_pSilverSphereMap ) m_pSilverSphereMap->Release();
    if( m_pRedSphereMap )    m_pRedSphereMap->Release();

    if( m_RobotShaderSet.hShader )      glDeleteProgram( m_RobotShaderSet.hShader );
    if( m_RoomShaderSet.hShader )       glDeleteProgram( m_RoomShaderSet.hShader );
    if( m_TennisBallShaderSet.hShader ) glDeleteProgram( m_TennisBallShaderSet.hShader );
    if( m_hOverlayShader )              glDeleteProgram( m_hOverlayShader );

    for( UINT32 i=0; i<NUM_ROOT_FRAMES; i++ )
        m_Meshes[i].Destroy();
    delete[] m_pScene;

    delete m_pRobotWalkAnimation;
    delete m_pRobotBeserkAnimation;
    delete m_pAnimBlender;
}


//--------------------------------------------------------------------------------------
// Name: GetRobotTransform()
// Desc: Gets the transform to position/orient the robot for the specifed time. The
//       robot walks to/fro along a path, so his position is predetermistic (which is
//       important so we can calculate future collisions with the tennis ball).
//--------------------------------------------------------------------------------------
FRMMATRIX4X4 GetRobotTransform( FLOAT32 fTime )
{
    const FLOAT32 fStepSize          = 0.06f;
    const FLOAT32 fNumStraightSteps  = (FLOAT32)5;
    const FLOAT32 fNumTurningSteps   = (FLOAT32)5;
    const FLOAT32 fTimePerStep       = 0.65f;
    const FLOAT32 fStraightWalkTime  = fNumStraightSteps * fTimePerStep;
    const FLOAT32 fTurningWalkTime   = fNumTurningSteps  * fTimePerStep;
    const FLOAT32 fTotalSequenceTime = 2 * fStraightWalkTime + 2 * fTurningWalkTime;
    const FLOAT32 fTurnRadius        = fStepSize * fNumTurningSteps / FRM_PI;

    while( fTime > fTotalSequenceTime )
        fTime -= fTotalSequenceTime;

    FRMVECTOR3 pos( 0.0f, 0.0f, 0.0f );
    FLOAT32    fAngle = 0.0f;

    while( 1 )
    {
        // Walk straight (to)
        if( fTime < fStraightWalkTime )
        {
            pos.x += 0.0f;
            pos.y += 0.0f;
            pos.z += -fStepSize * fNumStraightSteps * ( fTime / fStraightWalkTime );
            break;
        }
        pos.x += 0.0f;
        pos.y += 0.0f;
        pos.z += -fStepSize * fNumStraightSteps;
        fTime -= fStraightWalkTime;

        // Turn (to)
        if( fTime < fTurningWalkTime )
        {
            fAngle += FRM_PI * fTime / fTurningWalkTime;
            pos.x += -fTurnRadius * ( FrmCos( fAngle ) - 1.0f );
            pos.y += 0.0f;
            pos.z += -fTurnRadius * FrmSin( fAngle );
            break;
        }
        fAngle += FRM_PI;
        pos.x += fTurnRadius * 2.0f;
        pos.y += 0.0f;
        pos.z += 0.0f;
        fTime -= fTurningWalkTime;

        // Walk straight (fro)
        if( fTime < fStraightWalkTime )
        {
            pos.x += 0.0f;
            pos.y += 0.0f;
            pos.z += +fStepSize * fNumStraightSteps * ( fTime / fStraightWalkTime );
            break;
        }
        pos.x += 0.0f;
        pos.y += 0.0f;
        pos.z += +fStepSize * fNumStraightSteps;
        fTime -= fStraightWalkTime;

        // Turn (fro)
        if( fTime < fTurningWalkTime )
        {
            fAngle += FRM_PI * fTime / fTurningWalkTime;
            pos.x += -fTurnRadius * ( FrmCos( fAngle ) + 1.0f );
            pos.y += 0.0f;
            pos.z += -fTurnRadius * FrmSin( fAngle );
            break;
        }
        fAngle += FRM_PI;
        pos.x += fTurnRadius * 2.0f;
        pos.y += 0.0f;
        pos.z += 0.0f;
        fTime -= fTurningWalkTime;
    }

    // Raise robot slightly to prevent it from intersecting it's shadow
    pos.y = 0.001f;
   
    FRMMATRIX4X4 srcA = FrmMatrixRotate(-fAngle,0.0f,1.0f,0.0f);
    FRMMATRIX4X4 srcB = FrmMatrixTranslate( pos); 

    return FrmMatrixMultiply( srcA, srcB );
}


//--------------------------------------------------------------------------------------
// Name: UpdateFrameMatrices()
// Desc: After the FrmAnimation_SetTime() function updates the m_matTransform of each
//	     frame, this function is called to rebuild the the combined matrices of the
//	     frame hierarchy.
//--------------------------------------------------------------------------------------
VOID UpdateFrameMatrices( FRM_MESH_FRAME* pFrame, FRMMATRIX4X4& matParent )
{
    pFrame->m_matCombined = FrmMatrixMultiply( pFrame->m_matTransform, matParent );

    if( pFrame->m_pNext )
        UpdateFrameMatrices( pFrame->m_pNext, matParent );

    if( pFrame->m_pChild )
        UpdateFrameMatrices( pFrame->m_pChild, pFrame->m_matCombined );
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    static FLOAT32 fRobotWalkSequenceTime = 0.0f;
    static UINT32  nRobotState = WALKING_FORWARD;
    static UINT32  nNumStepsTaken = 0;
    static UINT32  nNumRotationStepsTaken = 0;
    static UINT32  nPeriodicAnimTime = 0;

    static BOOL bTennisBallInMotion = FALSE;
    BOOL bTennisBallLaunched = FALSE;

    // Get the current time
    static FLOAT32 fRecordTime;
    static FLOAT32 fTime;
    static FLOAT32 fElapsedTime;
    fTime        = m_Timer.GetFrameTime();
    fElapsedTime = m_Timer.GetFrameElapsedTime();

    // Handle input
    {
        UINT32 nButtons;
        UINT32 nPressedButtons;
        FrmGetInput( &m_Input, &nButtons, &nPressedButtons );
        UINT32 pntState;
	    FRMVECTOR2 newMousePose;
	    m_Input.GetPointerState( &pntState, &newMousePose );
        // Launch the tennis ball when the user presses the '0' key
        if( nButtons & INPUT_KEY_0 || (pntState & FRM_INPUT::POINTER_RELEASED))
        {
            if( bTennisBallInMotion == FALSE && nRobotState != GOING_BESERK )
                bTennisBallLaunched = TRUE;
        }

    }

    // When the tennis ball is launched, first predict a collision point based on the robot's current
    // path, and then use a random launch point that is 2.0 seconds away from the collision point.
    // Note: A tennis ball is 2.5", or 0.0635 meters (It's radius is 0.03175 meters)
    static FRMVECTOR3 vCollisionPoint;
    static FRMVECTOR3 vStartPoint;
    static FRMVECTOR3 vTennisBallRotationAxis;
    static FLOAT32    fLaunchTime;
    static FLOAT32    fRollTime;
    static FLOAT32    fCameraAnimTime = 0.0f;
    static FRMVECTOR3 vForwardDirection;

    if( bTennisBallLaunched == TRUE )
    {
        FLOAT32 fTotalAnimTime   = m_pRobotCurrentAnimation->m_nPeriodInTicks / (FLOAT32)FRM_ANIMATION_TICKS_PER_SEC;
        FLOAT32 fCurrentAnimTime = nPeriodicAnimTime / (FLOAT32) FRM_ANIMATION_TICKS_PER_SEC;
        FLOAT32 fAnimTimeLeftInCurrentCycle = fTotalAnimTime - ( fCurrentAnimTime + fElapsedTime );

        // Simulate where the robot will be in 1.2 seconds, so we can decide where to
        // aim the tennis ball
        fRollTime = 1.2f + fAnimTimeLeftInCurrentCycle;
        FRMMATRIX4X4 matFutureTransform = GetRobotTransform( fRobotWalkSequenceTime + fRollTime + 0.0f );
        vCollisionPoint.x = matFutureTransform.M(3,0);
        vCollisionPoint.y = matFutureTransform.M(3,1);
        vCollisionPoint.z = matFutureTransform.M(3,2);

        FRMVECTOR3 tempA = FRMVECTOR3(0.0f,0.0f,0.0f);
        FRMVECTOR3 tempB = FRMVECTOR3(0.0f,0.0f,-1.0f);

        FRMVECTOR3 v0 = FrmVector3TransformCoord( tempA, matFutureTransform );
        FRMVECTOR3 v1 = FrmVector3TransformCoord( tempB, matFutureTransform );
        FRMVECTOR3 v2 = v1 - v0;
        vForwardDirection = FrmVector3Normalize( v2 );
        fCameraAnimTime = 0.0001f;

        // Start the tennis ball 0.9 meters away in some random backfacing location
        FLOAT32 fAngle = 0.375f * FRM_PI + 0.250f * FRM_PI * ( 2.0f * FrmPow( 2.7128f, -5.0f * FrmRand() - 1.0f ));
        if( FrmRand() > 0.5f )
            fAngle = -fAngle;
        vStartPoint = FRMVECTOR3( 0.0f, 0.0f, 0.9f );

        FRMMATRIX4X4 matA = FrmMatrixRotate (fAngle, 0.0f, 1.0f, 0.0f);
        vStartPoint = FrmVector3TransformCoord( vStartPoint, matA );
        vStartPoint = FrmVector3TransformCoord( vStartPoint, matFutureTransform );

        // Back off a tad so that objects collide not at their centers
        FRMVECTOR3 vVector = vCollisionPoint - vStartPoint;
        vCollisionPoint -= 0.070f * FrmVector3Normalize( vVector );
        
        fLaunchTime = fTime;

        FRMVECTOR3 yVec = FRMVECTOR3(0,1,0);
        FRMVECTOR3 vVector2 = vCollisionPoint - vStartPoint;
        vTennisBallRotationAxis = FrmVector3Cross( yVec, vVector2 );

        bTennisBallInMotion = TRUE;
        bTennisBallLaunched = FALSE;
    }

    // Update the tennis ball
    if( bTennisBallInMotion == TRUE )
    {
        if( nRobotState == WALKING_FORWARD )
        {
            FLOAT32 s = ( fTime - fLaunchTime ) / fRollTime;
            FRMVECTOR3 vTennisBallPos = FrmVector3Lerp( vStartPoint, vCollisionPoint, s );

            m_pScene[TENNISBALL_ROOT].m_matTransform = FrmMatrixRotate( 2.0f * FRM_PI * 2 * s, vTennisBallRotationAxis );
            m_pScene[TENNISBALL_ROOT].m_matTransform.M(3,0) = vTennisBallPos.x;
            m_pScene[TENNISBALL_ROOT].m_matTransform.M(3,1) = 0.03125f;
            m_pScene[TENNISBALL_ROOT].m_matTransform.M(3,2) = vTennisBallPos.z;
            m_pScene[TENNISBALL_ROOT].m_pChild->m_matTransform = FrmMatrixIdentity();
            m_pScene[TENNISBALL_ROOT].m_pChild->m_pMesh->m_pSubsets[0].m_vDiffuseColor.a = FrmPow( s, 0.2f );

            m_pScene[BALLSHADOW_ROOT].m_matTransform = FrmMatrixTranslate( vTennisBallPos.x, 0.0f, vTennisBallPos.z );
            m_pScene[BALLSHADOW_ROOT].m_pChild->m_matTransform = FrmMatrixIdentity();
            m_pScene[BALLSHADOW_ROOT].m_pChild->m_pMesh->m_pSubsets[0].m_vDiffuseColor.a = 0.7f * FrmPow( s, 0.2f );

            if( s >= 1.0f )
            {
                nRobotState = GOING_BESERK;
                m_pRobotCurrentAnimation = m_pRobotBeserkAnimation;
                nPeriodicAnimTime = 0;
                
                fLaunchTime = fTime;
                vCollisionPoint = vCollisionPoint + 0.5f * ( vStartPoint - vCollisionPoint );
                vStartPoint = vTennisBallPos;
                vTennisBallRotationAxis = -vTennisBallRotationAxis;
            }
        }
        else // nRobotState == GOING_BESERK
        {
            FLOAT32 s = ( fTime - fLaunchTime ) / fRollTime;
            FRMVECTOR3 vTennisBallPos = FrmVector3Lerp( vStartPoint, vCollisionPoint, s );

            m_pScene[TENNISBALL_ROOT].m_matTransform = FrmMatrixRotate( 2.0f * FRM_PI * 2 * s, vTennisBallRotationAxis );
            m_pScene[TENNISBALL_ROOT].m_matTransform.M(3,0) = vTennisBallPos.x;
            m_pScene[TENNISBALL_ROOT].m_matTransform.M(3,1) = 0.03125f;
            m_pScene[TENNISBALL_ROOT].m_matTransform.M(3,2) = vTennisBallPos.z;
            m_pScene[TENNISBALL_ROOT].m_pChild->m_matTransform = FrmMatrixIdentity();

            m_pScene[BALLSHADOW_ROOT].m_matTransform = FrmMatrixTranslate( vTennisBallPos.x, 0.0f, vTennisBallPos.z );
            m_pScene[BALLSHADOW_ROOT].m_pChild->m_matTransform = FrmMatrixIdentity();

            if( s >= 2.0f )
            {
                m_pScene[TENNISBALL_ROOT].m_pChild->m_pMesh->m_pSubsets[0].m_vDiffuseColor.a = 0.0f;
                m_pScene[BALLSHADOW_ROOT].m_pChild->m_pMesh->m_pSubsets[0].m_vDiffuseColor.a = 0.0f;
                bTennisBallInMotion = FALSE;
            }
            else
            {
                m_pScene[TENNISBALL_ROOT].m_pChild->m_pMesh->m_pSubsets[0].m_vDiffuseColor.a = FrmPow( 1.0f - s/2, 0.5f );
                m_pScene[BALLSHADOW_ROOT].m_pChild->m_pMesh->m_pSubsets[0].m_vDiffuseColor.a = 0.7f * FrmPow( 1.0f - s/2, 0.5f );
            }
        }
    }

    // Animate the system
    if( m_pRobotCurrentAnimation && m_pAnimBlender )
    {
        // Update the animation time
        nPeriodicAnimTime += (UINT32)( fElapsedTime * FRM_ANIMATION_TICKS_PER_SEC );
        
        // Check if the animation time was exceeded
        if( nPeriodicAnimTime >= m_pRobotCurrentAnimation->m_nPeriodInTicks )
        {

            if( nRobotState == WALKING_FORWARD )
            {
                // Loop the walk cycle animation
                nPeriodicAnimTime -= m_pRobotCurrentAnimation->m_nPeriodInTicks;
            }
            else if( nRobotState == GOING_BESERK )
            {
                // The beserk animation ended, so go back to the walk cycle
                nRobotState = WALKING_FORWARD;
                m_pRobotCurrentAnimation = m_pRobotWalkAnimation;
                nPeriodicAnimTime -= m_pRobotCurrentAnimation->m_nPeriodInTicks;
            }
        }

        // Update the robot position/orientation. The GetRobotTransform() computes
        // the position/orientation predetermistically based on the time.
        if( nRobotState == WALKING_FORWARD )
            fRobotWalkSequenceTime += fElapsedTime;
        m_pScene[ROBOT_ROOT].m_matTransform = GetRobotTransform( fRobotWalkSequenceTime );

        // Compute the current animation values (one set of data for each frame)        
        m_pAnimBlender->SetAnimation( m_pRobotCurrentAnimation , nPeriodicAnimTime);        
    }

    // Updating the frame matrices hierarchically with the previously computed animation values
    FRMMATRIX4X4 matI = FrmMatrixIdentity();
    UpdateFrameMatrices( m_pScene, matI );

    // Position the shadow directly underneath the robot
    m_pScene[ROBOTSHADOW_ROOT].m_pChild->m_matCombined = m_pScene[ROBOT_ROOT].m_pChild->m_matCombined;
    m_pScene[ROBOTSHADOW_ROOT].m_pChild->m_matCombined.M(3,1) = 0.0f;

    // Animate the camera
    {
        FRMVECTOR3 zeroVec = FRMVECTOR3(0.0f,0.0f,0.0f);
        FRMVECTOR3 vRobotPos = FrmVector3TransformCoord( zeroVec, m_pScene[ROBOT_ROOT].m_pChild->m_matCombined );
        FRMVECTOR3 vAt  = FRMVECTOR3( vRobotPos.x, 0.1f, vRobotPos.z );
        FRMVECTOR3 vEye = vAt + FRMVECTOR3( 0.03f, 0.06f, 0.24f );
        FRMVECTOR3 vUp  = FRMVECTOR3( 0, 1, 0 );

        vEye = FRMVECTOR3( -0.2f, 0.06f, 0.0f );

        // Transition the camera differently before the collision and after the beserk animation
        if( fCameraAnimTime > 0.0f )
        {
            fCameraAnimTime += fElapsedTime;
            if( fCameraAnimTime > 7.0f )
                fCameraAnimTime = 0.0f;

            if( fCameraAnimTime > 1.1f )
            {
                FLOAT32 s = FrmSin(FRM_PI*fCameraAnimTime/8.0f) * FrmSin(FRM_PI*fCameraAnimTime/7.0f);
                FRMVECTOR3 vEye1 = vAt + 0.2f * vForwardDirection;
                vAt.y   = 0.06f;
                vEye1.y = 0.10f;
                vEye = FrmVector3Lerp( vEye, vEye1, 1.0f );
            }
        }

        m_matView = FrmMatrixLookAtRH( vEye, vAt, vUp );
    }

    // Set the light direction
    FRMVECTOR3 yzVec = FRMVECTOR3(0,1,1);
    m_vLightDir = FrmVector3Normalize( yzVec);
}


//--------------------------------------------------------------------------------------
// Name: RenderMesh()
// Desc: Render the contents of a mesh
//--------------------------------------------------------------------------------------
VOID CSample::RenderMesh( FRM_MESH* pMesh, FRMMATRIX4X4& matCurrentMatrixSet, BOOL bAlphaPass )
{
    // The m_nReserved field is used to store a pointer to the shader set
    SHADER_SET* pShaderSet = (SHADER_SET*)pMesh->m_Reserved;
    {
        glUseProgram( pShaderSet->hShader );
        glUniform3fv( pShaderSet->slotLightDir, 1, (FLOAT32*)&m_vLightDir ); // light
        glUniformMatrix4fv( pShaderSet->slotView, 1, FALSE, (FLOAT32*)&m_matView );
        glUniformMatrix4fv( pShaderSet->slotProj, 1, FALSE, (FLOAT32*)&m_matProj );

        // Turn off depth for decals
        if( pShaderSet->bIsDecal )
            glDepthMask( GL_FALSE );
    }

	// Clear out vertex attribute arrays BEFORE setting up vertex layouts
	FrmClearVertexAttributeArrays();

    // Bind the mesh's buffers and vertex layout
    FrmSetVertexBuffer( pMesh->m_hVertexBuffer );
    FrmSetVertexLayout( pMesh->m_VertexLayout, pMesh->m_nVertexSize );
    
    // Render each subset of the mesh
    for( UINT32 iSubset = 0; iSubset < pMesh->m_nNumSubsets; iSubset++ )
    { 
        FRM_MESH_SUBSET* pSubset = &pMesh->m_pSubsets[iSubset];

        // Set the material properties
        {
            if( bAlphaPass && pSubset->m_vDiffuseColor.a >= 1.0f )
                continue;
            if( !bAlphaPass && pSubset->m_vDiffuseColor.a < 1.0f )
                continue;
            if( pSubset->m_vDiffuseColor.a < 0.01f )
                continue;

            // Set material colors 
            {
                glUniform4fv( pShaderSet->slotMtrlDif, 1, (FLOAT32*)&pSubset->m_vDiffuseColor );  // mtrl diffuse
                glUniform4fv( pShaderSet->slotMtrlSpc, 1, (FLOAT32*)&pSubset->m_vSpecularColor ); // mtrl specular
                glUniform3fv( pShaderSet->slotMtrlAmb, 1, (FLOAT32*)&pSubset->m_vAmbientColor );  // mtrl ambient
                glUniform3fv( pShaderSet->slotMtrlEms, 1, (FLOAT32*)&pSubset->m_vEmissiveColor ); // mtrl emissive
            }

            // Setup the texture(s) of the mesh subset
            FrmSetTexture( 0, m_pWhiteTexture->m_hTextureHandle );
            glUniform1i( pShaderSet->slotBaseTexture, 0 );

            for( UINT32 iTexture = 0; iTexture < pSubset->m_nNumTextures; iTexture++ )
            {
                if( pSubset->m_pTextures[iTexture].m_pTexture )
                    FrmSetTexture( iTexture, pSubset->m_pTextures[iTexture].m_pTexture->m_hTextureHandle );
            }

            // Shiny components of the robot use the silver spheremap, which contains a static
            // reflection of the room
            FrmSetTexture( 1, m_pSilverSphereMap->m_hTextureHandle );
            glUniform1i( pShaderSet->slotLightingTexture, 1 );
        }

        // Set the matrix transforms. The robot uses a simple, non-skinning vertex shader
        {
            FRMMATRIX4X4 matWorld   = matCurrentMatrixSet;
            FRMMATRIX3X3 matNormal  = FrmMatrixNormal( matWorld );
            glUniformMatrix3fv( pShaderSet->slotNormal, 1, FALSE, (FLOAT32*)&matNormal );
            glUniformMatrix4fv( pShaderSet->slotWorld,  1, FALSE, (FLOAT32*)&matWorld );
        }

		// Draw the mesh
		FrmSetIndexBuffer(pMesh->m_hIndexBuffer);		
		FrmDrawIndexedVertices(pSubset->m_nPrimType, pSubset->m_nNumIndices, pMesh->m_nIndexSize, pSubset->m_nIndexOffset );		
    }

    // Restore states
    glDepthMask( GL_TRUE );
    FrmSetVertexBuffer( NULL );
}


//--------------------------------------------------------------------------------------
// Name: RenderSceneHierarchy()
// Desc: Render the scene, which is a tree hierarchy of mesh frames.
//--------------------------------------------------------------------------------------
VOID CSample::RenderSceneHierarchy( FRM_MESH_FRAME* pFrame, BOOL bAlphaPass )
{
    if( pFrame->m_pMesh )
        RenderMesh( pFrame->m_pMesh, pFrame->m_matCombined, bAlphaPass );

    if( pFrame->m_pChild )
        RenderSceneHierarchy( pFrame->m_pChild, bAlphaPass );

    if( pFrame->m_pNext )
        RenderSceneHierarchy( pFrame->m_pNext, bAlphaPass );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{	
    // Clear the depth-buffer
    glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    // Set default render states
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Render the scene hierarchy
    {
        // Draw the opaque pass of the scene
        glDisable( GL_BLEND );
        RenderSceneHierarchy( m_pScene, FRM_MESH_OPAQUE );
        
        // Draw the transparent pass of the scene
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
        RenderSceneHierarchy( m_pScene, FRM_MESH_ALPHA );
    }

    // Update the frame counter
    m_Timer.MarkFrame();

    // Draw the user interface
    m_Font.SetScaleFactors( 1.0f, 1.0f );
    m_Font.DrawText( 5, 0, 0xffffffff, "Robot Demo" );
    m_Font.SetScaleFactors( 0.8f, 0.8f );
    m_Font.DrawText( 5, 30, 0xffffffff, "Press " FRM_FONT_KEY_0 " to throw a tennis ball\nat the robot" );

    // Draw the Logo
    FrmRenderTextureToScreen_GLES( -5, -5, 1.0f, m_pLogoTexture,
                              m_hOverlayShader, m_hScreenSizeLoc );
}

