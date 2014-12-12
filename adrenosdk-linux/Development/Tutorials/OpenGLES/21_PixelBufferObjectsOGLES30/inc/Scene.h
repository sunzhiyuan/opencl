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
    void UpdatePixels(UCHAR *, int);
    void UpdateTexture();

    CHAR*  g_strWindowTitle;
    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;

    const CHAR*  g_strImageFileName;
    UINT32	    g_nImages;
    
    GLuint       g_hTextureHandle;
    GLuint       g_hShaderProgram;
    GLuint       g_VertexLoc     ;
    GLuint       g_TexcoordLoc   ;
    
    GLuint		 g_hPbos[2];
    GLubyte*	 g_imageData	;
    int	 IMAGE_WIDTH	;
    int	 IMAGE_HEIGHT	;
    int	 CHANNEL_COUNT	;
    int	 DATA_SIZE		;
    GLint  INTERNAL_FORMAT;
    GLenum DATA_TYPE		;
    GLenum FORMAT			;
    
    const CHAR* g_strVSProgram;
    const CHAR* g_strFSProgram;

	GLuint		 g_hPositionVbo;
	GLuint		 g_hTexcoordVbo;
	GLuint		 g_hVao;

    
};

#endif // SCENE_H
