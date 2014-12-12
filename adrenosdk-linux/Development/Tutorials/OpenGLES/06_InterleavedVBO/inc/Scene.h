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

typedef struct
{
    FLOAT x;
    FLOAT y;
    FLOAT z;
    FLOAT w;
} POS;

typedef struct
{
    FLOAT r;
    FLOAT g;
    FLOAT b;
    FLOAT a;
} COLOR;

// Structure to hold vertex attributes
typedef struct
{
    POS      *pVertices;
    UINT8    *pPosOffset;
    UINT8    *pColorOffset;
    INT32     nStride;
    INT32     nTotalSizeInBytes;
}VERTEX_ATTRIBUTES_DATA;

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
    VOID InitVertexAttributesData();
    const CHAR*  g_strWindowTitle;
    CHAR* g_strVSProgram;
    CHAR* g_strFSProgram;
    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;
    VERTEX_ATTRIBUTES_DATA *pVbuff;
    INT32 nNumVertices; // Vertices have 4 components
    INT32 nNumIndices;
    INT32 nVsize;
    INT32 nCsize;
    GLuint nPosVBOId;
    GLuint nIndexVBOId; // VBO ids
    GLuint g_VertexLoc;
    GLuint g_ColorLoc;
    
    FLOAT VertexPositions[16];
    GLuint g_hShaderProgram;
    GLuint g_hModelViewMatrixLoc;
    GLuint g_hProjMatrixLoc;

    CHAR* g_strFragmentShader;
    CHAR* g_strVertexShader;

};

#endif // SCENE_H
