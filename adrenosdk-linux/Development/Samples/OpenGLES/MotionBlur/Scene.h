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

private:
    BOOL InitShaders();
    VOID RenderCube();
    VOID RenderBlurredScene();
    static VOID WrapperRenderBlurredScene( VOID *pScene );

    CFrmFontGLES           m_Font;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    CFrmTexture*    m_pLogoTexture;

    GLuint          m_hTextureShader;
    GLint           m_nTextureAlphaSlot;

    GLuint          m_hConstantShader;
    GLint           m_nConstantMVPSlot;

    FRMMATRIX4X4    m_mPerspectiveMatrix;
    FRMMATRIX4X4    m_matModelViewProj;
    FRMMATRIX4X4    m_matView;

    CMotionBlur*    m_pMotionBlur;

    UINT32          m_nFBOTextureWidth;
    UINT32          m_nFBOTextureHeight;

    BOOL            m_bMotionBlurOn;

    FLOAT32         m_fSpeed;
    FLOAT32         m_fShutterDuration;
    FLOAT32         m_fRotTotal;

    FLOAT32         m_fStartTime;
    FLOAT32         m_fCurrentTime;
};


#endif // SCENE_H
