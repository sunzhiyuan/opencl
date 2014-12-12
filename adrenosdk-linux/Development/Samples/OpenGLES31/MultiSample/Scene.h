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
    virtual VOID Destroy();
    virtual BOOL Initialize();
    virtual VOID Render();
    virtual BOOL Resize();
    virtual VOID Update();

    CSample( const CHAR* strName );

private:
    BOOL CheckFrameBufferStatus();
    void DeleteTextures();
    void InitOffScreenFramebuffer(unsigned int n_texture_to_use);
    void InitProjectionMatrix();
    BOOL InitShaders();
    BOOL InitTexture();
    BOOL InitVertexArrayObject();
    BOOL InitVertexBufferObject();

    CFrmFontGLES           m_Font;
    CFrmTexture*           m_pLogoTexture;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    BOOL                   m_bRotationOn;
    BOOL                   m_is_frozen;

    UINT32                 m_currentTextureIndex;
    UINT32                 m_fboId;
    UINT32                 m_MultiSampleShaderProgram;
    UINT32                 m_vao_id;
    UINT32                 m_vbo_id;

    GLint                  m_mvpLocation;

    FLOAT32                m_fCurrentTime;
    FLOAT32                m_freeze_time;
    FLOAT32                m_fStartTime;

    FRMMATRIX4X4           m_matProj;
    FRMMATRIX4X4           m_mvp;

    const UINT32           m_cube_indices;
    const UINT32           m_cube_lines;
};


#endif // SCENE_H
