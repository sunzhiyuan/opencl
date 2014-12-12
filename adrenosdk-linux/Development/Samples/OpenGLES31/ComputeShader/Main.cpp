//**********************************
// OpenGL Samples Pack 
// ogl-samples.g-truc.net
//**********************************
// OpenGL Compute Program
// 12/08/2012 - 12/08/2012
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

#define GLF_BUFFER_OFFSET(i) ((char *)NULL + (i))

// GLM Includes
#include "gtx/transform.hpp"
#include "core/type_mat.hpp"
#include "core/type_vec.hpp"

using std::string;
using std::ifstream;
using std::ios;


struct vertex_v4fv4fv4f
{
	vertex_v4fv4fv4f
	(
		glm::vec4 const & Position,
		glm::vec4 const & Texcoord,
		glm::vec4 const & Color
	) :
		Position(Position),
		Texcoord(Texcoord),
		Color(Color)
	{}

	glm::vec4 Position;
	glm::vec4 Texcoord;
	glm::vec4 Color;
};

const CHAR*  g_strWindowTitle = "Compute Shader";
const UINT32 g_nWindowWidth   = 640;
const UINT32 g_nWindowHeight  = 480;

                       
GLsizei const VertexCount(8);
GLsizeiptr const VertexSize = VertexCount * sizeof(vertex_v4fv4fv4f);
vertex_v4fv4fv4f const VertexData[VertexCount] =
{
	vertex_v4fv4fv4f(glm::vec4(-1.0f,-1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, glm::vec2(0.0f)), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)),
	vertex_v4fv4fv4f(glm::vec4( 1.0f,-1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, glm::vec2(0.0f)), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)),
	vertex_v4fv4fv4f(glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, glm::vec2(0.0f)), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)),
	vertex_v4fv4fv4f(glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, glm::vec2(0.0f)), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)),
	vertex_v4fv4fv4f(glm::vec4(-1.0f,-1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, glm::vec2(0.0f)), glm::vec4(1.0f, 0.5f, 0.5f, 1.0f)),
	vertex_v4fv4fv4f(glm::vec4( 1.0f,-1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 1.0f, glm::vec2(0.0f)), glm::vec4(1.0f, 1.0f, 0.5f, 1.0f)),
	vertex_v4fv4fv4f(glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, glm::vec2(0.0f)), glm::vec4(0.5f, 1.0f, 0.0f, 1.0f)),
	vertex_v4fv4fv4f(glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, glm::vec2(0.0f)), glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)),
};

GLsizei const ElementCount(6);
GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
GLushort const ElementData[ElementCount] =
{
	0, 1, 2, 
	2, 3, 0
};

namespace program
{
	enum type
	{
		GRAPHICS,
		COMPUTE,
		MAX
	};
}//namespace program

namespace buffer
{
	enum type
	{
		ELEMENT,
		INPUT,
		OUTPUT,
		TRANSFORM,
		MAX
	};
}//namespace buffer

namespace semantics
{
	enum type
	{
		INPUT,
		OUTPUT
	};
}//namespace semantics

GLuint ProgramName[program::MAX] = {0, 0};
GLuint VertexArrayName(0);
GLuint BufferName[buffer::MAX] = {0, 0, 0, 0};
GLuint TextureName(0);


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
	
	LoadShader("Samples/Shaders/OpenGLES31/program-compute.vert",shaderSource,len);
	GLuint hVertShader = CreateShader(GL_VERTEX_SHADER, shaderSource);
	Validated = Validated && IsShaderStatusGood(hVertShader,GL_COMPILE_STATUS);	
	
	LoadShader("Samples/Shaders/OpenGLES31/program-compute.frag",shaderSource,len);
	GLuint hFragShader = CreateShader(GL_FRAGMENT_SHADER, shaderSource);
	Validated = Validated && IsShaderStatusGood(hFragShader,GL_COMPILE_STATUS);
	
	LoadShader("Samples/Shaders/OpenGLES31/program-compute.comp",shaderSource,len);
	GLuint hComputeShader = CreateShader(GL_COMPUTE_SHADER, shaderSource);
	Validated = Validated && IsShaderStatusGood(hComputeShader,GL_COMPILE_STATUS);

	ProgramName[program::GRAPHICS] = glCreateProgram();
	glAttachShader(ProgramName[program::GRAPHICS], hVertShader);
	glAttachShader(ProgramName[program::GRAPHICS], hFragShader);
	glLinkProgram(ProgramName[program::GRAPHICS]);
	glDeleteShader(hVertShader);	
	glDeleteShader(hFragShader);	

	ProgramName[program::COMPUTE] = glCreateProgram();
	glAttachShader(ProgramName[program::COMPUTE], hComputeShader);
	glLinkProgram(ProgramName[program::COMPUTE]);
	glDeleteShader(hComputeShader);

	return Validated;
}

bool InitBuffer()
{
	bool Validated(true);

	glGenBuffers(buffer::MAX, BufferName);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::INPUT]);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::OUTPUT]);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLint UniformBufferOffset(0);

	glGetIntegerv(
		GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
		&UniformBufferOffset);

	GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

	glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
	glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return Validated;
}


bool InitTexture()
{
	// Load the packed resources
	CFrmPackedResourceGLES resource;	
	if( FALSE == resource.LoadFromFile("Samples\\Textures\\ComputeShader31.pak" ))
	{
		return FALSE;
	}
	CFrmTexture* DiffuseTexture = resource.GetTexture("DiffuseTexture");
	TextureName = DiffuseTexture->m_hTextureHandle;

	return true;
}

bool InitVertexArray()
{
	glGenVertexArrays(1, &VertexArrayName);
	glBindVertexArray(VertexArrayName);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
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



	return Validated;
}

bool Cleanup()
{
	glDeleteProgram(ProgramName[program::GRAPHICS]);
	glDeleteProgram(ProgramName[program::COMPUTE]);
	glDeleteBuffers(buffer::MAX, BufferName);
	glDeleteTextures(1, &TextureName);
	glDeleteVertexArrays(1, &VertexArrayName);

	return true;
}

void Render()
{
	// Update of the uniform buffer
	{
		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(
			GL_UNIFORM_BUFFER, 0,	sizeof(glm::mat4),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		glm::mat4 Projection = glm::perspectiveFov(45.f, 640.f, 480.f, 0.1f, 100.0f);
		glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
		glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, 0.0f, glm::vec3(1.f, 0.f, 0.f));
		glm::mat4 View = glm::rotate(ViewRotateX, 0.0f, glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 Model = glm::mat4(1.0f);
		
		*Pointer = Projection * View * Model;

		// Make sure the uniform buffer is uploaded
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}

    glUseProgram(ProgramName[program::COMPUTE]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, semantics::INPUT, BufferName[buffer::INPUT]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, semantics::OUTPUT, BufferName[buffer::OUTPUT]);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, BufferName[buffer::TRANSFORM]);
    glDispatchCompute(GLuint(VertexCount), 1, 1);

    glViewport(0, 0, g_nWindowWidth, g_nWindowHeight);

	glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f)[0]);

    glUseProgram(ProgramName[program::GRAPHICS]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureName);
	glBindVertexArray(VertexArrayName);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, BufferName[buffer::TRANSFORM]);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, semantics::INPUT, BufferName[buffer::OUTPUT]);

    glDrawElements( GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, GLF_BUFFER_OFFSET(0));
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