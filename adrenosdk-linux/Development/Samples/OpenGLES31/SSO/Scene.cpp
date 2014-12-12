//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc:
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
#include <GLES3/gl31.h>
#include <FrmApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include "Scene.h"

/*
   OPENGL ES 3.1 APIS are called using function pointer since the latest version of OpenGL library available(libGLESv3)
   in Android SDK does not include them.The code under the macro __ANDROID__ has to be removed once the app is
   linked to OPENGL ES 3.1 library
*/

#ifdef __ANDROID__
    typedef void (GL_APIENTRYP PFNGLGENPROGRAMPIPELINESOESPROC)(GLsizei n,GLuint *pipelines);
    PFNGLGENPROGRAMPIPELINESOESPROC fp_glGenProgramPipelines = NULL;

    typedef void (GL_APIENTRYP PFNGLBINDPROGRAMPIPELINEOESPROC)(GLuint pipeline);
    PFNGLBINDPROGRAMPIPELINEOESPROC fp_glBindProgramPipeline = NULL;

    typedef void (GL_APIENTRYP PFNGLUSEPROGRAMSTAGESOESPROC)(GLuint pipeline,GLbitfield stages,GLuint program);
    PFNGLUSEPROGRAMSTAGESOESPROC fp_glUseProgramStages = NULL;

    typedef GLuint (GL_APIENTRYP PFNGLCREATESHADERPROGRAMVOESPROC)(GLenum type,GLsizei count,const char **strings);
    PFNGLCREATESHADERPROGRAMVOESPROC fp_glCreateShaderProgramv = NULL;

    typedef void (GL_APIENTRYP PFNGLDELETEPROGRAMPIPELINESOESPROC)(GLsizei n,const GLuint *pipelines);
    PFNGLDELETEPROGRAMPIPELINESOESPROC fp_glDeleteProgramPipelines = NULL;

    #define glGenProgramPipelines     fp_glGenProgramPipelines
    #define glBindProgramPipeline     fp_glBindProgramPipeline
    #define glUseProgramStages        fp_glUseProgramStages
    #define glCreateShaderProgramv    fp_glCreateShaderProgramv
    #define glDeleteProgramPipelines  fp_glDeleteProgramPipelines
#endif

/* Demo shaders */
const char* fragment_shader_1_body = "#version 310 es\n"
                                      "\n"
                                      "layout(location = 0) in vec2 uv;\n"
                                      "\n"
                                      "out vec4 result;\n"
                                      "\n"
                                      "void main()\n"
                                      "{\n"
                                      "    result = vec4(uv.x, 0.0, 1.0, 1.0);\n"
                                      "}\n";
const char* fragment_shader_2_body = "#version 310 es\n"
                                      "\n"
                                      "layout(location = 0) in vec2 uv;\n"
                                      "\n"
                                      "out vec4 result;\n"
                                      "\n"
                                      "void main()\n"
                                      "{\n"
                                      "    result = vec4(0.0, uv.y, 1.0, 1.0);\n"
                                      "}\n";
const char* fragment_shader_3_body = "#version 310 es\n"
                                      "\n"
                                      "layout(location = 0) in vec2 uv;\n"
                                      "\n"
                                      "out vec4 result;\n"
                                      "\n"
                                      "void main()\n"
                                      "{\n"
                                      "    result = vec4(uv.x, uv.y, 1.0, 1.0);\n"
                                      "}\n";
const char* vertex_shader_body =      "#version 310 es\n"
                                      "\n"
                                      "layout(location = 0) out vec2 uv;\n"
                                      "\n"
                                      "void main()\n"
                                      "{\n"
                                      "    switch (gl_VertexID)\n"
                                      "    {\n"
                                      "        case 0: gl_Position = vec4(-1.0, -1.0, 0.0, 1.0); uv = vec2(0.0, 1.0); break;\n"
                                      "        case 1: gl_Position = vec4(-1.0,  1.0, 0.0, 1.0); uv = vec2(0.0, 0.0); break;\n"
                                      "        case 2: gl_Position = vec4( 1.0, -1.0, 0.0, 1.0); uv = vec2(1.0, 1.0); break;\n"
                                      "        case 3: gl_Position = vec4( 1.0,  1.0, 0.0, 1.0); uv = vec2(1.0, 0.0); break;\n"
                                      "    }\n"
                                      "}\n";


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "SSO" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_fs_id_1               = 0;
    m_fs_id_2               = 0;
    m_fs_id_3               = 0;
    m_pipeline_object_id    = 0;
    m_vao_id                = 0;
    m_vs_id                 = 0;
    m_OpenGLESVersion       = GLES3;

#ifdef __ANDROID__
    fp_glGenProgramPipelines     = (PFNGLGENPROGRAMPIPELINESOESPROC) eglGetProcAddress("glGenProgramPipelines");
    fp_glBindProgramPipeline     = (PFNGLBINDPROGRAMPIPELINEOESPROC) eglGetProcAddress("glBindProgramPipeline");
    fp_glUseProgramStages        = (PFNGLUSEPROGRAMSTAGESOESPROC) eglGetProcAddress("glUseProgramStages");
    fp_glCreateShaderProgramv    = (PFNGLCREATESHADERPROGRAMVOESPROC) eglGetProcAddress("glCreateShaderProgramv");
    fp_glDeleteProgramPipelines  = (PFNGLDELETEPROGRAMPIPELINESOESPROC) eglGetProcAddress("glDeleteProgramPipelines");
#endif
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: Invoked when scene is destroyed.
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    /* Release the fragment shaders */
    if (m_fs_id_1 != 0)
    {
        glDeleteProgram(m_fs_id_1);
        m_fs_id_1 = 0;
    }
    if (m_fs_id_2 != 0)
    {
        glDeleteProgram(m_fs_id_2);
        m_fs_id_1 = 0;
    }
    if (m_fs_id_3 != 0)
    {
        glDeleteProgram(m_fs_id_3);
        m_fs_id_1 = 0;
    }
    if (m_vs_id != 0)
    {
        glDeleteProgram(m_vs_id);
        m_fs_id_1 = 0;
    }

    /* Release the pipeline object */
    if (m_pipeline_object_id != 0)
    {
        glDeleteProgramPipelines(1, &m_pipeline_object_id);
        m_pipeline_object_id = 0;
    }

    /* Release the vertex array object */
    if (m_vao_id != 0)
    {
        glDeleteVertexArrays(1, &m_vao_id);
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
    if( FALSE == resource.LoadFromFile( "Samples/Textures/SSO.pak" ) )
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture( "Logo" );

    // Setup the user interface
    if( FALSE == m_UserInterface.Initialize( &m_Font, m_strName ) )
        return FALSE;

    m_UserInterface.AddOverlay( m_pLogoTexture->m_hTextureHandle, -5, -5,
                                m_pLogoTexture->m_nWidth, m_pLogoTexture->m_nHeight );

    m_UserInterface.AddTextString( "Press " FRM_FONT_KEY_0 " for Help", 1.0f, -1.0f );
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_STAR, "Toggle Orientation" );

    /* Set up a Vertex Array Object */
    glGenVertexArrays(1,&m_vao_id);
    glBindVertexArray(m_vao_id);

    // Initialize the shaders
    if ( (m_fs_id_1 = InitShader(GL_FRAGMENT_SHADER,
                                 fragment_shader_1_body)) == 0)
    {
        return FALSE;
    }

    if ( (m_fs_id_2 = InitShader(GL_FRAGMENT_SHADER,
                                 fragment_shader_2_body)) == 0)
    {
        return FALSE;
    }

    if ( (m_fs_id_3 = InitShader(GL_FRAGMENT_SHADER,
                                 fragment_shader_3_body)) == 0)
    {
        return FALSE;
    }

    if ( (m_vs_id = InitShader(GL_VERTEX_SHADER,
                               vertex_shader_body)) == 0)
    {
        return FALSE;
    }

    /* Create pipeline object */
    glGenProgramPipelines(1, &m_pipeline_object_id);

    /* Configure it */
    glBindProgramPipeline(m_pipeline_object_id);
    glUseProgramStages   (m_pipeline_object_id,
                          GL_VERTEX_SHADER_BIT,
                          m_vs_id);

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: Render the scene.
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glBindProgramPipeline(m_pipeline_object_id);
    glBindVertexArray    (m_vao_id);
    glUseProgram         (0);

    /* Draw a full-screen quad */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLE_STRIP,
                 0,  /* first */
                 4); /* count */

    // Render the user interface
    glBindProgramPipeline(0);
    glBindBuffer         (GL_ARRAY_BUFFER, 0);
    glBindVertexArray    (0);

    m_UserInterface.Render( m_Timer.GetFrameRate() );
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: Invoked when window is resized
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    glViewport( 0, 0, m_nWidth, m_nHeight );
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

   /* Check if we need to switch the fragment shader. This needs to happen either when the
    * first frame is being rendered, or after a pre-defined number of seconds have passed
    * since the last time we've made a switch.
    */
    UINT32         frame_time_seconds       = 0;
    static UINT32  last_frame_time_seconds  = 0;
    const  UINT32  switch_delta_s           = 5;

    frame_time_seconds = (UINT32) m_Timer.GetTime();

    if (last_frame_time_seconds == 0 || (frame_time_seconds - last_frame_time_seconds >= switch_delta_s))
    {
        GLint fs_ids[] =
        {
            m_fs_id_1,
            m_fs_id_2,
            m_fs_id_3
        };
        const UINT32 n_fs_ids       = sizeof(fs_ids) / sizeof(fs_ids[0]);
        UINT32       n_fs_id_to_use = (frame_time_seconds / switch_delta_s) % n_fs_ids;

        /* Switch to a new fragment shader */
        glUseProgramStages(m_pipeline_object_id,
                           GL_FRAGMENT_SHADER_BIT,
                           fs_ids[n_fs_id_to_use]);

        last_frame_time_seconds = frame_time_seconds;
    }

}


//--------------------------------------------------------------------------------------
// Name: InitShader()
// Desc: Initialize the shader program
//--------------------------------------------------------------------------------------
GLuint CSample::InitShader(GLenum shader_type,const CHAR* shader_body)
{
    GLuint result_id  = 0;

    /* Create the separate shader program */
    result_id = glCreateShaderProgramv(shader_type,
                                       1,/* count */
                                       &shader_body);

    /* Verify the linking was successful */
    if (result_id != 0)
    {
        GLint link_status = GL_FALSE;

        glGetProgramiv(result_id,
                       GL_LINK_STATUS,
                       &link_status);

        if (link_status != GL_TRUE)
        {
           glDeleteProgram(result_id);

           result_id = 0;
        }
    }

    return result_id;
}