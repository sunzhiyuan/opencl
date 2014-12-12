//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <CL/cl_gl.h>
#include <FrmComputeGraphicsApplication.h>
#include <OpenGLES/FrmFontGLES.h>
#include <OpenGLES/FrmMesh.h>
#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmShader.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>
#include <OpenGLES/FrmUtilsGLES.h>

#include <OpenCL/FrmKernel.h>
#include <FrmUtils.h>
#include "Scene.h"



//--------------------------------------------------------------------------------------
// Name: FrmCreateComputeGraphicsApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmComputeGraphicsApplication* FrmCreateComputeGraphicsApplicationInstance()
{
#ifdef NO_SHARED_BUFFER
    FrmLogMessage( "Starting app without CL/GL buffer sharing." );
    return new CSample( "PostProcessCLGLES", false );
#else
    FrmLogMessage( "Starting app with CL/GL buffer sharing." );
    return new CSample( "PostProcessCLGLES", true );
#endif // NO_SHARED_BUFFER
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName, BOOL sharedBuffer ) :
    CFrmComputeGraphicsApplication( strName ),
    m_sharedBuffer(sharedBuffer),
    m_pHostBuf(NULL)
{
    m_hTextureShader           = 0;
    m_hPerPixelLightingShader  = 0;
    m_qMeshRotation     = FRMVECTOR4( 0.0f, 0.0f, 0.0f, 1.0f );
    m_qRotationDelta    = FRMVECTOR4( 0.0f, 0.01f, 0.0f, 1.0f );
    m_vLightPosition.x  = 2.0f;
    m_vLightPosition.y  = 1.0f;
    m_vLightPosition.z  = 1.0f;

    m_pLogoTexture = NULL;

    m_pBufferId = 0;
    m_pTextureId = 0;
    m_pDepthBufferId = 0;

    m_nFBOWidth = 0;
    m_nFBOHeight = 0;

    m_commandQueue = 0;
    m_program = 0;
    m_kernel = 0;
    m_fboMem = 0;
    m_filteredTexMem = 0;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the PerPixelLighting shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        {
            { "g_vPositionOS", FRM_VERTEX_POSITION },
            { "g_vNormalOS",   FRM_VERTEX_NORMAL }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/PerPixelLighting.vs",
                                                      "Samples/Shaders/PerPixelLighting.fs",
                                                      &m_hPerPixelLightingShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
            return FALSE;

        // Make a record of the location for each shader constant
        m_hPPLModelViewMatrixLoc     = glGetUniformLocation( m_hPerPixelLightingShader, "g_matModelView" );
        m_hPPLModelViewProjMatrixLoc = glGetUniformLocation( m_hPerPixelLightingShader, "g_matModelViewProj" );
        m_hPPLNormalMatrixLoc        = glGetUniformLocation( m_hPerPixelLightingShader, "g_matNormal" );
        m_hPPLLightPositionLoc       = glGetUniformLocation( m_hPerPixelLightingShader, "g_vLightPos" );
        m_hPPLMaterialAmbientLoc     = glGetUniformLocation( m_hPerPixelLightingShader, "g_Material.vAmbient" );
        m_hPPLMaterialDiffuseLoc     = glGetUniformLocation( m_hPerPixelLightingShader, "g_Material.vDiffuse" );
        m_hPPLMaterialSpecularLoc    = glGetUniformLocation( m_hPerPixelLightingShader, "g_Material.vSpecular" );
    }

    // Create our Texture FBO shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        {
            { "g_vVertex", FRM_VERTEX_POSITION }
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Texture.vs",
                                                      "Samples/Shaders/Texture.fs",
                                                      &m_hTextureShader, pShaderAttributes,
                                                      nNumShaderAttributes ) )
            return FALSE;
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: InitKernels()
// Desc: Initialize the kernels
//--------------------------------------------------------------------------------------
BOOL CSample::InitKernels()
{
    cl_int errNum;

    // Create the command queue
    m_commandQueue = clCreateCommandQueue( m_context, m_devices[0], CL_QUEUE_PROFILING_ENABLE, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to create command queue" );
        return FALSE;
    }

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/PostProcessCLGLES.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }

    m_kernel = clCreateKernel( m_program, "SobelFilter" , &errNum );
    if ( errNum != CL_SUCCESS )
    {
        CHAR str[256];
        FrmSprintf( str, sizeof(str), "ERROR: Failed to create kernel 'SobelFilter'.\n" );
        FrmLogMessage( str );
        return FALSE;
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    // Create the font
    if( FALSE == m_Font.Create( "Samples/Fonts/Tuffy12.pak" ) )
        return FALSE;

    // Load the packed resources
    CFrmPackedResourceGLES resource;
    if( FALSE == resource.LoadFromFile( "Samples/Textures/PostProcessCLGLES.pak" ) )
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
    m_UserInterface.AddHelpKey( FRM_FONT_KEY_0, "Toggle Info Pane" );

    // Load the mesh
    if( FALSE == m_Mesh.Load( "Samples/Meshes/Phone.mesh" ) )
        return FALSE;
    if( FALSE == m_Mesh.MakeDrawable() )
        return FALSE;

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Initialize the kernels
    if( FALSE == InitKernels() )
        return FALSE;

    CreateFBO( m_nWidth, m_nHeight );

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: CreateFBO()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::CreateFBO( INT32 nWidth, INT32 nHeight )
{
    m_nFBOWidth = nWidth / 4;
    m_nFBOHeight = nHeight / 4;

    // Free the FBO, in case it was previously created
    FreeFBO();

    glGenFramebuffers( 1, &m_pBufferId );
    glBindFramebuffer( GL_FRAMEBUFFER, m_pBufferId );

    glGenTextures( 1, &( m_pTextureId ) );
    glBindTexture( GL_TEXTURE_2D, m_pTextureId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_nFBOWidth, m_nFBOHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pTextureId );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTextureId, 0 );

    glGenRenderbuffers(1, &m_pDepthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, m_pDepthBufferId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_nFBOWidth, m_nFBOHeight );
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_pDepthBufferId);

    CheckFrameBufferStatus();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    // Create another texture to hold the result of running the OpenCL filter kernel
    glGenTextures( 1, &( m_pFilteredTextureId ) );
    glBindTexture( GL_TEXTURE_2D, m_pFilteredTextureId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_nFBOWidth, m_nFBOHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glBindTexture( GL_TEXTURE_2D, 0 );


    // Create the OpenCL resource from the FBO
    cl_int errNum = 0;

    if (m_sharedBuffer)
    {
        // use memory already allocated in GL context
        m_fboMem = clCreateFromGLTexture2D( m_context, CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, m_pTextureId, &errNum );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error creating OpenCL image object from GL FBO." );
            return FALSE;
        }

        m_filteredTexMem = clCreateFromGLTexture2D( m_context, CL_MEM_WRITE_ONLY, GL_TEXTURE_2D, 0, m_pFilteredTextureId, &errNum );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error creating OpenCL image object from GL texture." );
            return FALSE;
        }
    }
    else // allocate new memory in cl context
    {
        cl_image_format image_format = {CL_RGBA, CL_UNORM_INT8};
        //CL 1.1
        m_fboMem = clCreateImage2D (m_context, CL_MEM_READ_ONLY, &image_format, m_nFBOWidth, m_nFBOHeight, 0, 0, &errNum);
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error creating CL image object." );
            return FALSE;
        }

        //CL 1.1
        m_filteredTexMem = clCreateImage2D (m_context, CL_MEM_WRITE_ONLY, &image_format, m_nFBOWidth, m_nFBOHeight, 0, 0, &errNum);
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error creating CL image object." );
            return FALSE;
        }

        if (!m_pHostBuf) { m_pHostBuf = new BYTE[m_nFBOWidth*m_nFBOHeight*4]; }
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::FreeFBO( )
{
    // Release OpenCL pointer to FBO memory
    if( m_fboMem )          clReleaseMemObject( m_fboMem );
    if( m_filteredTexMem )  clReleaseMemObject( m_filteredTexMem );


    // Release OpenGL ES FBO
    if( m_pBufferId )           glDeleteFramebuffers( 1, &m_pBufferId );
    if( m_pTextureId )          glDeleteTextures( 1, &m_pTextureId );
    if( m_pFilteredTextureId )  glDeleteTextures( 1, &m_pFilteredTextureId );
    if( m_pDepthBufferId )      glDeleteRenderbuffers( 1, &m_pDepthBufferId );
}

//--------------------------------------------------------------------------------------
// Name:
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::CheckFrameBufferStatus()
{
    GLenum nStatus;
    nStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    switch( nStatus )
    {
    case GL_FRAMEBUFFER_COMPLETE:
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        break;
    default:
        FrmAssert(0);
    }
}



//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    CreateFBO( m_nWidth, m_nHeight );

    FRMVECTOR3 vPosition = FRMVECTOR3( 0.0f, 0.1f, 0.5f );
    FRMVECTOR3 vLookAt   = FRMVECTOR3( 0.0f, 0.0f, 0.0f );
    FRMVECTOR3 vUp       = FRMVECTOR3( 0.0f, 1.0f,  0.0f );
    m_matView     = FrmMatrixLookAtRH( vPosition, vLookAt, vUp );

    FLOAT32 fAspect = (FLOAT32)m_nWidth / (FLOAT32)m_nHeight;
    m_matProj = FrmMatrixPerspectiveFovRH( FRM_PI/4.0f, fAspect, 0.1f, 10.0f );

    // To keep sizes consistent, scale the projection matrix in landscape oriention
    if( fAspect > 1.0f )
    {
        m_matProj.M(0,0) *= fAspect;
        m_matProj.M(1,1) *= fAspect;
    }

    m_matViewProj = FrmMatrixMultiply( m_matView, m_matProj );

    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Resize the arc ball control
    m_ArcBall.Resize( m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_hPerPixelLightingShader )  glDeleteProgram( m_hPerPixelLightingShader );
    if( m_hTextureShader )           glDeleteProgram( m_hTextureShader );

    if( m_pLogoTexture ) m_pLogoTexture->Release();

    if( m_program )         clReleaseProgram( m_program );
    if( m_kernel )          clReleaseKernel( m_kernel );
    if( m_commandQueue )    clReleaseCommandQueue( m_commandQueue );

    FreeFBO();

    if (m_pHostBuf)
    {
        delete [] m_pHostBuf;
        m_pHostBuf = NULL;
    }
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
    // Get the current time
    FLOAT32 fElapsedTime = m_Timer.GetFrameElapsedTime();

    // Process input
    {
        // Get button input since the last frame
        UINT32 nButtons;
        UINT32 nPressedButtons;
        m_Input.GetButtonState( &nButtons, &nPressedButtons );

        // Get pointer input since the last frame
        UINT32     nPointerState;
        FRMVECTOR2 vPointerPosition;
        m_Input.GetPointerState( &nPointerState, &vPointerPosition );

        // Conditionally advance the UI state
        m_UserInterface.HandleInput( m_nWidth, m_nHeight, nPressedButtons,
                                     nPointerState, vPointerPosition );

        // Use the pointer input to rotate the object
        FRMVECTOR4 qArcBallDelta = m_ArcBall.HandleInput( vPointerPosition, nPointerState & FRM_INPUT::POINTER_DOWN );
        if( m_ArcBall.IsDragging() )
            m_qRotationDelta = qArcBallDelta;
        FRMVECTOR4 qDelta = nPointerState ? qArcBallDelta : m_qRotationDelta;

        qDelta = FrmVector4Normalize( qDelta );
        m_qMeshRotation = FrmQuaternionMultiply( m_qMeshRotation, qDelta );

        // Move the light up/down/left/right
        if( nButtons & FRM_INPUT::DPAD_UP )
            m_vLightPosition.y = FrmMin( +5.0f, m_vLightPosition.y + 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_DOWN )
            m_vLightPosition.y = FrmMax( -5.0f, m_vLightPosition.y - 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_LEFT )
            m_vLightPosition.x = FrmMax( -5.0f, m_vLightPosition.x - 5.0f * fElapsedTime );
        if( nButtons & FRM_INPUT::DPAD_RIGHT )
            m_vLightPosition.x = FrmMin( +5.0f, m_vLightPosition.x + 5.0f * fElapsedTime );
    }

    // Orientate the object
    FRMMATRIX4X4 matRotate    = FrmMatrixRotate( m_qMeshRotation );
    FRMMATRIX4X4 matTranslate = FrmMatrixTranslate( 0.0f, -0.07f, 0.0f );

    // Build the matrices
    FRMMATRIX4X4 matModel = FrmMatrixMultiply( matTranslate,   matRotate );
    m_matModelView        = FrmMatrixMultiply( matModel,       m_matView );
    m_matModelViewProj    = FrmMatrixMultiply( m_matModelView, m_matProj );
    m_matNormal           = FrmMatrixNormal( m_matModelView );
}

//--------------------------------------------------------------------------------------
// Name: RenderScreenAlignedQuad()
// Desc: Render full screen quad
//--------------------------------------------------------------------------------------
VOID CSample::RenderScreenAlignedQuad()
{
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    const FLOAT32 Quad[] =
    {
        -1.0, -1.0f, 0.0f, 1.0f,
        +1.0, -1.0f, 1.0f, 1.0f,
        +1.0, +1.0f, 1.0f, 0.0f,
        -1.0, +1.0f, 0.0f, 0.0f,
    };

    glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, Quad );
    glEnableVertexAttribArray( 0 );

    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );

    glDisableVertexAttribArray( 0 );

    glDisable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
}

//--------------------------------------------------------------------------------------
// Name: PostProcessFBOOpenCL()
// Desc: Postprocess the FBO with a Sobel filter using OpenCL
//--------------------------------------------------------------------------------------
BOOL CSample::PostProcessFBOOpenCL()
{
    cl_int errNum = 0;

    // Finish any GL rendering
    glFinish();

    // Acquire the FBO and texture from OpenGL
    cl_mem memObjects[2] =
    {
        m_fboMem,
        m_filteredTexMem
    };
    size_t origin[] = {0, 0, 0};
    size_t region[] = {m_nFBOWidth, m_nFBOHeight, 1};

    if (m_sharedBuffer)
    {
        errNum = clEnqueueAcquireGLObjects( m_commandQueue, 2, &memObjects[0], 0, NULL, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error acquiring FBO and texture to OpenCL memory object." );
            return FALSE;
        }
    }
    else
    {
        // copy host memory to CL
        errNum = clEnqueueWriteImage (m_commandQueue, m_fboMem,
                  CL_TRUE,origin, region,
                  0, 0, m_pHostBuf,
                  0 ,    0 , 0);
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error writing image to buffer" );
            return FALSE;
        }
    }

    // Set the kernel arguments
    errNum |= clSetKernelArg( m_kernel, 0, sizeof(cl_mem), &m_fboMem );
    errNum |= clSetKernelArg( m_kernel, 1, sizeof(cl_mem), &m_filteredTexMem );
    errNum |= clSetKernelArg( m_kernel, 2, sizeof(cl_int), &m_nFBOWidth );
    errNum |= clSetKernelArg( m_kernel, 3, sizeof(cl_int), &m_nFBOHeight );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments." );
        return FALSE;
    }

    // Launch the kernel to compute the vertices
    size_t globalWorkSize[2] = { m_nFBOWidth, m_nFBOHeight };
    errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernel, 2, NULL, globalWorkSize,
                                     NULL, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error queuing kernel for execution." );
        return FALSE;
    }

    // Finish executing kernel
    clFinish( m_commandQueue );

    if (m_sharedBuffer)
    {
        // Release the VBO back to OpenGLGL
        errNum = clEnqueueReleaseGLObjects( m_commandQueue, 2, &memObjects[0], 0, NULL, NULL );
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error releasing VBO and texture from OpenCL back to OpenGL." );
            return FALSE;
        }
    }
    else
    {
        // copy CL buffer back to host memory
        errNum = clEnqueueReadImage (m_commandQueue, m_filteredTexMem,
                  CL_TRUE,origin, region,
                  0, 0, m_pHostBuf,
                  0, NULL, NULL);
        if( errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error reading image from buffer" );
            return FALSE;
        }
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    // Render to the FBO
    glBindFramebuffer( GL_FRAMEBUFFER, m_pBufferId );
    glViewport( 0, 0, m_nFBOWidth, m_nFBOHeight );


    // Clear the color and depth-buffer
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram( m_hPerPixelLightingShader );

    FRMVECTOR4 vAmbient = FRMVECTOR4( 0.0f, 0.1f, 0.25f, 1.0f );
    FRMVECTOR4 vDiffuse = FRMVECTOR4( 0.0f, 0.4f, 1.0f, 1.0f );
    FRMVECTOR4 vSpecular = FRMVECTOR4( 1.0f, 1.0f, 1.0f, 20.0f );

    glUniform4fv( m_hPPLMaterialAmbientLoc,  1, (FLOAT32*)&vAmbient );
    glUniform4fv( m_hPPLMaterialDiffuseLoc,  1, (FLOAT32*)&vDiffuse );
    glUniform4fv( m_hPPLMaterialSpecularLoc, 1, (FLOAT32*)&vSpecular );
    glUniformMatrix4fv( m_hPPLModelViewMatrixLoc,     1, FALSE, (FLOAT32*)&m_matModelView );
    glUniformMatrix4fv( m_hPPLModelViewProjMatrixLoc, 1, FALSE, (FLOAT32*)&m_matModelViewProj );
    glUniformMatrix3fv( m_hPPLNormalMatrixLoc,        1, FALSE, (FLOAT32*)&m_matNormal );
    glUniform3fv( m_hPPLLightPositionLoc, 1, (FLOAT32*)&m_vLightPosition );

    m_Mesh.Render();

    if (!m_sharedBuffer)
    {
        // copy GL memory to host memory
        glReadPixels(0, 0, m_nFBOWidth, m_nFBOHeight, GL_RGBA, GL_UNSIGNED_BYTE, m_pHostBuf);
    }

    // Unbind the FBO
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    // Postprocess the rendered image using OpenCL
    PostProcessFBOOpenCL();

    glViewport( 0, 0, m_nWidth, m_nHeight );

    // Clear the backbuffer
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Bind the result of rendering to the FBO
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_pFilteredTextureId );

    if (!m_sharedBuffer)
    {
        // copy host memory back to GL
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_nFBOWidth, m_nFBOHeight,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, m_pHostBuf );
    }

    glUseProgram( m_hTextureShader );

    // Render the FBO color buffer to the screen
    RenderScreenAlignedQuad();

    glBindTexture( GL_TEXTURE_2D, 0 );
    glUseProgram( 0 );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );
}

