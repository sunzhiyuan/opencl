//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl31.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include "Scene.h"

/*
   OPENGL ES 3.1 APIS are called using function pointer since the latest version of OpenGL library available (libGLESv3)
   in Android SDK does not include them.The code under the macro __ANDROID__ has to be removed once the app is
   linked to OPENGL ES 3.1 library
*/

#ifdef __ANDROID__
    typedef void (GL_APIENTRYP PFNGLGENPROGRAMUNIFORMMATRIX4FVOESPROC)(GLuint program,GLint location,GLsizei count,GLboolean transpose,const GLfloat *value);
    PFNGLGENPROGRAMUNIFORMMATRIX4FVOESPROC fp_glProgramUniformMatrix4fv = NULL;

    typedef void (GL_APIENTRYP PFNGLTEXSTORAGE2DMULTISAMPLEOESPROC)(GLenum target,GLsizei samples,GLenum internalformat,GLsizei width,GLsizei height,GLboolean fixedsamplelocations);
    PFNGLTEXSTORAGE2DMULTISAMPLEOESPROC fp_glTexStorage2DMultisample = NULL;

    #define glProgramUniformMatrix4fv     fp_glProgramUniformMatrix4fv
    #define glTexStorage2DMultisample     fp_glTexStorage2DMultisample
#endif

/* Cube data set: vertices */
const FLOAT g_cube_vertices[] =
{
  /* x   y   z  w */
    -1, -1, -1, 1,
    -1, -1,  1, 1,
     1, -1,  1, 1,
     1, -1, -1, 1,

    -1,  1, -1, 1,
    -1,  1,  1, 1,
     1,  1,  1, 1,
     1,  1, -1, 1
};

// Cube data set: indices
const BYTE g_cube_indices[] =
{
    0, 1,  1, 2,  2, 3,  3, 0,
    4, 5,  5, 6,  6, 7,  7, 4,
    0, 4,  1, 5,  2, 6,  3, 7
};

//structure to hold texture information
struct texture_descriptor
{
    UINT32    n_samples;
    UINT32    texture;
};

/* Tells how many textures, each with different number of samples, we want to use
* for the demo.
*/
const UINT32        NO_MULTIMPLE_TEXTURES               = 4;
texture_descriptor  g_textures[NO_MULTIMPLE_TEXTURES]   = {NULL};

/* Tells how many msec is needed for the cube to perform a full rotation */
const UINT32        FULL_ROTATION_TIME_MSEC             = 40000;

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "MultiSample" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName ),
                                          m_cube_indices(sizeof(g_cube_indices) / sizeof(g_cube_indices[0])),
                                          m_cube_lines( m_cube_indices / 2)
{
    m_fCurrentTime              = 0.0f;
    m_fStartTime                = FrmGetTime();

    m_is_frozen                 = FALSE;
    m_bRotationOn               = TRUE;

    m_MultiSampleShaderProgram  = NULL;
    m_pLogoTexture              = NULL;

    m_OpenGLESVersion           = GLES3;

    m_currentTextureIndex       = 0;

 #ifdef __ANDROID__
     fp_glProgramUniformMatrix4fv     = (PFNGLGENPROGRAMUNIFORMMATRIX4FVOESPROC) eglGetProcAddress("glProgramUniformMatrix4fv");
     fp_glTexStorage2DMultisample     = (PFNGLTEXSTORAGE2DMULTISAMPLEOESPROC) eglGetProcAddress("glTexStorage2DMultisample");
 #endif
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: Invoked when scene is destroyed.
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    // Release shaders
    if( m_MultiSampleShaderProgram )
        glDeleteProgram(m_MultiSampleShaderProgram);

    // Release textures
    if( m_pLogoTexture )
        m_pLogoTexture->Release();

     /* Release the vertex buffer object */
    if (m_vbo_id != 0)
    {
        glDeleteBuffers(1, &m_vbo_id);
        m_vbo_id = 0;
    }

    /* Release the framebuffer object */
    if (m_fboId != 0)
    {
        glDeleteFramebuffers(1, &m_fboId);
        m_fboId = 0;
    }

    DeleteTextures();

    /* Release the vertex array object */
    if (m_vao_id != 0)
    {
        glDeleteVertexArrays(1,
                             &m_vao_id);
        m_vao_id = 0;
    }

}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: Initialize the scene.
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/MultiSample.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;
    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );
    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddBoolVariable( &m_bRotationOn, "Rotation", "On", "Off" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_1, "Start Rotating" );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_2, "Stop Rotating" );

    InitProjectionMatrix();

    /* Set up line width */
    glLineWidth(1.0f);

    /* Set up a Framebuffer Object */
    glGenFramebuffers(1,&m_fboId);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);

    InitVertexBufferObject();

    /* Note that our FBO does not use a depth attachment. Safe to disable the depth writes */
    glDepthMask(GL_FALSE);

    InitVertexArrayObject();

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    InitTexture();

    /* Set up clear color */
    glClearColor(0.0f, 0.75f, 1.0f, 1.0f);

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: Render the scene.
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    /* Clear the color buffer before we continue */
    glBindFramebuffer(GL_FRAMEBUFFER,
                      m_fboId);

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram     (m_MultiSampleShaderProgram);
    glBindVertexArray(m_vao_id);

    {
        glDrawElements(GL_LINES,
                       m_cube_lines * 2, /* vertices per line */
                       GL_UNSIGNED_BYTE, /* type */
                       NULL);            /* indices */
    }

    /* Blit the multisample texture contents onto single-sampled back buffer */
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                      0);

    glBlitFramebuffer(0,                  /* srcX0 */
                      0,                  /* srcY0 */
                      m_nWidth,
                      m_nHeight,
                      0,                 /* dstX0 */
                      0,                 /* dstY0 */
                      m_nWidth,
                      m_nHeight,
                      GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);       /* filter */

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    glBindVertexArray(0);
    glBindBuffer     (GL_ARRAY_BUFFER, 0);

    m_UserInterface.Render( m_Timer.GetFrameRate() );

    /* All done! */
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                      m_fboId);
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: Invoked when window is resized
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    InitProjectionMatrix();
    InitTexture();
    InitOffScreenFramebuffer(m_currentTextureIndex);

    glViewport(0,          /* x */
               0,          /* y */
               m_nWidth,   /* width */
               m_nHeight); /* height */

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: Update the scene.
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
   // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;
    FrmGetInput( &m_Input, &nButtons, &nPressedButtons );

    // Toggle user interface
    if( nPressedButtons & INPUT_KEY_0 )
        m_UserInterface.AdvanceState();

    if( nPressedButtons & FRM_INPUT::KEY_1 )
    {
        if (!m_is_frozen)
        {
            m_bRotationOn = TRUE;
            m_is_frozen   = TRUE;
            m_freeze_time = m_Timer.GetTime();
        }

    }
    if( nPressedButtons & FRM_INPUT::KEY_2 )
    {
         m_is_frozen   = FALSE;
         m_bRotationOn = FALSE;
    }

    /* Kick off off-screen rendering */
    glBindFramebuffer(GL_FRAMEBUFFER,
                      m_fboId);
    glUseProgram(m_MultiSampleShaderProgram);

    /* Check if we need to switch to a different texture. This needs to happen either when the
     * first frame is being rendered, or after a pre-defined number of seconds have passed
     * since the last time we've made a switch.
     */
           UINT32  frame_time_seconds      = 0;
    static UINT32  last_frame_time_seconds = 0;
    const  UINT32  switch_delta_s          = 5;

    frame_time_seconds = (UINT32) m_Timer.GetTime();

    if (last_frame_time_seconds == 0 || (frame_time_seconds - last_frame_time_seconds >= switch_delta_s))
    {
        m_currentTextureIndex = (frame_time_seconds / switch_delta_s) % NO_MULTIMPLE_TEXTURES;

        //TODO need to show sample no on to the screen once app starts working.
        //LOG_INFO("Using texture with [%d] samples", _textures[n_texture_to_use].n_samples);

        InitOffScreenFramebuffer(m_currentTextureIndex);

        last_frame_time_seconds = frame_time_seconds;
    }

    /* Set up MVP matrix */
    static FLOAT32 angle              = 0.0f;
    UINT32         frame_time_msec    = 0;
    FRMVECTOR3     rotation_xyz       = FRMVECTOR3( 1.0f, 0.0f,  0.0f);
    FRMVECTOR3     translation_xyz    = FRMVECTOR3(0.0f, 0.0f, -3.0f);

    if (m_is_frozen)
    {
        frame_time_msec = (UINT32) (m_freeze_time * 1000);
    }
    else
    {
        frame_time_msec = (UINT32) (m_Timer.GetTime() * 1000);
    }

    angle = float(frame_time_msec % FULL_ROTATION_TIME_MSEC) / float(FULL_ROTATION_TIME_MSEC) * 2 * 3.14152965f;

    {
        FRMMATRIX4X4 model;
        FRMMATRIX4X4 rotation_matrix;
        FRMMATRIX4X4 translation_matrix;

        translation_matrix = FrmMatrixTranslate(translation_xyz);
        rotation_matrix    = FrmMatrixRotate   (angle,rotation_xyz);

        model = FrmMatrixMultiply(rotation_matrix, translation_matrix);
        m_mvp = FrmMatrixMultiply(model,           m_matProj);

        glProgramUniformMatrix4fv(m_MultiSampleShaderProgram,
                                  m_mvpLocation,
                                  1,                          /* count */
                                  GL_FALSE,                   /* transpose */
                                  (FLOAT32*) &m_mvp);
    }
}


//--------------------------------------------------------------------------------------
// Name: CheckFrameBufferStatus()
// Desc: check the completeness status of a framebuffer.
//--------------------------------------------------------------------------------------
BOOL CSample::CheckFrameBufferStatus()
{
    switch( glCheckFramebufferStatus(GL_FRAMEBUFFER) )
    {
        case GL_FRAMEBUFFER_COMPLETE:
            return TRUE;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            return FALSE;
    }

    FrmAssert(0);
    return FALSE;
}


//--------------------------------------------------------------------------------------
// Name: DeleteTextures()
// Desc: Delete textures
//--------------------------------------------------------------------------------------
void CSample::DeleteTextures()
{
    /* Release the multisample textures */
    for (UINT32 n_texture = 0; n_texture < NO_MULTIMPLE_TEXTURES; ++n_texture)
    {
        if (g_textures[n_texture].texture != 0)
        {
            glDeleteTextures( 1,
                              &g_textures[n_texture].texture );
            g_textures[n_texture].texture = 0;
        }
    }
}

//--------------------------------------------------------------------------------------
// Name: InitOffScreenFramebuffer()
// Desc: Initializes off-screen framebuffer color attachment.
//--------------------------------------------------------------------------------------
void CSample::InitOffScreenFramebuffer(unsigned int n_texture_to_use)
{
    glBindFramebuffer     (GL_DRAW_FRAMEBUFFER,
                           m_fboId);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D_MULTISAMPLE,
                           g_textures[n_texture_to_use].texture,
                           0); /* level */

    if( TRUE != CheckFrameBufferStatus() )
        return;
}

//--------------------------------------------------------------------------------------
// Name: InitProjectionMatrix()
// Desc: Initialize the projection matrix.
//--------------------------------------------------------------------------------------
void CSample::InitProjectionMatrix()
{
    //set Matrix Perspective
    const FLOAT32 field_of_view     = 45.0f;
    const FLOAT32 z_near            = 0.001f;
    const FLOAT32 z_far             = 10.0f;

    m_matProj = FrmMatrixPerspectiveFovRH( field_of_view /* Field of View */,
                                           FLOAT(m_nWidth) / FLOAT(m_nHeight) /*ratio*/,
                                           z_near,
                                           z_far);

}

//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the Main shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        {
           { "g_vVertex",   FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/MultiSample.vs",
                                                      "Samples/Shaders/MultiSample.fs",
                                                      &m_MultiSampleShaderProgram,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_mvpLocation = glGetUniformLocation( m_MultiSampleShaderProgram,
                                              "mvp" );

        if(-1 == m_mvpLocation)
           return FALSE;

    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitTexture()
// Desc: Set up Texture
//--------------------------------------------------------------------------------------
BOOL CSample::InitTexture()
{
    GLint n_max_color_texture_samples = 0;

    DeleteTextures();

    glGetInternalformativ(GL_TEXTURE_2D_MULTISAMPLE, GL_RGBA8, GL_SAMPLES, 1, &n_max_color_texture_samples);

    for (UINT32 n_texture = 0;n_texture < NO_MULTIMPLE_TEXTURES; ++n_texture)
    {
         UINT32 n_texture_samples = n_max_color_texture_samples * n_texture / (NO_MULTIMPLE_TEXTURES - 1);

         if (n_texture_samples == 0)
         {
             /* Requesting zero samples is not permitted */
             n_texture_samples = 1;
         }

         GLuint texture_id;
         glGenTextures( 1, &texture_id );

         g_textures[n_texture].n_samples = n_texture_samples;
         g_textures[n_texture].texture   = texture_id;

         glBindTexture(GL_TEXTURE_2D_MULTISAMPLE,
                       g_textures[n_texture].texture);

         glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
                                   n_texture_samples,
                                   GL_RGBA8,
                                   m_nWidth,
                                   m_nHeight,
                                   GL_FALSE);                /* fixedsamplelocations */

    } /* for (all multisample textures that need to be initialized) */

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitVertexArrayObject()
// Desc: Set up a Vertex Array Object
//--------------------------------------------------------------------------------------
BOOL CSample::InitVertexArrayObject()
{
     const UINT32 bytes_indices  = sizeof(g_cube_indices);

     glGenVertexArrays(1,
                       &m_vao_id);
     glBindVertexArray(m_vao_id);

     glBindBuffer         (GL_ARRAY_BUFFER,
                           m_vbo_id);
     glVertexAttribPointer(0,                               /* index      */
                           4,                               /* size       */
                           GL_FLOAT,
                           GL_FALSE,                        /* normalized */
                           0,                               /* stride     */
                           (const GLvoid*) bytes_indices);  /* pointer    */

    glEnableVertexAttribArray(0);                           /* index      */

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 m_vbo_id);

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitVertexBufferObject()
// Desc: Set up a Buffer object
//--------------------------------------------------------------------------------------
BOOL CSample::InitVertexBufferObject()
{
    /* Set up a Buffer object */
    const UINT32 bytes_indices  = sizeof(g_cube_indices);
    const UINT32 bytes_vertices = sizeof(g_cube_vertices);
    const UINT32 bytes_total    = bytes_vertices + bytes_indices;

    glGenBuffers(1,
                 &m_vbo_id);

    glBindBuffer(GL_ARRAY_BUFFER,
                 m_vbo_id);

    glBufferData(GL_ARRAY_BUFFER,
                 bytes_total,
                 NULL, /* data */
                 GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER,
                    0, /* offset */
                    bytes_indices,
                    g_cube_indices);

    glBufferSubData(GL_ARRAY_BUFFER,
                    bytes_indices,
                    bytes_vertices,
                    g_cube_vertices);

    return TRUE;
}
