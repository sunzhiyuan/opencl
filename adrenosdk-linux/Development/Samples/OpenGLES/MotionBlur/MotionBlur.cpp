//--------------------------------------------------------------------------------------
// File: MotionBlur.cpp
// Desc: This class is designed to apply motion blur to an arbitrary scene.
//
// There are basically two key concepts involved with this algorithm.  The first
// has to do with the concept of a shutter, and this class' method of emulating
// one.  The second has to do with how this algorithm blends frames together 
// using an accumulation tree to create the desired motion blur effect.
//
// 1. Shutter
// In the days before digital cameras, pictures were created by exposing film.
// The shutter was used to control the duration of time in which the underlying
// film was exposed.  When objects moved rapidly infront of the camera, the 
// longer the shutter was open, the longer the film was exposed, and the larger
// the blur that was obtained as a result.
//
// This class closely mimicks this concept.  Every frame of animation that is
// rendered using the MotionBlur::apply() method is the culmination of several
// frames of animation that are being blured together for some shutter duration
// amount of time.  The shutter opens, frames are rendered to offscreen buffers,
// the shutter closes, and the resulting blended frames are rendered to the 
// framebuffer.  The larger the shutter duration, the more blended frames, the
// larger the blur that is obtained.
//
// 2. Accumulation Tree
// As described in the previous section, every frame that is rendered to the
// framebuffer is the culmination of as many frames as can be rendered to
// offscreen buffers during the shutter duration.  The question becomes, how 
// do we actually store these frames, and handle our blending?
//
// Well, the simplest approach would be to simply render all N frames to N
// separate textures while the shutter is open, and then to render the 
// blended result of these textures to the framebuffer.  While this approach
// sounds good in theory it has 2 severe limitations.  First off, it will
// require a significant amount of storage space, if we are rendering 64
// offscreen frames, that can really add up, in fact on a handheld device,
// it's simply infeasible at the moment.  The second problem is that every
// time that we blend two frames together we incur a certain amount of error.
// So, blending 64 frames together is going to give us very inaccurate 
// results.
//
// The question then becomes, how can we minimize the number of offscreen
// textures required, as well as minimize our error by blending as few frames
// together as possible?  The answer: with an accumulation tree.
//
// The basic idea behind an accumulation tree is that we can blend every 
// multiple of two frames together.  So for example if we render two frames, 
// we can blend them together and only have to keep track of the blended result.
// If we had four frames, we could blend each pair of frames together, be left
// with two frames, and blend those two together.  As a result we are able to 
// reduce our overhead for 64 frames to a mear: 7 offscreen textures.  Plus we 
// have a logarithmic number of blending operations as opposed to a linear 
// number which drastically reduces our error.
//
// To learn more about how accumulation trees work, please read the following 
// paper: www.cs.kuleuven.ac.be/publicaties/rapporten/cw/CW369.pdf
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <FrmStdLib.h>
#include <OpenGLES/FrmUtilsGLES.h>
#include "MotionBlur.h"


FLOAT32 CMotionBlur::g_pPlane[] =
{
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
};

FLOAT32 CMotionBlur::g_pPlaneTexCoords[] =
{
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
};


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
CAccumulationLevel::CAccumulationLevel( INT32 nHeight, CMotionBlur &MotionBlur ) :
    m_nHeight( nHeight ),
    m_nLevelLeaves( 0 ),
    m_nBuffer( -1 ),
    m_MotionBlur( MotionBlur )
{
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
VOID CAccumulationLevel::addBuffer( INT32 nTexNum, INT32 &nCurTexNum )
{
    if( m_nBuffer == -1 )
    {
        m_nBuffer = nTexNum;
    }
    else
    {
        // Merge this levels textures and promote to the higher level

        // combine textures
        m_MotionBlur.CombineTextures( m_nBuffer, nTexNum, 0.5f, m_nBuffer, 
                                      m_MotionBlur.m_nFBOTextureWidth, m_MotionBlur.m_nFBOTextureHeight );

        // Are we at the highest level in the tree?  If so we need to create a higher level.
        if( m_MotionBlur.m_nNumLevels == m_nHeight + 1 )
        {
            CAccumulationLevel* pNewHighestLevel = new CAccumulationLevel( m_nHeight + 1, m_MotionBlur );
            m_MotionBlur.m_pAccumulationTree[ m_MotionBlur.m_nNumLevels++ ] = pNewHighestLevel;
        }

        // Add the 
        m_MotionBlur.m_pAccumulationTree[ m_nHeight + 1 ]->addBuffer( m_nBuffer, nCurTexNum );

        m_nBuffer = -1;

        nCurTexNum--; // We've finished with one of the texture numbers as only one of the 2 merged texture numbers are needed
    }
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
CMotionBlur::CMotionBlur( GLuint& hTextureShader, GLint& nTextureAlphaSlot, 
                          VOID (*RenderFunc)(VOID*), VOID* pScene, FLOAT32 fShutterDuration, 
                          const GLuint nScreenWidth, const GLuint nScreenHeight,
                          const GLuint nFBOtextureWidth, const GLuint nFBOtextureHeight ) :
    m_hTextureShader( hTextureShader ),
    m_nTextureAlphaSlot( nTextureAlphaSlot ),
    m_RenderFunc( RenderFunc ),
    m_pScene( pScene ),
    m_fShutterDuration( fShutterDuration ),
    m_fShutterOpen( 0.0f ),
    m_nNumBuffers( 0 ),
    m_nNumLevels( 0 ),
    m_nDepthbufferId( 0 ),
    m_nScreenWidth( nScreenWidth ),
    m_nScreenHeight( nScreenHeight ),
    m_nFBOTextureWidth( nFBOtextureWidth ),
    m_nFBOTextureHeight( nFBOtextureHeight )
{
    // Clear our arrays to 0
    FrmMemset( m_pBufferIds, 0, MAX_FBOS * sizeof( GLuint ) );
    FrmMemset( m_pTextureIds, 0, MAX_FBOS * sizeof( GLuint ) );
    FrmMemset( m_pAccumulationTree, NULL, MAX_FBOS * sizeof( CAccumulationLevel* ) );

    // Setup our depth buffer that will be shared by our various offscreen buffers
    glGenRenderbuffers( 1, &m_nDepthbufferId );
    glBindRenderbuffer( GL_RENDERBUFFER, m_nDepthbufferId );    
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_nFBOTextureWidth, m_nFBOTextureHeight );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
CMotionBlur::~CMotionBlur()
{
    // Dealocate the render buffers we've used
    for( INT32 nBufferNum = 0; nBufferNum < m_nNumBuffers; ++nBufferNum )
    {
        glBindFramebuffer( GL_FRAMEBUFFER, m_pBufferIds[ nBufferNum ] );
            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glDeleteFramebuffers( 1, &m_pBufferIds[ nBufferNum ] );
        glDeleteTextures( 1, &m_pTextureIds[ nBufferNum ] );
    }
    m_nNumBuffers = 0;

    glDeleteRenderbuffers( 1, &m_nDepthbufferId );

    ClearAccumulationTree();
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: Apply motion blur to the supplied render command
//--------------------------------------------------------------------------------------
VOID CMotionBlur::Apply()
{
    ClearAccumulationTree();

    // Create the accumulation level for our leaves
    CAccumulationLevel *pLeafLevel = new CAccumulationLevel( 0, *this );
    m_pAccumulationTree[ m_nNumLevels++ ] = pLeafLevel;

    int nCurTexNum = 0;

    m_fShutterOpen = FrmGetTime();
    FLOAT32 fShutterClose = m_fShutterOpen + m_fShutterDuration;

    // Keep rendering to our accumulation tree while our shutter is still open
    while( FrmGetTime() < fShutterClose )
    {
        RenderToTexture( nCurTexNum );
        pLeafLevel->addBuffer( nCurTexNum, nCurTexNum );    // Note add can change curTexNum
        nCurTexNum++;
    }

    // The shutter has closed, we're done rendering to our tree
    // Figure out the number of leaves on the left and right subtrees
    // to figure out the weight of the final texture combinations

     // Need to make sure we have enought space for MAX_FBO levels + blended FBO levels.
    CAccumulationLevel* pTreeRootsQueue[ 2 * MAX_FBOS ];
    FrmMemset( pTreeRootsQueue, NULL, MAX_FBOS * sizeof( CAccumulationLevel* ) );

    INT32 nIndex = 0;
    for( INT32 nLevel = m_nNumLevels - 1; nLevel >= 0; --nLevel )
    {
        if( m_pAccumulationTree[ nLevel ]->m_nBuffer != -1 )
        {
            m_pAccumulationTree[ nLevel ]->m_nLevelLeaves = 1 << nLevel; // 2 ^ level
            pTreeRootsQueue[ nIndex++ ] = m_pAccumulationTree[ nLevel ];
        }
    }

    CAccumulationLevel* pRoot1 = NULL;
    CAccumulationLevel* pRoot2 = NULL;
    GLfloat fWeight1 = 0;
    GLfloat fWeight2 = 0;
    INT32 nTotalLeaves = 0;

    INT32 nFront = 0;
    while( nIndex - nFront > 0 )
    {
        pRoot1 = pTreeRootsQueue[ nFront ];
        pTreeRootsQueue[ nFront++ ] = NULL;

        // If the queue is empty
        if( nIndex - nFront == 0 )
        {
            // copy root1 to the framebuffer, we're done
            RenderToFramebuffer( pRoot1->m_nBuffer );
        }
        else
        {
            pRoot2 = pTreeRootsQueue[ nFront ];
            pTreeRootsQueue[ nFront++ ] = NULL;

            // If the queue is empty
            if( nIndex - nFront == 0 )
            {
                // Blend root1 and root2 in the framebuffer with their weights
                // ie. if root1 has 8 leaves and root2 has 4 leaves, then
                //     root1's weight is 2 / 3 and root2's weight is 1 / 3
                nTotalLeaves = pRoot1->m_nLevelLeaves + pRoot2->m_nLevelLeaves;
                fWeight2 = pRoot2->m_nLevelLeaves / (FLOAT32)nTotalLeaves;
                RenderToFramebuffer( pRoot1->m_nBuffer, pRoot2->m_nBuffer, fWeight2 );
            }
            else
            {
                // Blend root1 and root2 in the specified buffer with their weights
                // ie. if root1 has 8 leaves and root2 has 4 leaves, then
                //     root1's weight is 2 / 3 and root2's weight is 1 / 3
                nTotalLeaves = pRoot1->m_nLevelLeaves + pRoot2->m_nLevelLeaves;
                fWeight2 = pRoot2->m_nLevelLeaves / (FLOAT32)nTotalLeaves;
                CombineTextures( pRoot1->m_nBuffer, pRoot2->m_nBuffer, fWeight2, 
                                 pRoot1->m_nBuffer, m_nFBOTextureWidth, m_nFBOTextureHeight );

                // Change the number of leaves of root1
                pRoot1->m_nLevelLeaves += pRoot2->m_nLevelLeaves;

                // we're done with root2 add root1 back to the end of the line
                pTreeRootsQueue[ nIndex++ ] = pRoot1;
            }
        }
    }
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
VOID CMotionBlur::SetShutterDuration( FLOAT32 fShutterDuration )
{
    m_fShutterDuration = fShutterDuration;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
VOID CMotionBlur::ClearAccumulationTree()
{
    for( int nAccumulationIndex = 0; nAccumulationIndex < m_nNumLevels; ++nAccumulationIndex )
    {
        if( m_pAccumulationTree[ nAccumulationIndex ] == NULL )
        {
            break;
        }
        delete m_pAccumulationTree[ nAccumulationIndex ];
        m_pAccumulationTree[ nAccumulationIndex ] = NULL;
    }

    m_nNumLevels = 0;
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
VOID CMotionBlur::GetBuffer( INT32 nTexNum )
{
    // Check to see if the buffer texNum is initialized, if not, set it up
    if( nTexNum >= m_nNumBuffers )
    {
        // Add an FBO to our list
        FrmAssert( m_pBufferIds[ nTexNum ] == 0 );
        glGenFramebuffers( 1, &m_pBufferIds[ nTexNum ] );
        glBindFramebuffer( GL_FRAMEBUFFER, m_pBufferIds[ nTexNum ] );
        FrmAssert( m_pBufferIds[ nTexNum ] != 0 );

        // Add a new texture to our list
        FrmAssert( m_pTextureIds[ nTexNum ] == 0 );

        GLushort *pData = new GLushort[ m_nFBOTextureWidth * m_nFBOTextureHeight * 4 ];
        FrmCreateTexture( m_nFBOTextureWidth, m_nFBOTextureHeight, 1, GL_RGBA,
                          GL_UNSIGNED_SHORT_5_5_5_1, 0, pData,
                          0, ( GLint* )&( m_pTextureIds[ nTexNum ] ) );
        delete [] pData;
        FrmAssert( m_pTextureIds[ nTexNum ] != 0 );

        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, m_pTextureIds[ nTexNum ] );
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTextureIds[ nTexNum ], 0 );

        // Check for framebuffer problems
        CheckFrameBufferStatus();

        m_nNumBuffers++;
    }
    else
    {
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, m_pTextureIds[ nTexNum ] );
        glBindFramebuffer( GL_FRAMEBUFFER, m_pBufferIds[ nTexNum ] );
    }

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTextureIds[ nTexNum ], 0 );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_nDepthbufferId );
    glViewport( 0, 0, m_nFBOTextureWidth, m_nFBOTextureHeight );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
VOID CMotionBlur::CombineTextures( INT32 nInput1TexNum, INT32 nInput2TexNum, FLOAT32 fInput2Weight,
                                   INT32 nOutputTexNum, INT32 nWidth, INT32 nHeight )
{
    GetBuffer( nOutputTexNum );
    CombineTextures( nInput1TexNum, nInput2TexNum, fInput2Weight, nWidth, nHeight );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
VOID CMotionBlur::CombineTextures( INT32 nInput1TexNum, INT32 nInput2TexNum, FLOAT32 fInput2Weight,
                                   INT32 nWidth, INT32 nHeight )
{
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );

    RenderScreenAlignedQuad( nInput2TexNum, fInput2Weight );

    glDisable( GL_BLEND );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
VOID CMotionBlur::CombineTextures2( INT32 nInput1TexNum, INT32 nInput2TexNum, FLOAT32 fInput2Weight,
                                    INT32 nWidth, INT32 nHeight )
{
    RenderScreenAlignedQuad( nInput1TexNum, 1.0f );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_BLEND );

    RenderScreenAlignedQuad( nInput2TexNum, fInput2Weight );

    glDisable( GL_BLEND );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
VOID CMotionBlur::RenderToTexture( INT32 nTexNum )
{
    GetBuffer( nTexNum );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Render the current frame to texture texNum
    ( *m_RenderFunc )( m_pScene );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
VOID CMotionBlur::RenderToFramebuffer( INT32 nInput1TexNum, INT32 nInput2TexNum, FLOAT32 fInput2Weight )
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nScreenWidth, m_nScreenHeight );
    CombineTextures2( nInput1TexNum, nInput2TexNum, fInput2Weight, m_nScreenWidth, m_nScreenHeight );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
VOID CMotionBlur::RenderToFramebuffer( INT32 nTexNum )
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_nScreenWidth, m_nScreenHeight );

    RenderScreenAlignedQuad( nTexNum, 1.0f );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
VOID CMotionBlur::RenderScreenAlignedQuad( INT32 nTexNum, FLOAT32 fAlpha )
{
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTextureIds[ nTexNum ] );

    glUseProgram( m_hTextureShader );
    glUniform1f( m_nTextureAlphaSlot, fAlpha );

    const FLOAT32 Quad[] =
    {
        - 1.0, -1.0f, 0.0f, 1.0f,
        + 1.0, -1.0f, 1.0f, 1.0f,
        + 1.0, +1.0f, 1.0f, 0.0f,
        - 1.0, +1.0f, 0.0f, 0.0f,
     };

    glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, Quad );
    glEnableVertexAttribArray( 0 );

    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );

    glDisableVertexAttribArray( 0 );

    glDisable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );
}


//--------------------------------------------------------------------------------------
// Name:
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CMotionBlur::CheckFrameBufferStatus()
{
    switch( glCheckFramebufferStatus( GL_FRAMEBUFFER ) )
    {
        case GL_FRAMEBUFFER_COMPLETE:
            return TRUE;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            return FALSE;
    }

    FrmAssert(0);   // assert if we get here, because we shouldn't get here
   
    return FALSE;
}
