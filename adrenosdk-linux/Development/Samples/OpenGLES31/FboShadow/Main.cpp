//**********************************
// OpenGL Shadow Gather
// 11/02/2013 - 11/02/2013
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
#include <vector>
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


struct vertex_v3fv4u8
{
	vertex_v3fv4u8
		(
		glm::vec3 const & Position,
		glm::u8vec4 const & Color
		) :
	Position(Position),
		Color(Color)
	{}

	glm::vec3 Position;
	glm::u8vec4 Color;
};

const CHAR*  g_strWindowTitle = "Fbo Shadow";
const UINT32 g_nWindowWidth   = 640;
const UINT32 g_nWindowHeight  = 480;
                       
GLsizei const VertexCount(8);
GLsizeiptr const VertexSize = VertexCount * sizeof(vertex_v3fv4u8);
vertex_v3fv4u8 const VertexData[VertexCount] =
{
	vertex_v3fv4u8(glm::vec3(-1.0f,-1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
	vertex_v3fv4u8(glm::vec3( 1.0f,-1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
	vertex_v3fv4u8(glm::vec3( 1.0f, 1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
	vertex_v3fv4u8(glm::vec3(-1.0f, 1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
	vertex_v3fv4u8(glm::vec3(-0.1f,-0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255)),
	vertex_v3fv4u8(glm::vec3( 0.1f,-0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255)),
	vertex_v3fv4u8(glm::vec3( 0.1f, 0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255)),
	vertex_v3fv4u8(glm::vec3(-0.1f, 0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255))
};

GLsizei const ElementCount(12);
GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
GLushort const ElementData[ElementCount] =
{
	0, 1, 2, 
	2, 3, 0,
	4, 5, 6, 
	6, 7, 4,
};


namespace buffer
{
	enum type
	{
		VERTEX,
		ELEMENT,
		TRANSFORM,
		MAX
	};
}//namespace buffer

namespace texture
{
	enum type
	{
		DIFFUSE,
		COLORBUFFER,
		RENDERBUFFER,
		SHADOWMAP,
		MAX
	};
}//namespace texture

namespace program
{
	enum type
	{
		DEPTH,
		RENDER,
		MAX
	};
}//namespace program

namespace framebuffer
{
	enum type
	{
		FRAMEBUFFER,
		SHADOW,
		MAX
	};
}//namespace framebuffer

namespace frag
{
	enum type
	{
		COLOR	= 0,
		RED		= 0,
		GREEN	= 1,
		BLUE	= 2,
		ALPHA	= 0
	};
}//namespace frag

namespace uniform
{
	enum type
	{
		MATERIAL = 0,
		TRANSFORM0 = 1,
		TRANSFORM1 = 2,
		INDIRECTION = 3
	};
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

std::vector<GLuint> FramebufferName(framebuffer::MAX);
std::vector<GLuint> ProgramName(program::MAX);
std::vector<GLuint> VertexArrayName(program::MAX);
std::vector<GLuint> BufferName(buffer::MAX);
std::vector<GLuint> TextureName(texture::MAX);
std::vector<GLint> UniformTransform(program::MAX);
GLint UniformShadow(0);
GLint uniformExplicitLocation(5);
glm::ivec2 const ShadowSize(1024, 1024);

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
		LoadShader("Samples/Shaders/OpenGLES31/fbo-shadow-render.vert",shaderSource,len);
		GLuint VertexShader = CreateShader(GL_VERTEX_SHADER, shaderSource);
		Validated = Validated && IsShaderStatusGood(VertexShader,GL_COMPILE_STATUS);

		LoadShader("Samples/Shaders/OpenGLES31/fbo-shadow-render.frag",shaderSource,len);
		GLuint FragmentShader = CreateShader(GL_FRAGMENT_SHADER, shaderSource);
		Validated = Validated && IsShaderStatusGood(FragmentShader,GL_COMPILE_STATUS);

		ProgramName[program::RENDER] = glCreateProgram();
		glAttachShader(ProgramName[program::RENDER], VertexShader);
		glAttachShader(ProgramName[program::RENDER], FragmentShader);
		glLinkProgram(ProgramName[program::RENDER]);
		glDeleteShader(VertexShader);
		glDeleteShader(FragmentShader);

		Validated = Validated && IsProgramStatusGood(ProgramName[program::RENDER]);	

		if(Validated)
		{
			UniformTransform[program::RENDER] = glGetUniformBlockIndex(ProgramName[program::RENDER], "transform");
			UniformShadow = glGetUniformLocation(ProgramName[program::RENDER], "Shadow");
		}

		if(Validated)
		{
			LoadShader("Samples/Shaders/OpenGLES31/fbo-shadow-depth.vert",shaderSource,len);
			GLuint VertShaderDepth = CreateShader(GL_VERTEX_SHADER, shaderSource);
			Validated = Validated && IsShaderStatusGood(VertShaderDepth,GL_COMPILE_STATUS);

			LoadShader("Samples/Shaders/OpenGLES31/fbo-shadow-depth.frag",shaderSource,len);
			GLuint FragShaderDepth = CreateShader(GL_FRAGMENT_SHADER, shaderSource);
			Validated = Validated && IsShaderStatusGood(FragShaderDepth,GL_COMPILE_STATUS);

			ProgramName[program::DEPTH] = glCreateProgram();
			glAttachShader(ProgramName[program::DEPTH], VertShaderDepth);
			glAttachShader(ProgramName[program::DEPTH], FragShaderDepth);
			glLinkProgram(ProgramName[program::DEPTH]);
			glDeleteShader(VertShaderDepth);
			glDeleteShader(FragShaderDepth);

			Validated = Validated && IsProgramStatusGood(ProgramName[program::DEPTH]);	
		}

		if(Validated)
		{
			UniformTransform[program::DEPTH] = glGetUniformBlockIndex(ProgramName[program::DEPTH], "transform");			
		}
	}
	return Validated;
}

bool InitBuffer()
{
	glGenBuffers(buffer::MAX, &BufferName[0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLint UniformBufferOffset(0);

	glGetIntegerv(
		GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
		&UniformBufferOffset);

	GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4) * 3), UniformBufferOffset);

	glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
	glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return true;
}

bool InitFramebuffer()
{	
	glGenFramebuffers(framebuffer::MAX, &FramebufferName[0]);

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::FRAMEBUFFER]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureName[texture::COLORBUFFER], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TextureName[texture::RENDERBUFFER], 0);

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::SHADOW]);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TextureName[texture::RENDERBUFFER], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TextureName[texture::SHADOWMAP], 0);
	glDrawBuffers(0, GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

bool InitTexture()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(texture::MAX, &TextureName[0]);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureName[texture::COLORBUFFER]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);		
	glTexImage2D(GL_TEXTURE_2D, GLint(0), GL_RGBA8, GLsizei(g_nWindowWidth), GLsizei(g_nWindowHeight), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureName[texture::RENDERBUFFER]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);	
	glTexImage2D(GL_TEXTURE_2D, GLint(0), GL_DEPTH_COMPONENT24, GLsizei(g_nWindowWidth), GLsizei(g_nWindowHeight), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureName[texture::SHADOWMAP]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);	
	glTexImage2D(GL_TEXTURE_2D, GLint(0), GL_DEPTH_COMPONENT24, ShadowSize.x, ShadowSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	return true;
}



bool InitVertexArray()
{
	glGenVertexArrays(program::MAX, &VertexArrayName[0]);
	glBindVertexArray(VertexArrayName[program::RENDER]);
	glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
	glVertexAttribPointer(attr::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_v3fv4u8), BUFFER_OFFSET(0));
	glVertexAttribPointer(attr::COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex_v3fv4u8), BUFFER_OFFSET(sizeof(glm::vec3)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(attr::POSITION);
	glEnableVertexAttribArray(attr::COLOR);

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
	if(Validated)
		Validated = InitFramebuffer();

	return Validated;
}

bool Cleanup()
{
	glDeleteFramebuffers(framebuffer::MAX, &FramebufferName[0]);
	for(std::size_t i = 0; i < program::MAX; ++i)
		glDeleteProgram(ProgramName[i]);
	glDeleteBuffers(buffer::MAX, &BufferName[0]);
	glDeleteTextures(texture::MAX, &TextureName[0]);
	glDeleteVertexArrays(program::MAX, &VertexArrayName[0]);

	return true;
}

void renderShadow()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glViewport(0, 0, ShadowSize.x, ShadowSize.y);

	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::SHADOW]);
	float Depth(1.0f);
	glClearBufferfv(GL_DEPTH , 0, &Depth);

	// Bind rendering objects
	glUseProgram(ProgramName[program::DEPTH]);

	glUniformBlockBinding(ProgramName[program::DEPTH], UniformTransform[program::DEPTH], uniform::TRANSFORM0);

	glBindVertexArray(VertexArrayName[program::RENDER]);
	glDrawElementsInstanced(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 1);

	glDisable(GL_DEPTH_TEST);
}

void renderFramebuffer()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glViewport(0, 0, g_nWindowWidth, g_nWindowHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	float Depth(1.0f);
	glClearBufferfv(GL_DEPTH , 0, &Depth);
	glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

	glUseProgram(ProgramName[program::RENDER]);

	// set value of uniform variable using it's explicitly defined location	
	glUniform1i(uniformExplicitLocation, 1);

	glUniform1i(UniformShadow, 0);

	glUniformBlockBinding(ProgramName[program::RENDER], UniformTransform[program::RENDER], uniform::TRANSFORM0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureName[texture::SHADOWMAP]);

	glBindVertexArray(VertexArrayName[program::RENDER]);
	glDrawElementsInstanced(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 1);

	glDisable(GL_DEPTH_TEST);
}

void Render()
{
	glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
	glm::mat4* Pointer = (glm::mat4*)glMapBufferRange(
		GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	{
		// Compute the MVP (Model View Projection matrix)
		float Aspect = (640 * 0.50f) / (480 * 0.50f);
		glm::mat4 Projection = glm::perspective(45.0f, Aspect, 0.1f, 100.0f);
		glm::mat4 ViewTranslateZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4));
		glm::mat4 ViewRotateX = glm::rotate(ViewTranslateZ, -4.0f, glm::vec3(1.f, 0.f, 0.f));
		glm::mat4 ViewRotateY = glm::rotate(ViewRotateX, -4.0f, glm::vec3(0.f, 1.f, 0.f));
		glm::mat4 View = ViewRotateY;
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;

		*Pointer = MVP;
	}
	// Update of the MVP matrix for the depth pass
	{
		glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f,-4.0f, 8.0f);
		glm::mat4 View = glm::lookAt(glm::vec3(0.5, 1.0, 2.0), glm::vec3(0), glm::vec3(0, 0, 1));
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 DepthMVP = Projection * View * Model;
		*(Pointer + 1) = DepthMVP;

		glm::mat4 BiasMatrix(
			0.5, 0.0, 0.0, 0.0, 
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0);

		*(Pointer + 2) = BiasMatrix * DepthMVP;
	}

	// Make sure the uniform buffer is uploaded
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	glBindBufferBase(GL_UNIFORM_BUFFER, uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);

	renderShadow();
	renderFramebuffer();
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