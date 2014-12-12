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
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample( const CHAR* strName );

protected:
    BOOL InitShaders();
    VOID RenderScene( const FRMVECTOR3& vLightPos, FLOAT32 fAmbientLighting );

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    // Room mesh
    CFrmMesh      m_RoomMesh;
    INT32         m_hNormalMappingShader;
    INT32         m_hDefaultLightingShader;
    INT32         m_LightPosLoc[2];
    INT32         m_CamPosLoc[2];
    INT32         m_AmbientLoc[2];
    INT32         m_MVPLoc[2];
    INT32         m_BaseLoc[2];
    INT32         m_BumpLoc[2];

    // Camera and projection
    FLOAT32       m_fCameraSplinePos;
    FRMVECTOR3    m_vCameraPos;
    FRMMATRIX4X4  m_matProj;
    FRMMATRIX4X4  m_matModelViewProj;

    // Options
    BOOL          m_bUseNormalMapping;
    FLOAT32       m_fCameraSpeed;
    BOOL          m_bCameraPaused;

    // Logo
    CFrmTexture*  m_pLogoTexture;
};


#endif // SCENE_H
