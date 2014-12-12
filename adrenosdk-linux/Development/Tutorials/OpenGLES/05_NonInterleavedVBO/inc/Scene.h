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
    FLOAT g_fAspectRatio;
    const CHAR*  g_strWindowTitle;
    const CHAR* g_strVSProgram;
    const CHAR* g_strFSProgram;
    GLuint       g_hShaderProgram;
    GLuint       g_VertexLoc;
    GLuint       g_ColorLoc;
    GLuint nPosVBOId, nColorVBOId, nIndexVBOId; // VBO ids
    INT32 nVsize, nNumVertices;
    INT32 nCsize;
    INT32 nNumIndices;
    FLOAT fSize;
    FLOAT VertexPositions[16];
};

#endif // SCENE_H
