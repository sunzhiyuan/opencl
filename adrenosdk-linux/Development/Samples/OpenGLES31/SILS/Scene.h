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
    void CreateComputeShader(std::string& shaderBody);
    BOOL CreateShader(const GLuint shaderType, const CHAR* shaderSource);
    BOOL CheckFrameBufferStatus();
    BOOL InitFrameBuffer();
    BOOL InitShaders();
    BOOL InitTexture();

    CFrmFontGLES           m_Font;
    CFrmTexture*           m_pLogoTexture;
    CFrmTimer              m_Timer;
    CFrmUserInterfaceGLES  m_UserInterface;

    GLint                  m_image_uniform_location;
    GLint                  m_time_uniform_location;

    UINT32                 m_fbo_Id;
    UINT32                 m_shader_Id;
    UINT32                 m_SILSSampleShaderProgram;
    UINT32                 m_texture_Id;
};
#endif // SCENE_H
