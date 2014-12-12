//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H


// States of the robot: either walking forward, or going beserk
enum
{
    WALKING_FORWARD,
    GOING_BESERK,
};


// Pass for rendering meshes. Opaque meshes are rendered first.
enum
{
    FRM_MESH_OPAQUE,
    FRM_MESH_ALPHA,
};


// Frame IDs for the global scene hierarchy
enum
{ 
    SCENE_ROOT,
    ROOM_ROOT,
    TENNISBALL_ROOT,
    BALLSHADOW_ROOT,
    ROBOT_ROOT,
    ROBOTSHADOW_ROOT,
    NUM_ROOT_FRAMES
};


// A structure to hold a shader with appropriate shader constant slots
struct SHADER_SET
{
    GLuint             hShader;
    GLint              slotLightDir;
    GLint              slotMtrlDif;
    GLint              slotMtrlSpc;
    GLint              slotMtrlAmb;
    GLint              slotMtrlEms;
    GLint              slotNormal;
    GLint              slotWorld;
    GLint              slotView;
    GLint              slotProj;
    GLint              slotBaseTexture;
    GLint              slotLightingTexture;
    BOOL               bIsDecal;
};


//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{	
    CFrmFontGLES            m_Font;                 // Font for rendering text
    CFrmTimer               m_Timer;                // Timer class

    GLuint                  m_hOverlayShader;       // For the logo overlay
    GLuint                  m_hScreenSizeLoc;
    CFrmTexture*            m_pLogoTexture;

    FRMMATRIX4X4            m_matView;              // View matrix
    FRMMATRIX4X4            m_matProj;              // Projection matrix
    FRMVECTOR3              m_vLightDir;            // Light direction

    FRM_MESH_FRAME*         m_pScene;               // The global scene and meshes
    CFrmMesh                m_Meshes[NUM_ROOT_FRAMES];

    SHADER_SET				m_RoomShaderSet;        // Shader sets for rendering the scene
    SHADER_SET				m_RobotShaderSet;
    SHADER_SET				m_TennisBallShaderSet;
    SHADER_SET				m_ShadowShaderSet;

    CFrmTexture*			m_pWhiteTexture;        // Textures for rendering the scene
    CFrmTexture*			m_pSilverSphereMap;
    CFrmTexture*			m_pRedSphereMap;

    FRM_ANIMATION_SET*      m_pRobotWalkAnimation; // Robot animations
    FRM_ANIMATION_SET*      m_pRobotBeserkAnimation;
    FRM_ANIMATION_SET*      m_pRobotCurrentAnimation;
    FRM_ANIMATION_BLENDER*  m_pAnimBlender;

    BOOL InitShaders();
    VOID RenderMesh( FRM_MESH* pMesh, FRMMATRIX4X4& matCurrentMatrixSet, BOOL bAlphaPass );
    VOID RenderSceneHierarchy( FRM_MESH_FRAME* pFrame, BOOL bAlphaPass );

public:
    CSample( const CHAR* strName );

    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();
};


#endif // SCENE_H
