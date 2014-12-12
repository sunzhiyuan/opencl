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
    GLuint InitShader(GLenum shader_type, const CHAR* shader_body);

    CFrmFontGLES            m_Font;
    CFrmTexture*            m_pLogoTexture;
    CFrmTimer               m_Timer;
    CFrmUserInterfaceGLES   m_UserInterface;

    GLuint                  m_fs_id_1;
    GLuint                  m_fs_id_2;
    GLuint                  m_fs_id_3;
    GLuint                  m_pipeline_object_id;
    GLuint                  m_vao_id;
    GLuint                  m_vs_id;
};


#endif // SCENE_H
