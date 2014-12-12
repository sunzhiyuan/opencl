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
    #define NUMBER_OF_TEXTURES 9
        
public:
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Destroy();
    virtual VOID Update();
    virtual VOID Render();

    CSample( const CHAR* strName );

private:    

    BOOL InitShaders();
    BYTE* LoadTGA(const CHAR*,UINT32*,UINT32*,UINT32*);
    void MakeTriangle(float,float,float,float*);
    void Render2DTexture(INT32);
    void RenderWrapCombo(INT32);
    void RenderWrapClamp(INT32);
    void RenderWrapMirror(INT32);
    void RenderWrapRepeat(INT32);
    void Render1DTexture(INT32);
    void RenderNonPowerof2Texture(INT32);
    void BasicRender(INT32, FLOAT*);

    const CHAR*  g_strWindowTitle;
    UINT32 g_nWindowWidth;
    UINT32 g_nWindowHeight;
    FLOAT  g_fAspectRatio;
    const CHAR*  g_strImageFilePath;

    
    GLuint g_hTextureHandle[NUMBER_OF_TEXTURES];
    GLuint g_hShaderProgram;
    GLuint g_VertexLoc;
    GLuint g_TexcoordLoc;
    FLOAT VertexPositions[9][12];

    const CHAR* g_strVSProgram;
    const CHAR* g_strFSProgram;

};

#endif // SCENE_H
