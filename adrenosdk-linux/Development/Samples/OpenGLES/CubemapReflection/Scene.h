//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H


//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmApplication
{
public:
    CSample( const CHAR* strName );

    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

private:
    BOOL InitShaders();
    VOID GenerateEnvironmentMap();
    VOID RenderMainObject( FRMMATRIX4X4 matModel, FRMMATRIX4X4 matView,
                           FRMMATRIX4X4 matProj );
    VOID RenderSatelliteObject( FRMMATRIX4X4 matModel, FRMMATRIX4X4 matView,
                                FRMMATRIX4X4 matProj );

    CFrmFontGLES          m_Font;
    CFrmTimer             m_Timer;
    CFrmUserInterfaceGLES m_UserInterface;

    GLuint                m_hMainShader;
    GLuint                m_hSatelliteShader;

    CFrmTexture*          m_pSatelliteTexture;
    CFrmTexture*          m_pPlaqueTexture;
    CFrmCubeMap*          m_pEnvironmentMap;

    GLuint                m_hReflectionMap;
    GLuint                m_hReflectionMaps[6];

    CFrmTexture*          m_pLogoTexture;

    CFrmMesh              m_MainObject;
    CFrmMesh              m_SatelliteObject;

    FRMMATRIX4X4          m_matMainObject;
    FRMMATRIX4X4          m_matSatelliteObject[2];
    FRMVECTOR3            m_vSatellitePos[2];
    FRMVECTOR3            m_vLightPosWorld;

    GLuint		          m_hFBODepthbufferID;
    GLuint		          m_hFBOFramebufferID;

    GLint                 m_locMainModelViewProjMatrix;
    GLint                 m_locMainModelViewMatrix;
    GLint                 m_locMainNormalMatrix;
    GLint                 m_locMainRefMap;
    GLint                 m_locMainEnvMap;
    GLint                 m_locMainLightPos;

    GLint                 m_locSatModelViewProjMatrix;
    GLint                 m_locSatModelViewMatrix;
    GLint                 m_locSatNormalMatrix;
    GLint                 m_locSatTex;
    GLint                 m_locSatLightPos;

    FRMMATRIX4X4 m_matProj;
    FRMMATRIX4X4 m_matView;
};


#endif // SCENE_H
