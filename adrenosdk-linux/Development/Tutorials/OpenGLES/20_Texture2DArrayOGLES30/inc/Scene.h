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


    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;
    const char *g_strWindowTitle;
    const CHAR*  g_strImageFileNames[2];
    UINT32	 g_nImages;		 
    const char* g_strVSProgram;
    const char* g_strFSProgram;

    
    GLuint g_hTextureHandle;
    GLuint g_hShaderProgram;
    GLuint g_VertexLoc;     
    GLuint g_TexcoordLoc;

};

#endif // SCENE_H
