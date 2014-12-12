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

#define  ATTR_POSITION  0
#define  ATTR_COLOR     1

// Wrapper for FBO objects and properties
struct FrameBufferObject
{
    UINT32 m_nWidth;
    UINT32 m_nHeight;

    UINT32 m_hFrameBuffer;
    UINT32 m_hRenderBuffer;
    UINT32 m_hTexture;
    UINT32 m_hSampler;
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
    BOOL CreateFBO(UINT32,UINT32,UINT32,UINT32,FrameBufferObject**);
    VOID DestroyFBO(FrameBufferObject*);
    VOID BeginFBO(FrameBufferObject*);
    VOID EndFBO(FrameBufferObject*);
    VOID RenderScene(FLOAT);

    const CHAR*  g_strWindowTitle;
    const CHAR* g_strOverlayFragmentShader;
    const CHAR* g_strOverlayVertexShader;
    const CHAR* g_strFragmentShader;
    const CHAR* g_strVertexShader;
    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;
      
    FrameBufferObject* g_pOffscreenFBO;


    GLuint       g_hShaderProgram       ;
    GLuint       g_hOverlayShaderProgram;
    GLuint       g_hModelViewMatrixLoc  ;
    GLuint       g_hProjMatrixLoc       ;
    GLuint       g_hUniformBlock        ;
    GLuint		 g_hTransformBufferName ;
                                        ;
    GLuint		 g_hBuffer			    ;
                                        ;
    GLuint		 g_hVertexArrayName		;
	GLuint		 g_hVao					;
    GLuint		 g_QueryName			;


};

#endif // SCENE_H