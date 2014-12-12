//**********************************
// OpenGL Framebuffer Multisample
// 16/06/2010 - 28/06/2010
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
const CHAR*  g_strWindowTitle = "Fbo MultiSample";
const UINT32 g_nWindowWidth   = 640;
const UINT32 g_nWindowHeight  = 480;

glm::ivec2 const FRAMEBUFFER_SIZE(320, 240);
int const SAMPLE_SIZE_WIDTH(640);
int const SAMPLE_SIZE_HEIGHT(480);


GLsizei const VertexCount = 4;
GLsizeiptr const VertexSize = VertexCount * sizeof(vertex_v2fv2f);
vertex_v2fv2f const VertexData[VertexCount] =
{
	vertex_v2fv2f(glm::vec2(-4.0f,-3.0f), glm::vec2(0.0f, 0.0f)),
	vertex_v2fv2f(glm::vec2( 4.0f,-3.0f), glm::vec2(1.0f, 0.0f)),
	vertex_v2fv2f(glm::vec2( 4.0f, 3.0f), glm::vec2(1.0f, 1.0f)),
	vertex_v2fv2f(glm::vec2(-4.0f, 3.0f), glm::vec2(0.0f, 1.0f))
};

GLsizei const ElementCount = 6;
GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
GLushort const ElementData[ElementCount] =
{
	0, 1, 2,
	2, 3, 0
};

enum buffer_type
{
	BUFFER_VERTEX,
	BUFFER_ELEMENT,
	BUFFER_MAX
};

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

GLuint VertexArrayName = 0;
GLuint ProgramName = 0;

GLuint BufferName[BUFFER_MAX];
GLuint Texture2DName = 0;

GLuint MultisampleTextureName = 0;
GLuint ColorTextureName = 0;
GLuint SamplerName = 0;

GLuint FramebufferRenderName = 0;
GLuint FramebufferResolveName = 0;

GLuint sampleColorRenderbuffer = 0;

GLint UniformMVP = 0;
GLint UniformDiffuse = 0;	



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
		LoadShader("Samples/Shaders/OpenGLES31/multisample.vert",shaderSource,len);
		GLuint VertShaderName = CreateShader(GL_VERTEX_SHADER, shaderSource);
		Validated = Validated && IsShaderStatusGood(VertShaderName,GL_COMPILE_STATUS);

		LoadShader("Samples/Shaders/OpenGLES31/multisample.frag",shaderSource,len);
		GLuint FragShaderName = CreateShader(GL_FRAGMENT_SHADER, shaderSource);
		Validated = Validated && IsShaderStatusGood(VertShaderName,GL_COMPILE_STATUS);	

		ProgramName = glCreateProgram();		
		glAttachShader(ProgramName, VertShaderName);
		glAttachShader(ProgramName, FragShaderName);

		glLinkProgram(ProgramName);
		Validated = Validated && IsProgramStatusGood(ProgramName);

	}

	UniformMVP = glGetUniformLocation(ProgramName, "MVP");
	UniformDiffuse = glGetUniformLocation(ProgramName, "Diffuse");

	return Validated;
}

bool InitVertexBuffer()
{
	glGenBuffers(BUFFER_MAX, BufferName);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[BUFFER_ELEMENT]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, BufferName[BUFFER_VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}

bool InitSampler()
{
	glGenSamplers(1, &SamplerName);

	// Parameters part of the sampler object:
	glSamplerParameteri(SamplerName, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(SamplerName, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(SamplerName, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//	glSamplerParameterfv(SamplerName, GL_TEXTURE_BORDER_COLOR, &glm::vec4(0.0f)[0]);
	glSamplerParameterf(SamplerName, GL_TEXTURE_MIN_LOD, -1000.f);
	glSamplerParameterf(SamplerName, GL_TEXTURE_MAX_LOD, 1000.f);
	//	glSamplerParameterf(SamplerName, GL_TEXTURE_LOD_BIAS, 0.0f);
	glSamplerParameteri(SamplerName, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glSamplerParameteri(SamplerName, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glBindSampler(0, SamplerName);

	return true;
}


bool InitTexture2D()
{
	// Load the packed resources
	CFrmPackedResourceGLES resource;	
	if( FALSE == resource.LoadFromFile("Samples\\Textures\\FboMultiSample.pak" ))
	{
		return FALSE;
	}
	CFrmTexture* DiffuseTexture = resource.GetTexture("DiffuseTexture");
	Texture2DName = DiffuseTexture->m_hTextureHandle;

	return true;
}

bool InitFrameBuffer()	
{
	glGenTextures(1, &MultisampleTextureName);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, MultisampleTextureName);

	glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, GL_FALSE);

	glGenFramebuffers(1, &FramebufferRenderName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferRenderName);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, MultisampleTextureName, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &ColorTextureName);
	glBindTexture(GL_TEXTURE_2D, ColorTextureName);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenFramebuffers(1, &FramebufferResolveName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferResolveName);	

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTextureName, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool InitVertexArray()
{
	glGenVertexArrays(1, &VertexArrayName);
	glBindVertexArray(VertexArrayName);	glBindBuffer(GL_ARRAY_BUFFER, BufferName[BUFFER_VERTEX]);
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
		Validated = InitSampler();
	if(Validated)
		Validated = InitVertexBuffer();
	if(Validated)
		Validated = InitVertexArray();
	if(Validated)
		Validated = InitTexture2D();
	if(Validated)
		Validated = InitFrameBuffer();

	return Validated;
}

bool Cleanup()
{
	bool Validated(true);

	glDeleteBuffers(BUFFER_MAX, BufferName);
	glDeleteProgram(ProgramName);
	glDeleteTextures(1, &Texture2DName);
	glDeleteTextures(1, &ColorTextureName);
	glDeleteTextures(1, &MultisampleTextureName);
	glDeleteFramebuffers(1, &FramebufferRenderName);
	glDeleteFramebuffers(1, &FramebufferResolveName);
	glDeleteVertexArrays(1, &VertexArrayName);
	glDeleteSamplers(1, &SamplerName);

	return Validated;
}

void renderFBO(GLuint Framebuffer)
{
	glm::mat4 Perspective = glm::perspective(45.0f, float(FRAMEBUFFER_SIZE.x) / FRAMEBUFFER_SIZE.y, 0.1f, 100.0f);
	glm::mat4 ViewFlip = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f,-1.0f, 1.0f));
	glm::mat4 ViewTranslate = glm::translate(ViewFlip, glm::vec3(0.0f, 0.0f, -4 * 2.0));
	glm::mat4 View = glm::rotate(ViewTranslate,-15.f, glm::vec3(0.f, 0.f, 1.f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Perspective * View * Model;
	glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

	glViewport(0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
	glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.5f, 1.0f, 1.0f)[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture2DName);

	glBindVertexArray(VertexArrayName);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[BUFFER_ELEMENT]);

	glDrawElementsInstanced(  GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, NULL, 1);
}

void renderFB(GLuint Texture2DName)
{
	glm::mat4 Perspective = glm::perspective(45.0f, float(SAMPLE_SIZE_WIDTH) / SAMPLE_SIZE_HEIGHT, 0.1f, 100.0f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4 * 2.0));
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, 0.0f, glm::vec3(1.f, 0.f, 0.f));
	glm::mat4 View = glm::rotate(ViewRotateX, 0.0f, glm::vec3(0.f, 1.f, 0.f));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Perspective * View * Model;
	glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture2DName);

	glBindVertexArray(VertexArrayName);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[BUFFER_ELEMENT]);

	glDrawElementsInstanced(  GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, NULL, 1);
}


void Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

	glUseProgram(ProgramName);
	glUniform1i(UniformDiffuse, 0);

	// Pass 1, render the scene in a multisampled framebuffer
	glEnable(GL_SAMPLE_SHADING_OES);
	glMinSampleShadingOES(2.0f);

	glEnable(GL_SAMPLE_MASK);
	glSampleMaski(0, 0xFF);

	renderFBO(FramebufferRenderName);

	// Resolved multisampling
	glBindFramebuffer(GL_READ_FRAMEBUFFER, FramebufferRenderName);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FramebufferResolveName);
	glBlitFramebuffer(
		0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y,
		0, 0, FRAMEBUFFER_SIZE.x, FRAMEBUFFER_SIZE.y,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Pass 2, render the colorbuffer from the multisampled framebuffer
	glViewport(0, 0, SAMPLE_SIZE_WIDTH, SAMPLE_SIZE_HEIGHT);
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