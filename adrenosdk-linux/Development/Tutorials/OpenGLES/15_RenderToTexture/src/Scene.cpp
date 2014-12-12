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
#include <FrmApplication.h>
#include <OpenGLES/FrmShader.h>
#include "Scene.h"

#if defined(__linux__)
#include <stdio.h>
#include <unistd.h>
#endif

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmApplication* FrmCreateApplicationInstance()
{
    return new CSample( "RenderToTexture" );
}

//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmApplication( strName )
{
    g_strWindowTitle = strName;
    g_nWindowWidth   = 765;
    g_nWindowHeight  = 480;
    g_fAspectRatio   = (FLOAT)g_nWindowWidth / (FLOAT)g_nWindowHeight;


    g_pOffscreenFBO = NULL;

    g_hShaderProgram        = 0;
    g_hOverlayShaderProgram = 0;
    g_hModelViewMatrixLoc   = 0;
    g_hProjMatrixLoc        = 0;
    g_hVertexLoc            = 0;
    g_hColorLoc             = 1;

}

//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    g_strVertexShader = 
    "uniform   mat4 g_matModelView;                                      \n"
    "uniform   mat4 g_matProj;                                           \n"
    "                                                                    \n"
    "attribute vec4 g_vPosition;	 								     \n"
    "attribute vec3 g_vColor; 			   		 		                 \n"
    "                                                                    \n"
    "varying   vec3 g_vVSColor;                                          \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    vec4 vPositionES = g_matModelView * g_vPosition;                \n"
    "    gl_Position  = g_matProj * vPositionES;                         \n"
    "    g_vVSColor = g_vColor;                                          \n"
    "}																   	 \n";

    g_strFragmentShader = 
    "#ifdef GL_FRAGMENT_PRECISION_HIGH									 \n"
    "   precision highp float;											 \n"
    "#else																 \n"
    "   precision mediump float;										 \n"
    "#endif																 \n"
    "																	 \n"
    "varying   vec3      g_vVSColor;                                     \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    gl_FragColor = vec4( g_vVSColor, 1.0 );                         \n"
    "}																	 \n";


//--------------------------------------------------------------------------------------
// Name: g_strOverlayVertexShader / g_strOverlayFragmentShader
// Desc: The vertex and fragment shader programs
//--------------------------------------------------------------------------------------
    g_strOverlayVertexShader =
    "attribute vec4 g_vVertex;                               \n"
    "varying   vec2 g_vTexCoord;                             \n"
    "														 \n"
    "void main()											 \n"
    "{														 \n"
    "    gl_Position.x = g_vVertex.x;                        \n"
    "    gl_Position.y = g_vVertex.y;                        \n"
    "    gl_Position.z = 0.0;                                \n"
    "    gl_Position.w = 1.0;                                \n"
    "                                                        \n"
    "    g_vTexCoord.x = g_vVertex.z;                        \n"
    "    g_vTexCoord.y = g_vVertex.w;                        \n"
    "}                                                       \n";

    g_strOverlayFragmentShader =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH						 \n"
    "   precision highp float;					             \n"
    "#else													 \n"
    "   precision mediump float;					         \n"
    "#endif							                         \n"
    "uniform sampler2D g_Texture;                            \n"
    "varying vec2      g_vTexCoord;                          \n"
    "                                                        \n"
    "void main()                                             \n"
    "{                                                       \n"
    "    gl_FragColor = texture2D( g_Texture, g_vTexCoord ); \n"
    "}                                                       \n";
    
    return TRUE;
}



//--------------------------------------------------------------------------------------
// Name: CreateFBO()
// Desc: Create objects needed for rendering to an FBO
//--------------------------------------------------------------------------------------
BOOL CSample::CreateFBO( UINT32 nWidth, UINT32 nHeight, UINT32 nFormat, UINT32 nType,
                FrameBufferObject** ppFBO )
{
    (*ppFBO) = new FrameBufferObject;
    (*ppFBO)->m_nWidth  = nWidth;
    (*ppFBO)->m_nHeight = nHeight;

    // Create an offscreen texture
    glGenTextures( 1, &(*ppFBO)->m_hTexture );
    glBindTexture( GL_TEXTURE_2D, (*ppFBO)->m_hTexture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE );
    glTexImage2D( GL_TEXTURE_2D, 0, nFormat, nWidth, nHeight, 0, nFormat, nType, NULL );

    // Create the depth buffer
    glGenRenderbuffers( 1, &(*ppFBO)->m_hRenderBuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, (*ppFBO)->m_hRenderBuffer );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, nWidth, nHeight );

    // Create the color buffer
    glGenFramebuffers( 1, &(*ppFBO)->m_hFrameBuffer );
    glBindFramebuffer( GL_FRAMEBUFFER, (*ppFBO)->m_hFrameBuffer );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (*ppFBO)->m_hTexture, 0 );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, (*ppFBO)->m_hRenderBuffer );

    if( GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus( GL_FRAMEBUFFER ) )
        return FALSE;

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: DestroyFBO()
// Desc: Destroy the FBO
//--------------------------------------------------------------------------------------
VOID CSample::DestroyFBO( FrameBufferObject* pFBO )
{
    glDeleteTextures( 1, &pFBO->m_hTexture );
    glDeleteFramebuffers( 1, &pFBO->m_hFrameBuffer );
    glDeleteRenderbuffers( 1, &pFBO->m_hRenderBuffer );
    delete pFBO;
}


//--------------------------------------------------------------------------------------
// Name: BeginFBO()
// Desc: Bind objects for rendering to the frame buffer object
//--------------------------------------------------------------------------------------
VOID CSample::BeginFBO( FrameBufferObject* pFBO )
{
    glBindFramebuffer( GL_FRAMEBUFFER, pFBO->m_hFrameBuffer );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pFBO->m_hTexture, 0 );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pFBO->m_hRenderBuffer );
    glViewport( 0, 0, pFBO->m_nWidth, pFBO->m_nHeight );
}


//--------------------------------------------------------------------------------------
// Name: EndFBO()
// Desc: Restore rendering back to the primary frame buffer
//--------------------------------------------------------------------------------------
VOID CSample::EndFBO( FrameBufferObject* pFBO )
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, g_nWindowWidth, g_nWindowHeight );
}



//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{

    InitShaders();

     // Create the shader program needed to render the scene
    {
        // Compile the shaders
        GLuint hVertexShader = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( hVertexShader, 1, &g_strVertexShader, NULL );
        glCompileShader( hVertexShader );

        // Check for compile success
        GLint nCompileResult = 0;
        glGetShaderiv( hVertexShader, GL_COMPILE_STATUS, &nCompileResult );
        if( 0 == nCompileResult )
        {
            CHAR  strLog[1024];
            GLint nLength;
            glGetShaderInfoLog( hVertexShader, 1024, &nLength, strLog );
            return FALSE;
        }

        GLuint hFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( hFragmentShader, 1, &g_strFragmentShader, NULL );
        glCompileShader( hFragmentShader );

        // Check for compile success
        glGetShaderiv( hFragmentShader, GL_COMPILE_STATUS, &nCompileResult );
        if( 0 == nCompileResult )
        {
            CHAR  strLog[1024];
            GLint nLength;
            glGetShaderInfoLog( hFragmentShader, 1024, &nLength, strLog );
            return FALSE;
        }

        // Attach the individual shaders to the common shader program
        g_hShaderProgram = glCreateProgram();
        glAttachShader( g_hShaderProgram, hVertexShader );
        glAttachShader( g_hShaderProgram, hFragmentShader );

        // Init attributes BEFORE linking
        glBindAttribLocation( g_hShaderProgram, g_hVertexLoc,   "g_vPosition" );
        glBindAttribLocation( g_hShaderProgram, g_hColorLoc,    "g_vColor" );

        // Link the vertex shader and fragment shader together
        glLinkProgram( g_hShaderProgram );

        // Check for link success
        GLint nLinkResult = 0;
        glGetProgramiv( g_hShaderProgram, GL_LINK_STATUS, &nLinkResult );
        if( 0 == nLinkResult )
        {
            CHAR strLog[1024];
            GLint nLength;
            glGetProgramInfoLog( g_hShaderProgram, 1024, &nLength, strLog );
            return FALSE;
        }

        // Get uniform locations
        g_hModelViewMatrixLoc = glGetUniformLocation( g_hShaderProgram, "g_matModelView" );
        g_hProjMatrixLoc      = glGetUniformLocation( g_hShaderProgram, "g_matProj" );

        glDeleteShader( hVertexShader );
        glDeleteShader( hFragmentShader );
    }

    // Create the shader program needed to render the overlay
    {
        // Compile the shaders
        GLuint hVertexShader = glCreateShader( GL_VERTEX_SHADER );
        glShaderSource( hVertexShader, 1, &g_strOverlayVertexShader, NULL );
        glCompileShader( hVertexShader );

        // Check for compile success
        GLint nCompileResult = 0;
        glGetShaderiv( hVertexShader, GL_COMPILE_STATUS, &nCompileResult );
        if( 0 == nCompileResult )
        {
            CHAR  strLog[1024];
            GLint nLength;
            glGetShaderInfoLog( hVertexShader, 1024, &nLength, strLog );
            return FALSE;
        }

        GLuint hFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
        glShaderSource( hFragmentShader, 1, &g_strOverlayFragmentShader, NULL );
        glCompileShader( hFragmentShader );

        // Check for compile success
        glGetShaderiv( hFragmentShader, GL_COMPILE_STATUS, &nCompileResult );
        if( 0 == nCompileResult )
        {
            CHAR  strLog[1024];
            GLint nLength;
            glGetShaderInfoLog( hFragmentShader, 1024, &nLength, strLog );
            return FALSE;
        }

        // Attach the individual shaders to the common shader program
        g_hOverlayShaderProgram = glCreateProgram();
        glAttachShader( g_hOverlayShaderProgram, hVertexShader );
        glAttachShader( g_hOverlayShaderProgram, hFragmentShader );

        // Link the vertex shader and fragment shader together
        glLinkProgram( g_hOverlayShaderProgram );

        // Check for link success
        GLint nLinkResult = 0;
        glGetProgramiv( g_hOverlayShaderProgram, GL_LINK_STATUS, &nLinkResult );
        if( 0 == nLinkResult )
        {
            CHAR strLog[1024];
            GLint nLength;
            glGetProgramInfoLog( g_hOverlayShaderProgram, 1024, &nLength, strLog );
            return FALSE;
        }

        glDeleteShader( hVertexShader );
        glDeleteShader( hFragmentShader );
    }

    // Create an offscreen FBO to render to
    if( FALSE == CreateFBO( 128, 128, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, &g_pOffscreenFBO ) )
        return FALSE;

    return TRUE;
}



//--------------------------------------------------------------------------------------
// Name: Resize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Resize()
{
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    glDeleteProgram( g_hShaderProgram );
    DestroyFBO( g_pOffscreenFBO );
}


//--------------------------------------------------------------------------------------
// Name: Update()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Update()
{
   return;
}


//--------------------------------------------------------------------------------------
// Name: RenderScene()
// Desc: Render the scene
//--------------------------------------------------------------------------------------
VOID CSample::RenderScene( FLOAT fTime )
{
    // Rotate and translate the model view matrix
    float matModelView[16] = {0};
    matModelView[ 0] = +cosf( fTime );
    matModelView[ 2] = +sinf( fTime );
    matModelView[ 5] = 1.0f;
    matModelView[ 8] = -sinf( fTime );
    matModelView[10] = +cosf( fTime );
    matModelView[14] = -5.0f;
    matModelView[15] = 1.0f;

    // Build a perspective projection matrix
    float matProj[16] = {0};
    matProj[ 0] = cosf( 0.5f ) / sinf( 0.5f );
    matProj[ 5] = matProj[0] * g_fAspectRatio;
    matProj[10] = -( 10.0f ) / ( 9.0f );
    matProj[11] = -1.0f;
    matProj[14] = -( 10.0f ) / ( 9.0f );

    // Define vertice for a 4-sided pyramid
    FLOAT VertexPositions[] =
    {
        0.0f,+1.0f, 0.0f,      -1.0f,-1.0f, 1.0f,      +1.0f,-1.0f, 1.0f,
        0.0f,+1.0f, 0.0f,      +1.0f,-1.0f, 1.0f,      +1.0f,-1.0f,-1.0f,
        0.0f,+1.0f, 0.0f,      +1.0f,-1.0f,-1.0f,      -1.0f,-1.0f,-1.0f,
        0.0f,+1.0f, 0.0f,      -1.0f,-1.0f,-1.0f,      -1.0f,-1.0f, 1.0f,
    };
    FLOAT VertexColors[] =
    {
        1.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f,       1.0f, 0.0f, 0.0f, // Red
        0.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f,       0.0f, 1.0f, 0.0f, // Green
        0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 1.0f,       0.0f, 0.0f, 1.0f, // Blue
        1.0f, 1.0f, 0.0f,       1.0f, 1.0f, 0.0f,       1.0f, 1.0f, 0.0f, // Yellow
    };

    // Clear the colorbuffer and depth-buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set some state
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Set the shader program
    glUseProgram( g_hShaderProgram );
    glUniformMatrix4fv( g_hModelViewMatrixLoc, 1, FALSE, matModelView );
    glUniformMatrix4fv( g_hProjMatrixLoc,      1, FALSE, matProj );

    // Bind the vertex attributes
    glVertexAttribPointer( g_hVertexLoc, 3, GL_FLOAT, 0, 0, VertexPositions );
    glEnableVertexAttribArray( g_hVertexLoc );

    glVertexAttribPointer( g_hColorLoc, 3, GL_FLOAT, 0, 0, VertexColors );
    glEnableVertexAttribArray( g_hColorLoc );

    // Draw the cubemap-reflected triangle
    glDrawArrays( GL_TRIANGLES, 0, 12 );

    // Cleanup
    glDisableVertexAttribArray( g_hVertexLoc );
    glDisableVertexAttribArray( g_hColorLoc );
}


//--------------------------------------------------------------------------------------
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    static float fTime = 0.0f;
    fTime += 0.01f;

    // Render the scene into the offscreen FBO with a green background
    {
        BeginFBO( g_pOffscreenFBO );
        glClearColor( 0.0f, 0.5f, 0.0f, 1.0f );
        RenderScene( fTime );
        EndFBO( g_pOffscreenFBO );
    }

    // Render the scene into the primary backbuffer with a dark blue background
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
    RenderScene( fTime );

    // Display the offscreen FBO texture as an overlay
    {
        float vQuad[] = 
        {
            0.35f, 0.95f,  0.0f, 1.0f,
            0.35f, 0.35f,  0.0f, 0.0f,
            0.95f, 0.35f,  1.0f, 0.0f,
            0.95f, 0.95f,  1.0f, 1.0f,
        };
        glVertexAttribPointer( 0, 4, GL_FLOAT, 0, 0, vQuad );
        glEnableVertexAttribArray( 0 );

        // Set the texture
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, g_pOffscreenFBO->m_hTexture );
        glDisable( GL_DEPTH_TEST );

        // Set the shader program
        glUseProgram( g_hOverlayShaderProgram );

        // Draw the quad
        glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
    }
}

