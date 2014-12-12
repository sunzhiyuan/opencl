//--------------------------------------------------------------------------------------
// File: Scene.cpp
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2009 QUALCOMM Incorporated.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
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
#include <OpenCL/FrmKernel.h>
#include <FrmUtils.h>
#include "Scene.h"



//--------------------------------------------------------------------------------------
// Name: FrmCreateComputeGraphicsApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmComputeGraphicsApplication* FrmCreateComputeGraphicsApplicationInstance()
{
    return new CSample( "InteropCLGLES" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmComputeGraphicsApplication( strName )
{
    m_hGraphShader = 0;
    m_hGraphVBO    = 0;
    m_hGraphIBO    = 0;
    m_vboMem       = 0;
    m_pLogoTexture = NULL;

    m_nLineSegments = 1000; // Number of line segments in the graph

    m_commandQueue = 0;
    m_program = 0;
    m_kernel = 0;
}


//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    // Create the Graph shader
    {
        FRM_SHADER_ATTRIBUTE pShaderAttributes[] =
        {
            { "g_vPositionOS", FRM_VERTEX_POSITION },
        };
        const INT32 nNumShaderAttributes = sizeof(pShaderAttributes)/sizeof(pShaderAttributes[0]);

        if( FALSE == FrmCompileShaderProgramFromFile( "Samples/Shaders/Graph.vs",
                                                      "Samples/Shaders/Graph.fs",
                                                      &m_hGraphShader,
                                                      pShaderAttributes, nNumShaderAttributes ) )
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

    if( FALSE == FrmBuildComputeProgramFromFile( "Samples/Kernels/InteropCLGLES.cl", &m_program, m_context,
                                                  &m_devices[0], 1, "-cl-fast-relaxed-math" ) )
    {
        return FALSE;
    }

    m_kernel = clCreateKernel( m_program, "ComputeGraph" , &errNum );
    if ( errNum != CL_SUCCESS )
    {
        CHAR str[256];
        FrmSprintf( str, sizeof(str), "ERROR: Failed to create kernel 'ComputeGraph'.\n" );
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
    if( FALSE == resource.LoadFromFile( "Samples/Textures/InteropCLGLES.pak" ) )
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

    // Initialize the shaders
    if( FALSE == InitShaders() )
        return FALSE;

    // Initialize the kernels
    if( FALSE == InitKernels() )
        return FALSE;

    // Create the VBO - allocate, but do not fill with data.  The OpenCL kernel
    // will generate the data.
    glGenBuffers( 1, &m_hGraphVBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_hGraphVBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * m_nLineSegments, NULL, GL_STREAM_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    // Create the IBO - this is static so fill with data
    unsigned short* pIndices = new unsigned short[ m_nLineSegments * 2 - 2];
    for( INT32 i = 0; i < m_nLineSegments - 1; i++ )
    {
        pIndices[i * 2 + 0] = i;
        pIndices[i * 2 + 1] = (i + 1);
    }

    glGenBuffers( 1, &m_hGraphIBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_hGraphIBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * m_nLineSegments * 2 - 2, pIndices, GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

    // Finally create the OpenCL memory object pointing to the VBO
    cl_int errNum = 0;
    m_vboMem = clCreateFromGLBuffer( m_context, CL_MEM_READ_WRITE, m_hGraphVBO, &errNum );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error creating OpenCL memory object from GL VBO." );
        return FALSE;
    }

    delete [] pIndices;
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    // Initialize the viewport
    glViewport( 0, 0, m_nWidth, m_nHeight );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_hGraphShader ) glDeleteProgram( m_hGraphShader );
    if( m_hGraphVBO )    glDeleteBuffers( 1, &m_hGraphVBO );
    if( m_hGraphIBO )    glDeleteBuffers( 1, &m_hGraphIBO );
    if( m_pLogoTexture ) m_pLogoTexture->Release();

    if( m_program )         clReleaseProgram( m_program );
    if( m_kernel )          clReleaseKernel( m_kernel );
    if( m_commandQueue )    clReleaseCommandQueue( m_commandQueue );
}

//--------------------------------------------------------------------------------------
// Name: ComputeGraphVerts()
// Desc: Using OpenCL, generate the contents of the graph VBO
//--------------------------------------------------------------------------------------
BOOL CSample::ComputeGraphVerts( FLOAT32 nTime )
{
    cl_int errNum = 0;

    // Finish any GL rendering
    glFinish();

    // Acquire the VBO from OpenGL
    errNum = clEnqueueAcquireGLObjects( m_commandQueue, 1, &m_vboMem, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error acquiring VBO to OpenCL memory object." );
        return FALSE;
    }

    // Set the kernel arguments
    errNum |= clSetKernelArg( m_kernel, 0, sizeof(cl_mem), &m_vboMem );
    errNum |= clSetKernelArg( m_kernel, 1, sizeof(cl_float), &nTime );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error setting kernel arguments." );
        return FALSE;
    }

    // Launch the kernel to compute the vertices
    size_t globalWorkSize[1] = { m_nLineSegments };
    errNum = clEnqueueNDRangeKernel( m_commandQueue, m_kernel, 1, NULL, globalWorkSize,
                                     NULL, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error queuing kernel for execution." );
        return FALSE;
    }

    // Finish executing kernel
    clFinish( m_commandQueue );

    // Release the VBO back to OpenGLGL
    errNum = clEnqueueReleaseGLObjects( m_commandQueue, 1, &m_vboMem, 0, NULL, NULL );
    if( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error releasing VBO from OpenCL back to OpenGL." );
        return FALSE;
    }

    return TRUE;

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
    }

    // Generate the VBO with OpenCL
    ComputeGraphVerts( m_Timer.GetTime() );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    glDisable( GL_DEPTH_TEST );

    // Clear the backbuffer and depth-buffer
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Update the timer
    m_Timer.MarkFrame();

    // Render the user interface
    m_UserInterface.Render( m_Timer.GetFrameRate() );

    // Bind the graph shader
    glUseProgram( m_hGraphShader );

    // Bind the VBO computed with OpenCL (during Update()) and the IBO
    glBindBuffer( GL_ARRAY_BUFFER, m_hGraphVBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_hGraphIBO );

    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (VOID*) NULL );
    glEnableVertexAttribArray( 0 );

    glDrawElements( GL_LINES, m_nLineSegments * 2 - 2, GL_UNSIGNED_SHORT, (VOID*) NULL );

    glDisableVertexAttribArray( 0 );
    glUseProgram( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

