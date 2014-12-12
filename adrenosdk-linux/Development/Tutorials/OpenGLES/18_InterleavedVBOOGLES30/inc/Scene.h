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
    const CHAR* g_strVSProgram;
    const CHAR* g_strFSProgram;

    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;
      
    VERTEX_ATTRIBUTES_DATA *pVbuff;
    GLuint       g_hShaderProgram;
    GLuint       g_VertexLoc     ;
    GLuint       g_ColorLoc      ;
    FLOAT fSize;
    FLOAT VertexPositions[16];
    GLuint nPosVBOId;
    GLuint nIndexVBOId;
    INT32 nNumVertices;  
    INT32 nNumIndices ;  
    INT32 nVsize      ;  
    INT32 nCsize      ;  

	GLuint nVaoId;
};

#endif // SCENE_H
