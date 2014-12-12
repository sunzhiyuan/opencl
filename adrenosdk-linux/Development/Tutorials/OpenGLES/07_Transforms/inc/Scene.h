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
    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;
    
    
    GLuint g_hShaderProgram;
    GLuint g_hModelViewMatrixLoc;
    GLuint g_hProjMatrixLoc;
    GLuint g_hVertexLoc;
    GLuint g_hColorLoc;
    CHAR* g_strFragmentShader;
    CHAR* g_strVertexShader;

};

#endif // SCENE_H
