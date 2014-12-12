//--------------------------------------------------------------------------------------
// File: FrmBasicTypes.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef _FRM_BASIC_TYPES_H_
#define _FRM_BASIC_TYPES_H_


//--------------------------------------------------------------------------------------
// Common types
//--------------------------------------------------------------------------------------

typedef unsigned char      BYTE;
typedef unsigned char      BYTE8;

typedef char               CHAR;
typedef char               CHAR8;
typedef unsigned char      UCHAR;
typedef unsigned char      UCHAR8;

typedef wchar_t            WCHAR;
typedef unsigned short     UCHAR16;

typedef signed char        INT8;
typedef signed short       INT16;
typedef signed int         INT32;
typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef unsigned int       UINT32;

typedef float              FLOAT32;
typedef double             FLOAT64;

typedef int                BOOL;

#ifndef VOID
    #define VOID void
#endif

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef NULL
    #define NULL 0
#endif


#endif // _FRM_BASIC_TYPES_H_
