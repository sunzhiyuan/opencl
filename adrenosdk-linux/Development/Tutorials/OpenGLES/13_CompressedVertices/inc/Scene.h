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

enum
{
    g_hVertexLoc,
    g_hNormalLoc,
    g_hColorLoc,
    g_hTexCoordLoc,
};

//--------------------------------------------------------------------------------------
// Uncompressed and compressed vertex data types
//--------------------------------------------------------------------------------------
typedef float          FLOAT32;
typedef unsigned short FLOAT16;
typedef unsigned long  INT_10_10_10;
typedef unsigned long  UINT_8_8_8_8;

struct UNCOMPRESSED_VERTEX
{
    FLOAT32 vPosition[3];
    FLOAT32 vNormal[3];
    FLOAT32 vColor[3];
    FLOAT32 vTexCoord[2];
};

struct COMPRESSED_VERTEX
{
    FLOAT16       vPosition[3];
    INT_10_10_10  vNormal;
    UINT_8_8_8_8  vColor;
    FLOAT16       vTexCoord[2];
};

//--------------------------------------------------------------------------------------
// Define vertices for a 4-sided pyramid
//--------------------------------------------------------------------------------------
UNCOMPRESSED_VERTEX g_UnCompressedVertexData[4*3] =
{
     0.0f,+1.0f, 0.0f,   0.0f, 0.5f, 1.0f,   1.0f, 0.0f, 0.0f,   0.5f, 0.0f,    
    -1.0f,-1.0f, 1.0f,	 0.0f, 0.5f, 1.0f,	 1.0f, 0.0f, 0.0f,   0.0f, 1.0f,    
    +1.0f,-1.0f, 1.0f,	 0.0f, 0.5f, 1.0f,	 1.0f, 0.0f, 0.0f,	 1.0f, 1.0f,

     0.0f,+1.0f, 0.0f,   1.0f, 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.5f, 0.0f,   
    +1.0f,-1.0f, 1.0f, 	 1.0f, 0.5f, 0.0f,	 0.0f, 1.0f, 0.0f,   0.0f, 1.0f,   
    +1.0f,-1.0f,-1.0f,	 1.0f, 0.5f, 0.0f,	 0.0f, 1.0f, 0.0f,	 1.0f, 1.0f,

     0.0f,+1.0f, 0.0f,   0.0f, 0.5f,-1.0f,   0.0f, 0.0f, 1.0f,   0.5f, 0.0f,   
    +1.0f,-1.0f,-1.0f,   0.0f, 0.5f,-1.0f,	 0.0f, 0.0f, 1.0f,   0.0f, 1.0f,    
    -1.0f,-1.0f,-1.0f,	 0.0f, 0.5f,-1.0f,	 0.0f, 0.0f, 1.0f,	 1.0f, 1.0f,
    
     0.0f,+1.0f, 0.0f,  -1.0f, 0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.5f, 0.0f,    
    -1.0f,-1.0f,-1.0f,  -1.0f, 0.5f, 0.0f,	 1.0f, 1.0f, 0.0f,	 0.0f, 1.0f,    
    -1.0f,-1.0f, 1.0f,  -1.0f, 0.5f, 0.0f,	 1.0f, 1.0f, 0.0f,	 1.0f, 1.0f,
};
const int g_nNumVertices = sizeof(g_UnCompressedVertexData) / sizeof(g_UnCompressedVertexData[0]);

// Compressed vertex data is generated by the app
COMPRESSED_VERTEX g_CompressedVertexData[g_nNumVertices];

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
    FLOAT16 CompressFloatToHalfFloat(float);
    INT_10_10_10 CompressVector3to10_10_10(float*);
    UINT_8_8_8_8 CompressVector3to8_8_8_8(float*);

    const CHAR*  g_strWindowTitle;
    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;
    const CHAR* g_strVertexShader;
    const CHAR* g_strFragmentShader;
    const CHAR* g_strImageFileName;
    
    GLuint g_hTextureHandle;
    GLuint g_hShaderProgram;
    GLuint g_hModelViewMatrixLoc;
    GLuint g_hProjMatrixLoc;

};

#endif // SCENE_H
