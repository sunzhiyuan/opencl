//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
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
class CSample : public CFrmComputeGraphicsApplication
{
public:
    CSample( const CHAR* strName );

    // CFrmApplication
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Update();
    virtual VOID Render();
    virtual VOID Destroy();

private:
    BOOL InitShaders();
    BOOL InitKernels();
    BOOL ComputeGraphVerts( FLOAT32 nTime );

    CFrmFontGLES          m_Font;
    CFrmTimer             m_Timer;
    CFrmUserInterfaceGLES m_UserInterface;

    INT32                 m_nLineSegments;

    CFrmTexture*       m_pLogoTexture;

    GLuint             m_hGraphShader;
    GLuint             m_hGraphVBO;
    GLuint             m_hGraphIBO;

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernel
    cl_kernel m_kernel;

    // OpenCL pointer to VBO memory
    cl_mem m_vboMem;

};


#endif // SCENE_H
