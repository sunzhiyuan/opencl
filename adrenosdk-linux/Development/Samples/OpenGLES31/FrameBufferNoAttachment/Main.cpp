//**********************************
// OpenGL Framebuffer Multisample
// 01/09/2009 - 23/02/2013
//**********************************
// Christophe Riccio
// ogl-samples@g-truc.net
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

//--------------------------------------------------------------------------------------
// File: Main.cpp
// Desc: 
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#define _WIN32_WINNT 0x0502
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <fstream>
#include <EGL/egl.h>

#include <FrmPlatform.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>

#include <OpenGLES/FrmPackedResourceGLES.h>
#include <OpenGLES/FrmUserInterfaceGLES.h>

#define TEST_FRAMEBUFFER_NO_ATTACH
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// GLM Includes
#include "gtx/transform.hpp"
#include "core/type_mat.hpp"
#include "core/type_vec.hpp"

using std::string;
using std::ifstream;
using std::ios;


struct vertex_v2fv2f
{
	vertex_v2fv2f
		(
		glm::vec2 const & Position,
		glm::vec2 const & Texcoord
		) :
	Position(Position),
		Texcoord(Texcoord)
	{}

	glm::vec2 Position;
	glm::vec2 Texcoord;
};

const CHAR*  g_strWindowTitle = "Frame Buffer No Attachment MultiSample";
const UINT32 g_nWindowWidth   = 640;
const UINT32 g_nWindowHeight  = 480;
                       
glm::ivec2 const FRAMEBUFFER_SIZE(160, 120);

// With DDS textures, v texture coordinate are reversed, from top to bottom
GLsizei const VertexCount = 6;
GLsizeiptr const VertexSize = VertexCount * sizeof(vertex_v2fv2f);
vertex_v2fv2f const VertexData[VertexCount] =
{
	vertex_v2fv2f(glm::vec2(-4.0f,-3.0f), glm::vec2(0.0f, 1.0f)),
	vertex_v2fv2f(glm::vec2( 4.0f,-3.0f), glm::vec2(1.0f, 1.0f)),
	vertex_v2fv2f(glm::vec2( 4.0f, 3.0f), glm::vec2(1.0f, 0.0f)),
	vertex_v2fv2f(glm::vec2( 4.0f, 3.0f), glm::vec2(1.0f, 0.0f)),
	vertex_v2fv2f(glm::vec2(-4.0f, 3.0f), glm::vec2(0.0f, 0.0f)),
	vertex_v2fv2f(glm::vec2(-4.0f,-3.0f), glm::vec2(0.0f, 1.0f))
};

GLuint VertexArrayName = 0;
GLuint ProgramName = 0;

GLuint BufferName = 0;
GLuint Texture2DName = 0;

GLuint ColorRenderbufferName = 0;
GLuint ColorTextureName = 0;

GLuint FramebufferRenderName = 0;
GLuint FramebufferResolveName = 0;

GLint UniformMVP = 0;
GLint UniformDiffuse = 0;	

namespace attr
{
	enum type
	{
		POSITION = 0,
		COLOR	 = 3,
		TEXCOORD = 4,
		DRAW_ID  = 5
	};
}//namespace attr

// ===========================================================================

// GL_COMPILE_STATUS
bool IsShaderStatusGood(GLuint shaderProgram, const GLenum statusToCheck)
{
	char Log[2048];
	// Check for success
	GLint res = GL_FALSE;
	glGetShaderiv(shaderProgram, statusToCheck, &res);
	int InfoLogLength;
	glGetShaderiv(shaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0)
	{
		GLint nLength;
		glGetShaderInfoLog(shaderProgram, 2048, &nLength, Log);
	}

	return (GL_TRUE==res);
}

// GL_LINK_STATUS
bool IsProgramStatusGood(GLuint shaderProgram)
{
	char Log[1024];
	// Check for success
	GLint res = GL_FALSE;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &res);
	int InfoLogLength;
	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0)
	{
		GLint nLength;
		glGetProgramInfoLog(shaderProgram, 1024, &nLength, Log);
	}

	return (GL_TRUE==res);
}

GLuint CreateShader(const GLuint shaderType, string shaderSource)
{
	GLuint shader;
	const char *source= shaderSource.c_str();

	shader = glCreateShader( shaderType );

	glShaderSource( shader, 1, &source, NULL );
	glCompileShader( shader );

	return shader;
}

unsigned long GetFileLength(ifstream& file)
{
	if(false==file.good()) 
		return 0;

	unsigned long pos = file.tellg();
	file.seekg(0,ios::end);
	unsigned long len = file.tellg();
	file.seekg(ios::beg);

	return len;
}

int LoadShader(const char* filename, string &ShaderSource, unsigned long &len)
{
	char *mem = 0;

	ifstream file;
	file.open(filename, ios::in); // opens as ASCII!
	if(false==file) 
		return -1;

	len = GetFileLength(file);

	if (len==0)
		return -2;   // Error: Empty File 

	mem = (char*) new char[len+1];
	if (mem == 0) return -3;   // can't reserve memory

	// len isn't always strlen cause some characters are stripped in ascii read...
	// it is important to 0-terminate the real length later, len is just max possible value... 
	mem[len] = 0; 

	unsigned int i=0;
	while (file.good())
	{
		mem[i] = file.get();       // get character from file.
		if (!file.eof())
			i++;
	}

	mem[i] = 0;  // 0-terminate it at the correct position

	ShaderSource = string(mem);
	if (mem != 0)
		delete[] mem;
	mem = 0;


	file.close();

	return 0; // No Error
}

bool InitProgram()
{
	bool Validated = true;

	string shaderSource = "";
	unsigned long len = 0;		

	if(Validated)
	{
		LoadShader("Samples/Shaders/OpenGLES31/image-2d.vert",shaderSource,len);
		GLuint VertexShader = CreateShader(GL_VERTEX_SHADER, shaderSource);
		Validated = Validated && IsShaderStatusGood(VertexShader,GL_COMPILE_STATUS);

		LoadShader("Samples/Shaders/OpenGLES31/image-2d.frag",shaderSource,len);
		GLuint FragmentShader = CreateShader(GL_FRAGMENT_SHADER, shaderSource);
		Validated = Validated && IsShaderStatusGood(FragmentShader,GL_COMPILE_STATUS);

		ProgramName = glCreateProgram();
		glAttachShader(ProgramName, VertexShader);
		glAttachShader(ProgramName, FragmentShader);
		glDeleteShader(VertexShader);
		glDeleteShader(FragmentShader);

		glBindAttribLocation(ProgramName, attr::POSITION, "Position");
		glBindAttribLocation(ProgramName, attr::TEXCOORD, "Texcoord");
		glLinkProgram(ProgramName);

		Validated = Validated && IsProgramStatusGood(ProgramName);	
	}
	if(Validated)
	{
		UniformMVP = glGetUniformLocation(ProgramName, "MVP");
		UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");
	}

	return Validated;
}

bool InitBuffer()
{
	glGenBuffers(1, &BufferName);
	glBindBuffer(GL_ARRAY_BUFFER, BufferName);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}


bool InitTexture()
{
	// Load the packed resources
	CFrmPackedResourceGLES resource;	
	if( FALSE == resource.LoadFromFile("Samples\\Textures\\FboNoAttachment.pak" ))
	{
		return FALSE;
	}
	CFrmTexture* DiffuseTexture = resource.GetTexture("DiffuseTexture");
	Texture2DName = DiffuseTexture->m_hTextureHandle;

	return true;
}

bool InitFramebuffer()
{
#ifdef TEST_FRAMEBUFFER_NO_ATTACH
	glGenFramebuffers(1, &ColorRenderbufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, ColorRenderbufferName);
	glFramebufferParameteri(GL_FRAMEBUFFER,	GL_FRAMEBUFFER_DEFAULT_WIDTH, FRAMEBUFFER_SIZE.x);
	glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, FRAMEBUFFER_SIZE.y);
	glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES,	8);
	glFramebufferParameteri(GL_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#else
	glGenRenderbuffers(1, &ColorRenderbufferName);
	glBindRenderbuffer(GL_RENDERBUFFER, ColorRenderbufferName);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGBA8, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y);
	// The second parameter is the number of samples.
	glGenFramebuffers(1, &FramebufferRenderName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferRenderName);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ColorRenderbufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
	glGenTextures(1, &ColorTextureName);
	glBindTexture(GL_TEXTURE_2D, ColorTextureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenFramebuffers(1, &FramebufferResolveName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferResolveName);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTextureName, 0);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool InitVertexArray()
{
	glGenVertexArrays(1, &VertexArrayName);
	glBindVertexArray(VertexArrayName);
	glBindBuffer(GL_ARRAY_BUFFER, BufferName);
	glVertexAttribPointer(attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_v2fv2f), BUFFER_OFFSET(0));
	glVertexAttribPointer(attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_v2fv2f), BUFFER_OFFSET(sizeof(glm::vec2)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(attr::POSITION);
	glEnableVertexAttribArray(attr::TEXCOORD);
	glBindVertexArray(0);

	return true;
}

bool Initialize()
{
	bool Validated(true);

	if(Validated)
		Validated = InitProgram();
	if(Validated)
		Validated = InitBuffer();
	if(Validated)
		Validated = InitVertexArray();
	if(Validated)
		Validated = InitTexture();
	if(Validated)
		Validated = InitFramebuffer();

	return Validated;
}

bool Cleanup()
{
	glDeleteBuffers(1, &BufferName);
	glDeleteProgram(ProgramName);
	glDeleteTextures(1, &Texture2DName);
	glDeleteTextures(1, &ColorTextureName);
	glDeleteRenderbuffers(1, &ColorRenderbufferName);
	glDeleteFramebuffers(1, &FramebufferRenderName);
	glDeleteFramebuffers(1, &FramebufferResolveName);
	glDeleteVertexArrays(1, &VertexArrayName);

	return true;
}


void renderFBO(GLuint Framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glm::mat4 Perspective = glm::perspective(45.0f, float(FRAMEBUFFER_SIZE.x) / FRAMEBUFFER_SIZE.y, 0.1f, 100.0f);
	glm::mat4 ViewFlip = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f,-1.0f, 1.0f));
	glm::mat4 ViewTranslate = glm::translate(ViewFlip, glm::vec3(0.0f, 0.0f, -4 * 2.0));
	glm::mat4 View = glm::rotate(ViewTranslate,-15.f, glm::vec3(0.f, 0.f, 1.f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Perspective * View * Model;
	glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

	glViewport(0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y);
	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture2DName);

	glBindVertexArray(VertexArrayName);
	glDrawArrays(GL_TRIANGLES, 0, VertexCount);
}

void renderFB(GLuint Texture2DName)
{
	glm::mat4 Perspective = glm::perspective(45.0f, float(FRAMEBUFFER_SIZE.x) / FRAMEBUFFER_SIZE.y, 0.1f, 100.0f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4 * 2.0));
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, 0.0f, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 View = glm::rotate(ViewRotateX, 0.0f, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Perspective * View * Model;
	glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture2DName);

	glBindVertexArray(VertexArrayName);
	glDrawArrays(GL_TRIANGLES, 0, VertexCount);
}

void Render()
{
	// Clear the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

	glUseProgram(ProgramName);
	glUniform1i(UniformDiffuse, 0);

	// Pass 1
	// Render the scene in a multisampled framebuffer
	renderFBO(FramebufferRenderName);

	// Resolved multisampling
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FramebufferRenderName);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FramebufferResolveName);
	glBlitFramebuffer(
		0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 
		0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 
		GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Pass 2
	// Render the colorbuffer from the multisampled framebuffer
	glViewport(0, 0, g_nWindowWidth, g_nWindowHeight);
	renderFB(ColorTextureName);
}


bool InitEGL(EGLDisplay &eglDisplay, EGLSurface &eglSurface, NativeWindowType &hWindow)
{
    // Get the display
    if( eglDisplay == EGL_NO_DISPLAY )
        return false;

    // Initialize EGL
    EGLint nMajorVersion, nMinorVersion;
    if( FALSE == eglInitialize( eglDisplay, &nMajorVersion, &nMinorVersion ) )
        return false;

    // Set our EGL API to OpenGL ES
    if( FALSE == eglBindAPI( EGL_OPENGL_ES_API ) )
        return false;
    
    // Get total number of configs
    EGLint nNumConfigs;
    if( FALSE == eglGetConfigs( eglDisplay, NULL, 0, &nNumConfigs ) )
        return false;

    // Get the first config
    EGLConfig configs[10];
    if (FALSE == eglGetConfigs(eglDisplay, configs, 1, &nNumConfigs) )
        return false;

    // Get the first config's Color buffer size
    EGLint nBuffSize;
    eglGetConfigAttrib(eglDisplay, configs[0], EGL_BUFFER_SIZE, &nBuffSize);

    // Test the display configuration
    EGLConfig eglConfig;

    if (nBuffSize == 32)
    {
        // Build the attibute list
        EGLint AttributeList[] = 
        { 
            EGL_RED_SIZE,		8,
            EGL_GREEN_SIZE,	    8,
            EGL_BLUE_SIZE,	    8,
            EGL_ALPHA_SIZE,	    0,
            EGL_DEPTH_SIZE,	    24,
            EGL_STENCIL_SIZE,   8,
            EGL_SAMPLE_BUFFERS, 0,
            EGL_SAMPLES,		0,
            EGL_NONE
        };

        // Choose config based on the requested attributes
        if( FALSE == eglChooseConfig( eglDisplay, AttributeList, &eglConfig, 1, &nNumConfigs ) )
            return false;
    }
    else if (nBuffSize == 16)
    {
        // Build the attibute list
        EGLint AttributeList[] = 
        { 
            EGL_RED_SIZE,		5,
            EGL_GREEN_SIZE,	    6,
            EGL_BLUE_SIZE,	    5,
            EGL_ALPHA_SIZE,	    0,
            EGL_DEPTH_SIZE,	    24,
            EGL_STENCIL_SIZE,   8,
            EGL_SAMPLE_BUFFERS, 0,
            EGL_SAMPLES,		0,
            EGL_NONE
        };

        // Choose config based on the requested attributes
        if( FALSE == eglChooseConfig( eglDisplay, AttributeList, &eglConfig, 1, &nNumConfigs ) )
            return false;
    }
    else return false; // unsupported display

    // Create a window surface
    eglSurface = eglCreateWindowSurface( eglDisplay, eglConfig, hWindow, NULL );
    if( EGL_NO_SURFACE == eglSurface )
        return false;

    // Create a OpenGL ES 2.0 rendering context
    EGLint ContextAttribList[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLContext eglContext = eglCreateContext( eglDisplay, eglConfig, EGL_NO_CONTEXT, ContextAttribList );
    if( EGL_NO_CONTEXT == eglContext )
        return false;

    // Make the context current
    if( FALSE == eglMakeCurrent( eglDisplay, eglSurface, eglSurface, eglContext ) )
        return false;

    return true;
}

//--------------------------------------------------------------------------------------
// Name: WndProc()
// Desc: The application window's message proc
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        case WM_CLOSE:
            PostQuitMessage( 0 );
            return 0;
    }

    // Pass all unhandled messages to the default WndProc
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    // Create the application window
    NativeWindowType hWindow;
    {
        // The global instance
        HINSTANCE hInstance  = GetModuleHandle( NULL );

        // Register the window class
        WNDCLASS wc = {0};
        wc.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;  // Window style
        wc.lpfnWndProc    = (WNDPROC)WndProc;                    // WndProc message handler
        wc.hInstance      = hInstance;                           // Instance
        wc.lpszClassName  = TEXT("Snapdragon SDK Window");             // Set the class name
        wc.hCursor        = LoadCursor( NULL, IDC_ARROW );       // Cursor
        if( FALSE == RegisterClass(&wc) )
            return FALSE;

        // Adjust the window size to fit our rectangle
        DWORD dwWindowStyle = WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION | WS_BORDER |
                              WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        RECT rcWindow;
        SetRect( &rcWindow, 0, 0, g_nWindowWidth, g_nWindowHeight );
        AdjustWindowRect( &rcWindow, dwWindowStyle, FALSE );

        // Create the parent window
        hWindow = CreateWindowEx(
            WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,    // Extended style
            TEXT("Snapdragon SDK Window"),         // Class
            g_strWindowTitle,				       // Title
            dwWindowStyle,                         // Style
            50 + rcWindow.left, 50 + rcWindow.top, // Position
            (rcWindow.right-rcWindow.left),        // Width
            (rcWindow.bottom-rcWindow.top),        // Height
            NULL, NULL, hInstance, NULL );
        if( NULL == hWindow )
            return FALSE;

        ShowWindow( hWindow, SW_SHOW );
        SetForegroundWindow( hWindow );
        SetFocus( hWindow );
    }

    EGLDisplay eglDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
    EGLSurface eglSurface;
    InitEGL(eglDisplay,eglSurface,hWindow);

    if( FALSE == Initialize() )
        return FALSE;

    // Run the main loop until the user closes the window
    while( TRUE )
    {
        MSG msg;
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            if( msg.message == WM_QUIT )
                return FALSE;
        }

        TranslateMessage( &msg );
        DispatchMessage( &msg );

        // Update and render the application
        Render();

        // Present the scene
        eglSwapBuffers( eglDisplay, eglSurface );
    }

    // Cleanup any objects created in Initialize()
    Cleanup();

    return TRUE;
}