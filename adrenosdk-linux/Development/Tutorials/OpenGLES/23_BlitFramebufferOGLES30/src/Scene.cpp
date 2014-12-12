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
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>
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
    
//--------------------------------------------------------------------------------------
// Name: g_strVertexShader / g_strFragmentShader
// Desc: The vertex and fragment shader programs
//--------------------------------------------------------------------------------------
    g_strVertexShader = 
	"#version 300 es													 \n"
    "uniform   mat4 g_matModelView;                                      \n"
    "uniform   mat4 g_matProj;                                           \n"
    "                                                                    \n"
    "in vec4 g_vPosition;	 											 \n"
    "in vec3 g_vColor; 			   		 								 \n"
    "                                                                    \n"
    "out   vec3 g_vVSColor;												 \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    vec4 vPositionES = g_matModelView * g_vPosition;                \n"
    "    gl_Position  = g_matProj * vPositionES;                         \n"
    "    g_vVSColor = g_vColor;                                          \n"
    "}																   	 \n";

    g_strFragmentShader = 
	"#version 300 es													 \n"
    "#ifdef GL_FRAGMENT_PRECISION_HIGH									 \n"
    "   precision highp float;											 \n"
    "#else																 \n"
    "   precision mediump float;										 \n"
    "#endif																 \n"
    "																	 \n"
    "in   vec3      g_vVSColor;											 \n"
	"out  vec4      g_FragColor;										 \n"
    "																	 \n"
    "void main()														 \n"
    "{																	 \n"
    "    g_FragColor = vec4( g_vVSColor, 1.0 );                          \n"
    "}																	 \n";


//--------------------------------------------------------------------------------------
// Name: g_strOverlayVertexShader / g_strOverlayFragmentShader
// Desc: The vertex and fragment shader programs
//--------------------------------------------------------------------------------------
    g_strOverlayVertexShader =
	"#version 300 es									     \n"
    "in        vec4 g_vVertex;                               \n"
    "out       vec2 g_vTexCoord;                             \n"
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
	"#version 300 es														\n"
    "#ifdef GL_FRAGMENT_PRECISION_HIGH										\n"
    "   precision highp float;												\n"
    "#else																	\n"
    "   precision mediump float;											\n"
    "#endif																	\n"
    
// Must define the sampler according to the format of the texture
#ifdef FB_TYPE_INT
	#ifdef FB_SIGNED
	"uniform isampler2D g_Texture;											\n"
	#else
	"uniform usampler2D g_Texture;											\n"
	#endif
#else
    "uniform sampler2D g_Texture;											\n"
#endif
    "in    vec2      g_vTexCoord;											\n"    
    "out vec4 out_color;                                                    \n"
#ifndef FB_FORMAT_SRGBA
	"void main()															\n"
    "{																		\n"
    "    out_color = texture( g_Texture, g_vTexCoord );			    		\n"
    "}																		\n";

#else
	// Perform the linear to sRGB conversion within the shader because
	//		the framebuffer we are rendering into is flagged as in linear
	//		color space, so no automatic conversion will occur.
	"void main()															    \n"
    "{																		    \n"
	"	 float c = 0.0031308;												    \n"
	"	 float cf = 12.92;													    \n"
	"	 float a = 0.055;													    \n"
	"	 float e = 0.41666;													    \n"
	"																		    \n"
	"    vec4 texel = texture( g_Texture, g_vTexCoord );					    \n"
	"	 vec3 finalColor;													    \n"
	"	 finalColor.r = (texel.r<c)? cf*texel.r : (1.0+a)*pow(texel.r, e)-a;	\n"
	"	 finalColor.g = (texel.g<c)? cf*texel.g : (1.0+a)*pow(texel.g, e)-a;	\n"
	"	 finalColor.b = (texel.b<c)? cf*texel.b : (1.0+a)*pow(texel.b, e)-a;	\n"
	"	 out_color  = vec4(finalColor, texel.a);							    \n"
	"}																		    \n";
#endif
    return TRUE;
}



//--------------------------------------------------------------------------------------
// Name: CreateFBO()
// Desc: Create objects needed for rendering to an FBO
//--------------------------------------------------------------------------------------
BOOL CSample::CreateFBO( UINT32 nWidth, UINT32 nHeight, UINT32 nFormat, UINT32 nType,
				UINT32 nInternalFormat, FrameBufferObject** ppFBO )
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
    glTexImage2D( GL_TEXTURE_2D, 0, nInternalFormat, nWidth, nHeight, 0, nFormat, nType, NULL );

    // Create the depth buffer
    glGenRenderbuffers( 1, &(*ppFBO)->m_hRenderBuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, (*ppFBO)->m_hRenderBuffer );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, nWidth, nHeight );

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
    /*glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pFBO->m_hTexture, 0 );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pFBO->m_hRenderBuffer );*/
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
    if( FALSE == CreateFBO( 128, 128, g_format, g_type, g_internalFormat, &g_pOffscreenFBO ) )
        return FALSE;
    
    
    /*if( FALSE == CreateFBO( 128, 128, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, GL_RGB565, &g_pOffscreenFBO ) )
        return FALSE;*/

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

	// Bind the vertex attributes
	glGenVertexArrays(1, &g_hVao);
	glBindVertexArray(g_hVao);
	
	glGenBuffers(1, &g_hPositionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_hPositionVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 36, VertexPositions, GL_DYNAMIC_DRAW);
    glVertexAttribPointer( g_hVertexLoc, 3, GL_FLOAT, false, 12, 0 );
    glEnableVertexAttribArray( g_hVertexLoc );
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &g_hColorVbo);
	glBindBuffer(GL_ARRAY_BUFFER, g_hColorVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(FLOAT32) * 36, VertexColors, GL_DYNAMIC_DRAW);
    glVertexAttribPointer( g_hColorLoc, 3, GL_FLOAT, false, 12, 0 );
    glEnableVertexAttribArray( g_hColorLoc );
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

	glBindVertexArray(g_hVao);

    // Draw the cubemap-reflected triangle
    glDrawArrays( GL_TRIANGLES, 0, 12 );

    // Cleanup
    glBindVertexArray(0);
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
        
        /*
        Tim- used to test if GL_SRGB was enabled automatically when an SRGB FBO is bound
        
        GLint fbap = 0;
        glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, &fbap);
        */
        
        glClearColor( 0.0f, 0.5f, 0.0f, 1.0f );
        RenderScene( fTime );
        EndFBO( g_pOffscreenFBO );
    }

    // Render the scene into the primary backbuffer with a dark blue background
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
    RenderScene( fTime );

#if 0
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
#else
	
	// Bind the offscreen FBO to the read framebuffer
	glBindFramebuffer( GL_READ_FRAMEBUFFER, g_pOffscreenFBO->m_hFrameBuffer );
	
	// Bind the default framebuffer as the draw framebuffer
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	
					   // specify the entire framebuffer from the offscreen FBO, and specify a small corner area for the blit target area
	glBlitFramebuffer( 
        (GLint)0, 
        (GLint)0, 
        (GLint)g_pOffscreenFBO->m_nWidth, 
        (GLint)g_pOffscreenFBO->m_nHeight,
		(GLint)INT32(0.675f*g_nWindowWidth), 
        (GLint)INT32(0.675f*g_nWindowHeight), 
        (GLint)INT32(0.975f*g_nWindowWidth), 
        (GLint)INT32(0.975f*g_nWindowHeight),
		GL_COLOR_BUFFER_BIT, 
        GL_LINEAR );	// Color only, and linear filtering
	
	// unbind the read framebuffer
	glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
	
#endif
}

