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

    const CHAR*  g_strWindowTitle;
    const CHAR* g_strVSProgram;
    const CHAR* g_strFSProgram;
    const CHAR*  g_strImageFileName0;
    const CHAR*  g_strImageFileName1;
    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;

    GLuint g_hTexture0Handle;
    GLuint g_hTexture1Handle;
    GLuint g_hShaderProgram;
    GLuint g_VertexLoc;
    GLuint g_Texcoord0Loc;
    GLuint g_Texcoord1Loc;
    GLuint g_hTexture0Slot;
    GLuint g_hTexture1Slot;
};

#endif // SCENE_H
