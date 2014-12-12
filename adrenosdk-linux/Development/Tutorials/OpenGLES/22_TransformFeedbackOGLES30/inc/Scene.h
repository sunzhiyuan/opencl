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

struct FrameBufferObject
{
    UINT32 m_nWidth;
    UINT32 m_nHeight;

    UINT32 m_hFrameBuffer;
    UINT32 m_hRenderBuffer;
    UINT32 m_hTexture;
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
    const CHAR*  g_strWindowTitle ;
    const CHAR* g_strVSTransformProgram;
    const CHAR* g_strFSTransformProgram;
    const CHAR* g_strVertexShader;
    const CHAR* g_strFragmentShader;
    const CHAR* g_strOverlayFeedbackVertexShader;
    const CHAR* g_strOverlayFeedbackFragmentShader;

    UINT32 g_nWindowWidth   ;
    UINT32 g_nWindowHeight  ;
    FLOAT  g_fAspectRatio   ;
    FrameBufferObject* g_pOffscreenFBO;

    GLuint       g_hShaderProgram               ;
    GLuint       g_hShaderTransformProgram      ;
    
    GLuint       g_hVertexArrayName		        ;
    GLuint       g_hTransformFeedbackVertexArray;
    GLuint       g_hSourceVertexArray           ;
    GLuint       g_hTransformFeedbackBuffer	    ;
    GLuint       g_hSourceBuffer	            ;
    
    GLuint		 g_hFeedbackName	            ;
    
    GLuint       g_hOverlayShaderProgram        ;
    GLuint       g_hOverlayFeedbackShaderProgram;
    
    GLuint       g_hModelViewMatrixLoc          ;
    GLuint       g_hProjMatrixLoc               ;
    
    GLuint		 g_hColor			            ;
    
    GLuint		 g_hQuery			            ;


};

#endif // SCENE_H
