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
#include <FrmUtils.h>
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
    return new CSample( "Stencil Buffer" );
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

    g_hSkyTexture = 0;
    
    g_hSkyShaderProgram = 0;
    g_hSkyVertexLoc = 0;
    g_hSkyColorLoc = 1;
    g_hSkyTimeLoc = 0;
    
    g_hMaskShaderProgram = 0;
    g_hMaskVertexLoc = 0;

    g_strImageFileName =  "Tutorials/OpenGLES/Clouds.tga";
}

//--------------------------------------------------------------------------------------
// Name: InitShaders()
// Desc: Initialize the shaders
//--------------------------------------------------------------------------------------
BOOL CSample::InitShaders()
{
    g_strSkyVSProgram = 
    "uniform float g_fTime;	     										 \n"
    "																	 \n"
    "attribute vec4 g_vVertex;											 \n"
    "																	 \n"
    "varying vec2 g_vOutTexCoord;										 \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    gl_Position    = vec4( g_vVertex.x, -g_vVertex.y, 0.0, 1.0 );   \n"
    "	 g_vOutTexCoord = vec2( g_vVertex.z + g_fTime, g_vVertex.w );    \n"
    "}																	 \n";

    g_strSkyFSProgram = 
    "#ifdef GL_FRAGMENT_PRECISION_HIGH									 \n"
    "   precision highp float;											 \n"
    "#else																 \n"
    "   precision mediump float;										 \n"
    "#endif																 \n"
    "																	 \n"
    "uniform sampler2D g_sSkyTexture;    								 \n"
    "																	 \n"
    "varying vec2 g_vOutTexCoord;										 \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    gl_FragColor = texture2D( g_sSkyTexture, g_vOutTexCoord );      \n"
    "}																	 \n";

//--------------------------------------------------------------------------------------
// Name: g_strMaskVSProgram / g_strMaskFSProgram
// Desc: The vertex and fragment shader programs
//--------------------------------------------------------------------------------------
    g_strMaskVSProgram = 
    "attribute vec4 g_vVertex;											 \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    gl_Position  = vec4( g_vVertex.x, g_vVertex.y,                  \n"
    "                         g_vVertex.z, g_vVertex.w );                \n"
    "}																	 \n";

    g_strMaskFSProgram = 
    "#ifdef GL_FRAGMENT_PRECISION_HIGH									 \n"
    "   precision highp float;											 \n"
    "#else																 \n"
    "   precision mediump float;										 \n"
    "#endif																 \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    gl_FragColor = vec4( 1.0 );                                     \n"
    "}																	 \n";
    
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitializeSkyShader()
// Desc: Initializes the sky shader
//--------------------------------------------------------------------------------------
BOOL CSample::InitializeSkyShader()
{
    // Compile the shaders
    GLuint hSkyVertexShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( hSkyVertexShader, 1, &g_strSkyVSProgram, NULL );
    glCompileShader( hSkyVertexShader );

    GLuint hSkyFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( hSkyFragmentShader, 1, &g_strSkyFSProgram, NULL );
    glCompileShader( hSkyFragmentShader );

    // Check for compile success
    GLint nCompileResult = 0;
    glGetShaderiv( hSkyFragmentShader, GL_COMPILE_STATUS, &nCompileResult );
    if( !nCompileResult )
    {
        CHAR Log[1024];
        GLint nLength;
        glGetShaderInfoLog( hSkyFragmentShader, 1024, &nLength, Log );
        return FALSE;
    }

    // Attach the individual shaders to the common shader program
    g_hSkyShaderProgram = glCreateProgram();
    glAttachShader( g_hSkyShaderProgram, hSkyVertexShader );
    glAttachShader( g_hSkyShaderProgram, hSkyFragmentShader );

    // Init attributes BEFORE linking
    glBindAttribLocation( g_hSkyShaderProgram, g_hSkyVertexLoc, "g_vVertex" );

    // Link the vertex shader and fragment shader together
    glLinkProgram( g_hSkyShaderProgram );

    // Check for link success
    GLint nLinkResult = 0;
    glGetProgramiv( g_hSkyShaderProgram, GL_LINK_STATUS, &nLinkResult );
    if( !nLinkResult )
    {
        CHAR Log[1024];
        GLint nLength;
        glGetProgramInfoLog( g_hSkyShaderProgram, 1024, &nLength, Log );
        return FALSE;
    }

    g_hSkyTimeLoc = glGetUniformLocation( g_hSkyShaderProgram,  "g_fTime" );

    glDeleteShader( hSkyVertexShader );
    glDeleteShader( hSkyFragmentShader );

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitializeMaskShader()
// Desc: Initializes the mask shader
//--------------------------------------------------------------------------------------
BOOL CSample::InitializeMaskShader()
{
    // Compile the shaders
    GLuint hMaskVertexShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( hMaskVertexShader, 1, &g_strMaskVSProgram, NULL );
    glCompileShader( hMaskVertexShader );

    GLuint hMaskFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( hMaskFragmentShader, 1, &g_strMaskFSProgram, NULL );
    glCompileShader( hMaskFragmentShader );

    // Check for compile success
    GLint nCompileResult = 0;
    glGetShaderiv( hMaskFragmentShader, GL_COMPILE_STATUS, &nCompileResult );
    if( !nCompileResult )
    {
        CHAR Log[1024];
        GLint nLength;
        glGetShaderInfoLog( hMaskFragmentShader, 1024, &nLength, Log );
        return FALSE;
    }

    // Attach the individual shaders to the common shader program
    g_hMaskShaderProgram = glCreateProgram();
    glAttachShader( g_hMaskShaderProgram, hMaskVertexShader );
    glAttachShader( g_hMaskShaderProgram, hMaskFragmentShader );

    // Init attributes BEFORE linking
    glBindAttribLocation( g_hMaskShaderProgram, g_hMaskVertexLoc, "g_vVertex" );

    // Link the vertex shader and fragment shader together
    glLinkProgram( g_hMaskShaderProgram );

    // Check for link success
    GLint nLinkResult = 0;
    glGetProgramiv( g_hMaskShaderProgram, GL_LINK_STATUS, &nLinkResult );
    if( !nLinkResult )
    {
        CHAR Log[1024];
        GLint nLength;
        glGetProgramInfoLog( g_hMaskShaderProgram, 1024, &nLength, Log );
        return FALSE;
    }

    glDeleteShader( hMaskVertexShader );
    glDeleteShader( hMaskFragmentShader );

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc: 
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{

    InitShaders();

    // Create a texture, loaded from an image file
    {
        UINT32 nWidth, nHeight, nFormat;
        VOID* pImageData = FrmUtils_LoadTGA( g_strImageFileName, &nWidth, &nHeight, &nFormat );
        if( NULL == pImageData )
            return FALSE;

        glGenTextures( 1, &g_hSkyTexture );
        glBindTexture( GL_TEXTURE_2D, g_hSkyTexture );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT );
        glTexImage2D( GL_TEXTURE_2D, 0, nFormat, nWidth, nHeight,
                      0, nFormat, GL_UNSIGNED_BYTE, pImageData );
        delete[] pImageData;
    }

    // Create the shader programs
    {
        if( !InitializeSkyShader() )
        {
            return FALSE;
        }
        if( !InitializeMaskShader() )
        {
            return FALSE;
        }
    }

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glDisable( GL_DEPTH_TEST );
    glStencilMask( 0xFFFFFFFF );
    glEnable( GL_STENCIL_TEST );

    // Render our 2 window masks to the stencil buffer
    {
        glClear( GL_STENCIL_BUFFER_BIT );

        glStencilFunc( GL_ALWAYS, 0, 0 );
        glStencilOp( GL_INCR, GL_INCR, GL_INCR );

        FLOAT Window1VertexPositions[] =
        {
            -0.75f, -0.75, 0.0f, 1.0f,
            -0.75f,  0.75, 0.0f, 1.0f,
            -0.10f, -0.75, 0.0f, 1.0f,
            -0.10f,  0.75, 0.0f, 1.0f,
        };

        FLOAT Window2VertexPositions[] =
        {
            0.75f, -0.75, 0.0f, 1.0f,
            0.75f,  0.75, 0.0f, 1.0f,
            0.10f, -0.75, 0.0f, 1.0f,
            0.10f,  0.75, 0.0f, 1.0f,
        };

        // Don't write to our color buffer, we want to write to the stencil buffer
        glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

        // Set the shader program
        glUseProgram( g_hMaskShaderProgram );

        glVertexAttribPointer( g_hMaskVertexLoc, 4, GL_FLOAT, 0, 0, Window1VertexPositions );
        glEnableVertexAttribArray( g_hMaskVertexLoc );

        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

        glVertexAttribPointer( g_hMaskVertexLoc, 4, GL_FLOAT, 0, 0, Window2VertexPositions );
        glEnableVertexAttribArray( g_hMaskVertexLoc );

        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

        glDisableVertexAttribArray( g_hMaskVertexLoc );

        glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    }

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
    glDeleteTextures( 1, &g_hSkyTexture );

    glDeleteProgram( g_hSkyShaderProgram );
    glDeleteProgram( g_hMaskShaderProgram );
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
// Name: Render()
// Desc: 
//--------------------------------------------------------------------------------------
VOID CSample::Render()
{
    FLOAT fTime = FrmGetTime() / 10.0f;
    glClear( GL_COLOR_BUFFER_BIT );

    // Draw our screen aligned quad only where our mask is
    glStencilFunc( GL_NOTEQUAL, 0, 0xFFFFFFFF );
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

    FLOAT ScreenAlignedQuadVertexPositions[] =
    {
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
    };

    FLOAT ScreenAlignedQuadVertexColors[] = 
    { 
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    // Set the shader program
    glUseProgram( g_hSkyShaderProgram );
    glBindTexture( GL_TEXTURE_2D, g_hSkyTexture );


    glUniform1f( g_hSkyTimeLoc, fTime );

    glVertexAttribPointer( g_hSkyVertexLoc, 4, GL_FLOAT, 0, 0, ScreenAlignedQuadVertexPositions );
    glEnableVertexAttribArray( g_hSkyVertexLoc );

    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    glDisableVertexAttribArray( g_hSkyVertexLoc );
}

