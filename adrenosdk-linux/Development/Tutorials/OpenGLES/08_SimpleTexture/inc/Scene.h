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
    BYTE *LoadTGA( const CHAR*, UINT32*, UINT32*, UINT32*);
    const CHAR*  g_strWindowTitle;
    CHAR* g_strVSProgram;
    CHAR* g_strFSProgram;
    CHAR *g_strImageFilePath;
    CHAR *g_strImageFileName;
    FLOAT  g_fAspectRatio;
    GLuint g_hShaderProgram;
    GLuint g_hTextureHandle;
    GLuint g_VertexLoc;
    GLuint g_TexcoordLoc;

};

#endif // SCENE_H
