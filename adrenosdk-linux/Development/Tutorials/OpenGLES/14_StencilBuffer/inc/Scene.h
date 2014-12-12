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
    BOOL InitializeSkyShader();
    BOOL InitializeMaskShader();
    const CHAR* g_strSkyVSProgram;
    const CHAR* g_strSkyFSProgram;
    const CHAR* g_strMaskVSProgram;
    const CHAR* g_strMaskFSProgram;
    const CHAR*  g_strImageFileName;
    const CHAR*  g_strWindowTitle;
    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;
    

    GLuint g_hSkyTexture;
    
    GLuint g_hSkyShaderProgram;
    GLuint g_hSkyVertexLoc;
    GLuint g_hSkyColorLoc;
    GLuint g_hSkyTimeLoc;
    
    GLuint g_hMaskShaderProgram;
    GLuint g_hMaskVertexLoc;

    

};

#endif // SCENE_H
