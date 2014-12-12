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
#include <string>
#include <sstream>
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
    typedef void (GL_APIENTRYP PFNGLDISPATCHCOMPUTEOESPROC)(GLuint num_groups_x,GLuint num_groups_y,GLuint num_groups_z);
    PFNGLDISPATCHCOMPUTEOESPROC fp_glDispatchCompute = NULL;

    typedef void (GL_APIENTRYP PFNGLMEMORYBARRIEROESPROC)(GLbitfield barriers);
    PFNGLMEMORYBARRIEROESPROC fp_glMemoryBarrier = NULL;

    typedef void (GL_APIENTRYP PFNGLBINDIMAGETEXTUREOESPROC)(GLuint unit,GLuint texture,GLint level,GLboolean layered,GLint layer,GLenum access,GLenum format);
    PFNGLBINDIMAGETEXTUREOESPROC fp_glBindImageTexture = NULL;

    #define glDispatchCompute     fp_glDispatchCompute
    #define glMemoryBarrier       fp_glMemoryBarrier
    #define glBindImageTexture    fp_glBindImageTexture
#endif

const char* compute_shader_body = "#version 310 es\n"
                                   "\n"
                                   "layout(local_size_x = LOCAL_SIZE_X, local_size_y = LOCAL_SIZE_Y) in;\n"
                                   "layout(rgba8) uniform image2D image;\n"
                                   "uniform                    float   time;\n"
                                   "\n"
                                   "void main()\n"
                                   "{\n"
                                   "    const uvec2 image_size       = imageSize(image);\n"
                                   "    const  vec2 image_size_float = vec2(image_size);\n"
                                   "\n"
                                   "    vec4 result = vec4(abs(cos(     time + float(gl_GlobalInvocationID.x) / image_size_float.x)),\n"
                                   "                       abs(sin(     time + float(gl_GlobalInvocationID.y) / image_size_float.y)),\n"
                                   "                       abs(cos(sqrt(time))),\n"
                                   "                       1.0);\n"
                                   "\n"
                                   "    imageStore(image,\n"
                                   "               ivec2(gl_GlobalInvocationID.xy),\n"
                                   "               result);\n"
                                   "}\n";

GLint g_workgroup_size[3] = {0};


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "SILS" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    m_fbo_Id                  = 0;
    m_texture_Id              = 0;
    m_SILSSampleShaderProgram = 0;
    m_shader_Id               = 0;
    m_OpenGLESVersion         = GLES3;

#ifdef __ANDROID__
    fp_glDispatchCompute     = (PFNGLDISPATCHCOMPUTEOESPROC)  eglGetProcAddress("glDispatchCompute");
    fp_glMemoryBarrier       = (PFNGLMEMORYBARRIEROESPROC)    eglGetProcAddress("glMemoryBarrier");
    fp_glBindImageTexture    = (PFNGLBINDIMAGETEXTUREOESPROC) eglGetProcAddress("glBindImageTexture");
#endif
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if (m_texture_Id != 0)
    {
       glDeleteTextures(1,
                        &m_texture_Id);

       m_texture_Id = 0;
    }

    /* Release the framebuffer object */
    if (m_fbo_Id != 0)
    {
        glDeleteFramebuffers(1,
                             &m_fbo_Id);

        m_fbo_Id = 0;
    }

    /* Release the shaders */
    if (m_shader_Id != 0)
    {
        glDeleteShader(m_shader_Id);

        m_shader_Id = 0;
    }

    /* Release the program object */
    if (m_SILSSampleShaderProgram != 0)
    {
        glDeleteProgram(m_SILSSampleShaderProgram);

        m_SILSSampleShaderProgram = 0;
    }

}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Create the font
    if (FALSE == m_Font.Create("Samples/Fonts/Tuffy12.pak"))
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;

    if (FALSE == resource.LoadFromFile("Samples/Textures/SILS.pak"))
        return FALSE;

    // Create the logo texture
    m_pLogoTexture = resource.GetTexture("Logo");

    // Setup the user interface
    if (FALSE == m_UserInterface.Initialize(&m_Font, m_strName) )
        return FALSE;

    m_UserInterface.AddOverlay(m_pLogoTexture->m_hTextureHandle,
                               -5,
                               -5,
                               m_pLogoTexture->m_nWidth,
                               m_pLogoTexture->m_nHeight);

    m_UserInterface.AddTextString("Press " FRM_FONT_KEY_0 " for Help",
                                  1.0f,
                                 -1.0f);

    m_UserInterface.AddHelpKey(FRM_FONT_KEY_STAR,
                               "Toggle Orientation" );

    // Initialize ES objects
    if (FALSE == InitShaders() )
        return FALSE;

    if (FALSE == InitTexture() )
        return FALSE;

    if (!InitFrameBuffer())
        return FALSE;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    /* Kick off compute shader-based rendering */
    UINT32  current_time_msec = 0;
    FLOAT32 current_time      = m_Timer.GetTime();

    glUseProgram(m_SILSSampleShaderProgram );

    glUniform1f (m_time_uniform_location,
                 current_time);

    glDispatchCompute(g_workgroup_size[0],
                      g_workgroup_size[1],
                      1);                       /* num_groups_z */

    /* Blit the multisample texture contents onto single-sampled back buffer */
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,
                      0);

    glMemoryBarrier  (GL_FRAMEBUFFER_BARRIER_BIT);
    glBlitFramebuffer(0,                        /* srcX0 */
                      0,                        /* srcY0 */
                      m_nWidth,
                      m_nHeight,
                      0,                        /* dstX0 */
                      0,                        /* dstY0 */
                      m_nWidth,
                      m_nHeight,
                      GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);              /* filter */

    // Update the timer
    m_Timer.MarkFrame();

    // Render the UI
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_UserInterface.Render(m_Timer.GetFrameRate() );
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: Invoked when window is resized
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    InitTexture();
    InitFrameBuffer();
    InitShaders();

    glViewport(0,         /* x */
               0,         /* y */
               m_nWidth,
               m_nHeight);

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: Update the Scene
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Process input
    UINT32 nButtons;
    UINT32 nPressedButtons;

    FrmGetInput(&m_Input,
                &nButtons,
                &nPressedButtons);

    // Toggle user interface
    if (nPressedButtons & INPUT_KEY_0)
        m_UserInterface.AdvanceState();
}


//--------------------------------------------------------------------------------------
// Name: CreateComputeShader()
// Desc: Create shader string
//--------------------------------------------------------------------------------------
void CSample::CreateComputeShader(std::string& shaderBody)
{
    /* Need to construct the compute shader by taking the template version
    *  and replacing LOCAL_SIZE_* tokens with actual values.
    *
    *  First, compute local size of the work-group in X & Y dimensions.
    */
    INT32 local_size_x                       = m_nWidth;
    INT32 local_size_y                       = m_nHeight;
    GLint max_compute_work_group_invocations = 0;

    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_compute_work_group_invocations);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_SIZE,         g_workgroup_size);

    if (local_size_x > g_workgroup_size[0])
    {
        local_size_x = g_workgroup_size[0];
    }

    if (local_size_y > g_workgroup_size[1])
    {
        local_size_y = g_workgroup_size[1];
    }

    if (local_size_x * local_size_y > max_compute_work_group_invocations)
    {
        local_size_y = max_compute_work_group_invocations / local_size_x;

        if (local_size_x > max_compute_work_group_invocations)
        {
            local_size_x = max_compute_work_group_invocations;
        }
    }

    /*
    * Now that we now the local work-group size, we can determine how many global
    * work-groups we actually need
    */
    g_workgroup_size[0] = (GLuint) (FLOAT32(m_nWidth)  / FLOAT32(local_size_x) + 0.5f);
    g_workgroup_size[1] = (GLuint) (FLOAT32(m_nHeight) / FLOAT32(local_size_y) + 0.5f);

    /* OK, we can now create the shader body */
    const char*       local_size_x_token = "LOCAL_SIZE_X";
    std::stringstream local_size_x_value;
    const char*       local_size_y_token = "LOCAL_SIZE_Y";
    std::stringstream local_size_y_value;
    size_t            offset             = std::string::npos;

    local_size_x_value << local_size_x;
    local_size_y_value << local_size_y;

    while ( (offset = shaderBody.find(local_size_x_token)) != std::string::npos)
    {
        shaderBody = shaderBody.replace(offset,
                                        strlen(local_size_x_token),
                                        local_size_x_value.str().c_str() );
    }

    while ( (offset = shaderBody.find(local_size_y_token)) != std::string::npos)
    {
        shaderBody = shaderBody.replace(offset,
                                        strlen(local_size_y_token),
                                        local_size_y_value.str().c_str() );
    }

}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::CreateShader(const GLuint shaderType, const CHAR* shaderSource)
{
    /* If there's any shader object initialized at this point, release
     * it before continuing */
    if (m_shader_Id != 0)
    {
        glDeleteShader(m_shader_Id);

        m_shader_Id = 0;
    }

    /* Create the shader object */
    m_shader_Id = glCreateShader(shaderType);

    /* Assign a source code to the object */
    glShaderSource (m_shader_Id,
                    1,           /* count */
                    &shaderSource,
                    NULL);

    /* Compile the shader */
    glCompileShader(m_shader_Id);

    /* Has the compilation succeeded? */
    GLint status = GL_FALSE;

    glGetShaderiv(m_shader_Id,
                  GL_COMPILE_STATUS,
                  &status);

    if (GL_FALSE == status)
    {
        return FALSE;
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: CheckFrameBufferStatus()
// Desc: check the completeness status of a framebuffer.
//--------------------------------------------------------------------------------------
BOOL CSample::CheckFrameBufferStatus()
{
    switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
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
// Name: InitFrameBuffer()
// Desc: Set up Frame Buffer
//--------------------------------------------------------------------------------------
BOOL CSample::InitFrameBuffer()
{
    if (m_fbo_Id != 0)
    {
       glDeleteFramebuffers(1,
                            &m_fbo_Id);

       m_fbo_Id = 0;
    }

    /* Set up a Framebuffer Object */
    glGenFramebuffers(1,
                      &m_fbo_Id);

    glBindFramebuffer(GL_READ_FRAMEBUFFER,
                      m_fbo_Id);

    glFramebufferTexture2D(GL_READ_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           m_texture_Id,
                           0); /* level */


    if (!CheckFrameBufferStatus())
       return FALSE;

   return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    /* Create the compute shader object */
	std::string shaderBody  = compute_shader_body;

    CreateComputeShader(shaderBody);

	if (!CreateShader(GL_COMPUTE_SHADER, shaderBody.c_str()))
    {
        return FALSE;
    }

    /* If there's any program object initialized at this point, release it
     * before continuing */
    if (m_SILSSampleShaderProgram != NULL)
    {
        glDeleteProgram(m_SILSSampleShaderProgram);

        m_SILSSampleShaderProgram = NULL;
    }

    /* Create the program object */
    m_SILSSampleShaderProgram = glCreateProgram();

    glAttachShader(m_SILSSampleShaderProgram, m_shader_Id);
    glLinkProgram (m_SILSSampleShaderProgram);

    /* Retrieve uniform locations */
    m_image_uniform_location = glGetUniformLocation(m_SILSSampleShaderProgram,
                                                    "image");

    if (-1 == m_image_uniform_location)
        return FALSE;

    m_time_uniform_location = glGetUniformLocation(m_SILSSampleShaderProgram,
                                                    "time");

    if (-1 == m_time_uniform_location)
       return FALSE;

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitTexture()
// Desc: Set up Texture
//--------------------------------------------------------------------------------------
BOOL CSample::InitTexture()
{
    if (m_texture_Id != 0)
    {
       glDeleteTextures(1,
                        &m_texture_Id);

       m_texture_Id = 0;
    }

    glGenTextures(1,
                 &m_texture_Id );

    glBindTexture(GL_TEXTURE_2D,
                  m_texture_Id);

    glTexStorage2D(GL_TEXTURE_2D,
                   1, /* levels */
                   GL_RGBA8,
                   m_nWidth,
                   m_nHeight);

    /* Set up image bindings */
    glBindImageTexture(0,            /* index    */
                       m_texture_Id,
                       0,            /* level    */
                       GL_FALSE,     /* layered  */
                       0,            /* layer    */
                       GL_WRITE_ONLY,
                       GL_RGBA8);

    return TRUE;
}
