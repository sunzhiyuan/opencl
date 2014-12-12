//--------------------------------------------------------------------------------------
// File: MotionBlur.h
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

// Calculated motion blur based upon the accumulation tree paper located at: 
// www.cs.kuleuven.ac.be/publicaties/rapporten/cw/CW369.pdf 
//
// Adds the notion of a shutter and does NOT assume that the number of samples
// form a complete tree.

#ifndef __MOTION_BLUR_H
#define __MOTION_BLUR_H


class CMotionBlur;  // Predeclaration

#define MAX_FBOS 15 // Supports 2^14 - 1 = 16383 fbo blending

struct CAccumulationLevel
{
    CAccumulationLevel( INT32 nHeight, CMotionBlur &MotionBlur );
    void addBuffer( INT32 nTexNum, INT32 &nCurTexNum );

    UINT32        m_nHeight;       // Leaves are height 0
    INT32         m_nLevelLeaves;  // The number of leaves if the current level was the root (used only after shutter closes)
    INT32         m_nBuffer;       // Keeps track of the texture buffers being used by the level
    CMotionBlur&  m_MotionBlur;    // Reference to the motion blur class
};


class CMotionBlur
{
    friend struct CAccumulationLevel; // Give CAccumulationLevel access to private functions
public:
    CMotionBlur( GLuint& hTextureShader, GLint& nTextureAlphaSlot, 
                 VOID (*RenderFunc)( VOID* pScene ), VOID *pScene, FLOAT32 fShutterDuration, 
                 const GLuint nScreenWidth, const GLuint nScreenHeight,
                 const GLuint nFBOtextureWidth, const GLuint nFBOtextureHeight );
    virtual ~CMotionBlur();

    VOID Apply();                 // Render the current frame of motion blur
    VOID SetShutterDuration( FLOAT32 fShutterDuration );

private:
    VOID ClearAccumulationTree();
    VOID GetBuffer( INT32 nTexNum );
    VOID CombineTextures( INT32 nInput1TexNum, INT32 nInput2TexNum, FLOAT32 fInput2Weight,
                          INT32 nOutputTexNum, INT32 nWidth, INT32 nHeight );
    VOID CombineTextures( INT32 nInput1TexNum, INT32 nInput2TexNum, FLOAT32 fInput2Weight,
                          INT32 nWidth, INT32 nHeight );
    VOID CombineTextures2( INT32 nInput1TexNum, INT32 nInput2TexNum, FLOAT32 fInput2Weight,
                           INT32 nWidth, INT32 nHeight );
    VOID RenderToTexture( INT32 texNum );
    VOID RenderToFramebuffer( INT32 nInput1TexNum, INT32 nInput2TexNum, FLOAT32 fInput2Weight );
    VOID RenderToFramebuffer( INT32 nTexNum );
    VOID RenderScreenAlignedQuad( INT32 nTexNum, FLOAT32 alpha );

    BOOL CheckFrameBufferStatus();

    GLuint&     m_hTextureShader;
    GLint&      m_nTextureAlphaSlot;

    VOID     ( *m_RenderFunc )( VOID* ); // Function pointer to the render command
    VOID*       m_pScene;
    FLOAT32     m_fShutterDuration;      // How long the shutter is open for
    FLOAT32     m_fShutterOpen;          // When the shutter last opened
    INT32       m_nNumBuffers;           // The dimensions of an accumulation tree are limited by
    INT32       m_nNumLevels;            // The number of levels in the accumulation tree
                                  // the number of available render buffers.
                                  // 2^(b+1) buffered frames can be represented,
                                  // ie. if we have 4 buffers then 2^(5) = 64 frames can
                                  // be accumulated.  This class assumes that unlimited
                                  // buffers are available as needed, although realistically,
                                  // 2^10 = 4096 is a lot of buffered frames so it shouldn't
                                  // be a problem, beats blending 4096 textures together!

    GLuint                           m_nDepthbufferId;
    GLuint                           m_pBufferIds[ MAX_FBOS ];
    GLuint                           m_pTextureIds[ MAX_FBOS ];
    CAccumulationLevel*              m_pAccumulationTree[ MAX_FBOS ];

    const GLuint                     m_nScreenWidth;
    const GLuint                     m_nScreenHeight;	
    const GLuint                     m_nFBOTextureWidth;
    const GLuint                     m_nFBOTextureHeight;

    static FLOAT32                   g_pPlane[];
    static FLOAT32                   g_pPlaneTexCoords[];

};

#endif // __MOTION_BLUR_H

