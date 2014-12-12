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
    return new CSample( "FrameBufferTextureLayerOGLES30" );
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


   g_pOffscreenFBO   = NULL;
   
    g_nLayers		         = 9;
    g_hTextureHandle        = 0;
   
    g_hShaderProgram        = 0;
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
	"#version 300 es													 \n"
    "uniform   mat4 g_matModelView;                                      \n"
    "uniform   mat4 g_matProj;                                           \n"
    "                                                                    \n"
    "in vec4 g_vPosition;	 								     \n"
    "in vec3 g_vColor; 			   		 		                 \n"
    "                                                                    \n"
    "out   vec3 g_vVSColor;                                          \n"
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
    "in   vec3      g_vVSColor;                                     \n"
	"out vec4 g_FragColor;												 \n"
    "void main()														 \n"
    "{																	 \n"
    "    g_FragColor = vec4( g_vVSColor, 1.0 );                         \n"
    "}																	 \n";

    
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

	// Generate the texture array that will be used as a set of color buffers
	//	for the frame buffer (GL_TEXTURE_3D would work here also)
	glGenTextures( 1, &g_hTextureHandle );
    glBindTexture( GL_TEXTURE_2D_ARRAY, g_hTextureHandle );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexImage3D( GL_TEXTURE_2D_ARRAY, 0, nInternalFormat, nWidth, nHeight,
                       g_nLayers, 0, nFormat, nType, NULL );

    // Create the depth buffer
    glGenRenderbuffers( 1, &(*ppFBO)->m_hRenderBuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, (*ppFBO)->m_hRenderBuffer );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, nWidth, nHeight );

    // Create the color buffer
    glGenFramebuffers( 1, &(*ppFBO)->m_hFrameBuffer );
    glBindFramebuffer( GL_FRAMEBUFFER, (*ppFBO)->m_hFrameBuffer );
    glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_hTextureHandle, 0, 0);
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
    glDeleteFramebuffers( 1, &pFBO->m_hFrameBuffer );
    glDeleteRenderbuffers( 1, &pFBO->m_hRenderBuffer );
	glDeleteRenderbuffers( 1, &pFBO->m_hColorBuffer );
    delete pFBO;
}


//--------------------------------------------------------------------------------------
// Name: BeginFBO()
// Desc: Bind objects for rendering to the frame buffer object
//--------------------------------------------------------------------------------------
VOID CSample::BeginFBO( FrameBufferObject* pFBO, INT32 layer )
{
    glBindFramebuffer( GL_FRAMEBUFFER, pFBO->m_hFrameBuffer );
    glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_hTextureHandle, 0, layer);
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

    // Create an offscreen FBO and a texture array to serve as the multiple color attachments
    if( FALSE == CreateFBO( g_nWindowWidth, g_nWindowHeight, g_format, g_type, g_internalFormat, &g_pOffscreenFBO ) )
        return FALSE;

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

	// Render the scene the texture array
		for(int i=0; i<g_nLayers; i++)
		{
			// bind the FBO with the i'th texture as the color attachment
			BeginFBO( g_pOffscreenFBO, i );
			
			glClearColor( 0.0f, 0.5f, 0.0f, 1.0f );
			RenderScene( fTime+i*0.1f );
			
			EndFBO( g_pOffscreenFBO );
		}
	
	
	// Blitting the texture array into the main framebuffer
		
		FLOAT offsets[9][4] = 
		{
			{ 0.0f,			0.0f,		1.0f/3.0f,	1.0f/3.0f },
			{ 1.0f/3.0f,	0.0f,		2.0f/3.0f,	1.0f/3.0f },
			{ 2.0f/3.0f,	0.0f,		1.0f,		1.0f/3.0f },
			{ 0.0f,			1.0f/3.0f,	1.0f/3.0f,	2.0f/3.0f },
			{ 1.0f/3.0f,	1.0f/3.0f,	2.0f/3.0f,	2.0f/3.0f },
			{ 2.0f/3.0f,	1.0f/3.0f,	1.0f,		2.0f/3.0f },
			{ 0.0f,			2.0f/3.0f,	1.0f/3.0f,	1.0f },
			{ 1.0f/3.0f,	2.0f/3.0f,	2.0f/3.0f,	1.0f },
			{ 2.0f/3.0f,	2.0f/3.0f,	1.0f,		1.0f },
		};
	
		// Bind the default framebuffer as the draw framebuffer
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		
		// Bind the offscreen FBO to the read framebuffer
		glBindFramebuffer( GL_READ_FRAMEBUFFER, g_pOffscreenFBO->m_hFrameBuffer );
		
		for(int i=0; i<g_nLayers; i++)
		{
			// Bind the next texture layer to be read into the main framebuffer	
			glFramebufferTextureLayer( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, g_hTextureHandle, 0, i);
			
			// Blit the fb into the main fb
			glBlitFramebuffer( 0, 0, g_pOffscreenFBO->m_nWidth, g_pOffscreenFBO->m_nHeight,
							   INT32(g_nWindowWidth*offsets[i][0]), INT32(g_nWindowHeight*offsets[i][1]), INT32(g_nWindowWidth*offsets[i][2]), INT32(g_nWindowHeight*offsets[i][3]),
							   GL_COLOR_BUFFER_BIT, GL_LINEAR );	// Color only, and filtering
		}
		
		// unbind the read framebuffer
		glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );
}

