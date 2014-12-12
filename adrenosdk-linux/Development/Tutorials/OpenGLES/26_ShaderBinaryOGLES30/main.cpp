//--------------------------------------------------------------------------------------
// File: main.cpp
//
// Desc: Tutorial showing how to save and load a shader program binary. When
//		 saving the program binary, the binary format is also saved in the first
//		 4 bytes of the program binary file, then the program binary follows.
//		 This saved file, when loaded, has its first 4 bytes read and saved as
//		 the program binary format to be passed in when the shader program
//		 binary is loaded. To change whether the program is loading or saving
//		 the binary, you must modify the Initialize() function and insert the
//		 InitializeAndSaveBinary() or InitializeAndLoadBinary() function as the
//		 initializer.
//
// Author: QUALCOMM Technologies, Inc. Advanced Content Group
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//--------------------------------------------------------------------------------------
#define _WIN32_WINNT 0x0502
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
const CHAR*  g_strWindowTitle = "ShaderBinary";
const UINT32 g_nWindowWidth   = 800;
const UINT32 g_nWindowHeight  = 480;
const FLOAT  g_fAspectRatio   = (FLOAT)g_nWindowWidth / (FLOAT)g_nWindowHeight;

GLuint       g_hShaderProgram        = 0;
GLuint       g_hModelViewMatrixLoc   = 0;
GLuint       g_hProjMatrixLoc        = 0;
GLuint       g_hVertexLoc            = 0;
GLuint       g_hColorLoc             = 1;

const char   *g_shaderBinFileName	 = "Tutorials/OpenGLES/shader.bin";

//--------------------------------------------------------------------------------------
// Name: g_strVertexShader / g_strFragmentShader
// Desc: The vertex and fragment shader programs
//--------------------------------------------------------------------------------------
const CHAR* g_strVertexShader = 
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

const CHAR* g_strFragmentShader = 
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
// Name: InitializeAndSaveBinary()
// Desc: Initialize the scene and save shader binary
//--------------------------------------------------------------------------------------
BOOL InitializeAndSaveBinary()
{
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

		// Before linking, pass binary retrieval hint
		glProgramParameteri( g_hShaderProgram, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);
	
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
               
        // Get the binary
        GLenum binaryFormat = 0;
        GLsizei writtenBinaryLength = 0;
        GLint   binaryLength;
        GLvoid* finalBinary;
        GLvoid* programBinary;
        FILE*   outfile;
        glGetProgramiv( g_hShaderProgram, GL_PROGRAM_BINARY_LENGTH, &binaryLength );
        
        binaryLength += sizeof(GLenum); // add enough space for a GLenum
        finalBinary = (GLvoid*)malloc(binaryLength);
        
        // set the program binary pointer
        programBinary = ((unsigned char*)finalBinary)+sizeof(GLenum);
        
        // get program binary call
        glGetProgramBinary( g_hShaderProgram, binaryLength, &writtenBinaryLength, &binaryFormat, programBinary );

		GLenum error = glGetError();
		if ( error != GL_NO_ERROR )
			return FALSE;
			
		// save the binary format with the file
		*((GLenum *)finalBinary) = binaryFormat;
			
        // Save the binary
        outfile = fopen( g_shaderBinFileName, "wb" );
        fwrite(finalBinary, binaryLength, 1, outfile);
        fclose(outfile);
        free(finalBinary);
        
        // Get uniform locations
        g_hModelViewMatrixLoc = glGetUniformLocation( g_hShaderProgram, "g_matModelView" );
        g_hProjMatrixLoc      = glGetUniformLocation( g_hShaderProgram, "g_matProj" );

        glDeleteShader( hVertexShader );
        glDeleteShader( hFragmentShader );
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitializeAndLoadBinary()
// Desc: Initialize the scene and load shader binary
//--------------------------------------------------------------------------------------
BOOL InitializeAndLoadBinary()
{
    // Create the shader program needed to render the scene
    {
		GLint   fileLength, programBinaryLength;
        GLvoid* fileBinary = NULL;
        GLvoid* programBinary = NULL;
        FILE*   infile;
        GLint	binFormat;
        
        // Load program binary from file
        infile = fopen( g_shaderBinFileName, "rb" );
        fseek( infile, 0, SEEK_END );
        fileLength = (GLint)ftell(infile);
        fileBinary = (GLvoid*)malloc(fileLength);
        fseek( infile, 0, SEEK_SET );
        fread( fileBinary, fileLength, 1, infile );
        fclose( infile );
        
        // set the program binary pointer and binary length
        programBinary = (unsigned char *)fileBinary + sizeof(GLenum);
        programBinaryLength = fileLength - sizeof(GLenum);
        
        // get the shader binary format from the beginning of the file
        binFormat = *(GLenum *)fileBinary;
        
        // Init the shader program
        g_hShaderProgram = glCreateProgram();
        
        // Init attributes BEFORE loading binary
        glBindAttribLocation( g_hShaderProgram, g_hVertexLoc,   "g_vPosition" );
        glBindAttribLocation( g_hShaderProgram, g_hColorLoc,    "g_vColor" );
        
        // Load program binary
		glProgramBinary( g_hShaderProgram, binFormat, programBinary, programBinaryLength );

        // Check for link success
        GLint nLinkResult = 0;
        glGetProgramiv( g_hShaderProgram, GL_LINK_STATUS, &nLinkResult );
        
        // Ignore link status
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
        
        free(fileBinary);
    }

    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: InitializeAndSaveBinary()
// Desc: Initialize the scene and save shader binary
//--------------------------------------------------------------------------------------
BOOL Initialize()
{
	// Run and save the program binary
	//return InitializeAndSaveBinary();
	
	// Run and load the program binary
	return InitializeAndLoadBinary();
}


//--------------------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Cleanup any objects created in Initialize();
//--------------------------------------------------------------------------------------
VOID Cleanup()
{
    glDeleteProgram( g_hShaderProgram );
}


//--------------------------------------------------------------------------------------
// Name: RenderScene()
// Desc: Render the scene
//--------------------------------------------------------------------------------------
VOID RenderScene( FLOAT fTime )
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
// Desc: Render the scene
//--------------------------------------------------------------------------------------
VOID Render()
{
    static float fTime = 0.0f;
    fTime += 0.01f;
    
    // Render the scene into the primary backbuffer with a dark blue background
    glClearColor( 0.0f, 0.0f, 0.5f, 1.0f );
    RenderScene( fTime );
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


//--------------------------------------------------------------------------------------
// Name: WinMain()
// Desc: Application entry-point on the Windows platform
//--------------------------------------------------------------------------------------
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
        wc.lpszClassName  = "Adreno SDK Window";                 // Set the class name
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
            "Adreno SDK Window",                   // Class
            g_strWindowTitle,                      // Title
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

    // Get the display
    EGLDisplay eglDisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
    if( eglDisplay == EGL_NO_DISPLAY )
        return FALSE;

    // Set our EGL API to OpenGL ES
    if( FALSE == eglBindAPI( EGL_OPENGL_ES_API ) )
        return FALSE;

    // Initialize EGL
    EGLint nMajorVersion, nMinorVersion;
    if( FALSE == eglInitialize( eglDisplay, &nMajorVersion, &nMinorVersion ) )
        return FALSE;
    
    // Get total number of configs
    EGLint nNumConfigs;
    if( FALSE == eglGetConfigs( eglDisplay, NULL, 0, &nNumConfigs ) )
        return FALSE;

    // Get the first config
    EGLConfig configs[10];
    if (FALSE == eglGetConfigs(eglDisplay, configs, 1, &nNumConfigs) )
        return FALSE;

    // Get the first config's Color buffer size
    EGLint nBuffSize;
    eglGetConfigAttrib(eglDisplay, configs[0], EGL_BUFFER_SIZE, &nBuffSize);

    // Test the display configuration
    EGLConfig eglConfig;

	if (nBuffSize == 64)
    {
        // Build the attibute list
        EGLint AttributeList[] = 
        { 
            EGL_RED_SIZE,		16,
            EGL_GREEN_SIZE,	    16,
            EGL_BLUE_SIZE,	    16,
            EGL_ALPHA_SIZE,	    16,
            EGL_DEPTH_SIZE,	    24,
            EGL_STENCIL_SIZE,   8,
            EGL_SAMPLE_BUFFERS, 0,
            EGL_SAMPLES,		0,
            EGL_NONE
        };

        // Choose config based on the requested attributes
        if( FALSE == eglChooseConfig( eglDisplay, AttributeList, &eglConfig, 1, &nNumConfigs ) )
            return FALSE;
    }
    else if (nBuffSize == 32)
    {
        // Build the attibute list
        EGLint AttributeList[] = 
        { 
            EGL_RED_SIZE,		8,
            EGL_GREEN_SIZE,	    8,
            EGL_BLUE_SIZE,	    8,
            EGL_ALPHA_SIZE,	    8,
            EGL_DEPTH_SIZE,	    24,
            EGL_STENCIL_SIZE,   8,
            EGL_SAMPLE_BUFFERS, 0,
            EGL_SAMPLES,		0,
            EGL_NONE
        };

        // Choose config based on the requested attributes
        if( FALSE == eglChooseConfig( eglDisplay, AttributeList, &eglConfig, 1, &nNumConfigs ) )
            return FALSE;
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
            return FALSE;
    }
    else return FALSE; // unsupported display

    // Create a window surface
    EGLSurface eglSurface = eglCreateWindowSurface( eglDisplay, eglConfig, hWindow, NULL );
    if( EGL_NO_SURFACE == eglSurface )
        return FALSE;

    // Create a OpenGL ES 3.0 rendering context 
	// Wolf ... need to ask for 3.0 
    EGLint ContextAttribList[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
    EGLContext eglContext = eglCreateContext( eglDisplay, eglConfig, EGL_NO_CONTEXT, ContextAttribList );
    if( EGL_NO_CONTEXT == eglContext )
        return FALSE;

    // Make the context current
    if( FALSE == eglMakeCurrent( eglDisplay, eglSurface, eglSurface, eglContext ) )
        return FALSE;

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

