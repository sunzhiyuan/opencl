//=============================================================================
// FILE:        common.h
// 
// DESCRIPTION: basic types
// 
// AUTHOR:      QUALCOMM
// 
//                  Copyright (c) 2008 QUALCOMM Incorporated.
//                            All Rights Reserved.
//                         QUALCOMM Proprietary/GTDR
//=============================================================================

#pragma once

#include "FrmPlatform.h"

#if defined(OS_WIN32)
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#endif // OS_WIN32

#if defined(OS_WM)
#include <windows.h>
#include <aygshell.h>
#endif // OS_WM

// Standard includes
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h> 


#if !defined(OS_OSX)
    #include <malloc.h>
#else //if defined(OS_OSX)
    #include <stdlib.h>
    #include "FrmPlatform.h"
#endif



#include <string.h>
#include <math.h>


// GLES 2.0 Includes
#if !defined(OS_ANDROID)
#include <EGL/egl.h>
#endif //  !defined(OS_ANDROID)

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#if !defined(OS_ANDROID)
// GLES 2.0 Extensions
#define GL_GLEXT_PROTOTYPES
#endif // !defined(OS_ANDROID)


#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

//-----------------------------------------------------------------------------
// WIN MOBILE
//-----------------------------------------------------------------------------
#ifdef OS_WM

typedef __int64         int64;
typedef void*           EGLImageKHR;

//-----------------------------------------------------------------------------
// WINDOWS
//-----------------------------------------------------------------------------
#elif OS_WIN32

typedef __int64         int64;

//-----------------------------------------------------------------------------
// LINUX
//-----------------------------------------------------------------------------
#elif OS_LINUX

typedef long long       int64;
#define MAX_PATH 260

//-----------------------------------------------------------------------------
// OSX
//-----------------------------------------------------------------------------
#elif OS_OSX

typedef long long       int64;
#define MAX_PATH 260

//-----------------------------------------------------------------------------
// ANDROID
//-----------------------------------------------------------------------------
#elif OS_ANDROID

typedef long long       int64;

//-----------------------------------------------------------------------------
// OTHER
//-----------------------------------------------------------------------------
#else

#error "Needs OS specific implementation"

#endif //os specific

//------------------------------------------------------------------------
// Functions Declarations
//------------------------------------------------------------------------
void    CovertLocalFileName(const char* pszFileName, char* pszFullFileName, UINT32 nFullFileNameBufSize);
BOOL    LoadFileIntoBuffer(const char* pszFileName, void** ppBuffer, UINT32* pBufferSize);

// These live in the platform specific files
//void    LogError(const char* pszFormat, ...);
//void    LogInfo(const char* pszFormat, ...);
//UINT32  GetTimeMS();

