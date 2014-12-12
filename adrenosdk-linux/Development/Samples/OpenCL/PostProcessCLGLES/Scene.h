//--------------------------------------------------------------------------------------
// File: Scene.h
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H


//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//
//       If m_sharedBuffer is true, CL will use memory already allocated in GL.
//       Otherwise, CL allocate new memory. This will involve copying GL memory to host.
//       Then, copy host memory back to GL memory after CL post processing.
//       The purpose is to show the advantage of CL/GL interop.
//
//--------------------------------------------------------------------------------------
class CSample : public CFrmComputeGraphicsApplication
{
public:
    CSample( const CHAR* strName, BOOL sharedBuffer=true );

    // CFrmApplication
    virtual BOOL Initialize();
    virtual BOOL Resize();
    virtual VOID Update();
    virtual VOID Render();
    virtual VOID Destroy();

private:
    BOOL InitShaders();
    BOOL InitKernels();
    BOOL CreateFBO( INT32 nWidth, INT32 nHeight );
    VOID FreeFBO();
    VOID CheckFrameBufferStatus();
    VOID RenderScreenAlignedQuad();
    BOOL PostProcessFBOOpenCL();

    CFrmFontGLES          m_Font;
    CFrmTimer             m_Timer;
    CFrmUserInterfaceGLES m_UserInterface;

    FRMMATRIX4X4       m_matView;
    FRMMATRIX4X4       m_matProj;
    FRMMATRIX4X4       m_matViewProj;
    FRMMATRIX4X4       m_matModelView;
    FRMMATRIX4X4       m_matModelViewProj;
    FRMMATRIX3X3       m_matNormal;
    CFrmMesh           m_Mesh;

    CFrmArcBall        m_ArcBall;
    FRMVECTOR4         m_qMeshRotation;
    FRMVECTOR4         m_qRotationDelta;

    FRMVECTOR3         m_vLightPosition;

    GLuint             m_hTextureShader;
    GLuint             m_hPerPixelLightingShader;
    GLint              m_hPPLModelViewMatrixLoc;
    GLint              m_hPPLModelViewProjMatrixLoc;
    GLint              m_hPPLNormalMatrixLoc;
    GLint              m_hPPLLightPositionLoc;
    GLint              m_hPPLMaterialAmbientLoc;
    GLint              m_hPPLMaterialDiffuseLoc;
    GLint              m_hPPLMaterialSpecularLoc;

    GLuint             m_pBufferId;
    GLuint             m_pTextureId;
    GLuint             m_pDepthBufferId;
    GLuint             m_pFilteredTextureId;

    CFrmTexture*       m_pLogoTexture;

    INT32              m_nFBOWidth;
    INT32              m_nFBOHeight;

    // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernel
    cl_kernel m_kernel;

    // OpenCL image object pointer to FBO memory
    cl_mem m_fboMem;

    // OpenCL image object pointer to result texture memory
    cl_mem m_filteredTexMem;

    BOOL m_sharedBuffer;

    // Host memory buffer for intermediate copying step if no buffersharing
    BYTE* m_pHostBuf;

};


#endif // SCENE_H
