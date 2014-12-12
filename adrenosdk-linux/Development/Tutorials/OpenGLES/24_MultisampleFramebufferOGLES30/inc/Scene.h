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
// Name: g_internalFormat, g_format, g_type
// Desc: Specifies the format of the multisample framebuffer
//--------------------------------------------------------------------------------------	
GLenum g_format			= GL_RGBA;
GLint g_internalFormat	= GL_RGBA8;
GLenum g_type			= GL_UNSIGNED_BYTE;

// Wrapper for FBO objects and properties
struct FrameBufferObject
{
    UINT32 m_nWidth;
    UINT32 m_nHeight;

    UINT32 m_hFrameBuffer;
    UINT32 m_hRenderBuffer;
    UINT32 m_hColorBuffer;
};

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
    BOOL CreateFBO(UINT32,UINT32,UINT32,UINT32,UINT32,FrameBufferObject**);
    VOID DestroyFBO(FrameBufferObject*);
    VOID BeginFBO(FrameBufferObject*);
    VOID EndFBO(FrameBufferObject*);
    VOID RenderScene(FLOAT);

    const CHAR*  g_strWindowTitle;
    const CHAR* g_strVertexShader;
    const CHAR* g_strFragmentShader;

    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;
      
    FrameBufferObject* g_pOffscreenFBO     ;
                                           
    GLuint       g_hShaderProgram          ;
    GLuint       g_hModelViewMatrixLoc     ;
    GLuint       g_hProjMatrixLoc          ;
    GLuint       g_hVertexLoc              ;
    GLuint       g_hColorLoc               ;
	GLuint		 g_hPositionVbo;
	GLuint		 g_hColorVbo;
	GLuint		 g_hVao;


};

#endif // SCENE_H
