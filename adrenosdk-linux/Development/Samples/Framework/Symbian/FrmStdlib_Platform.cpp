//--------------------------------------------------------------------------------------
// File: StdLib_Symbian.cpp
// Desc: Wrapper around various C standard library functions for portability.
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <ngi/libc/string.h>
#include <ngi/libc/stdlib.h>
#include "FrmBasicTypes.h"


// Memory allocation functions
VOID*       FrmMalloc( UINT32 nSize )                          { return malloc( (size_t)nSize ); }
VOID*       FrmRealloc( VOID* iptr, UINT32 nSize )             { return realloc( iptr, (size_t)nSize ); }
VOID        FrmFree( VOID* ptr )                               { free( ptr ); }

// Memory manipulation functions
VOID*       FrmMemcpy( VOID* s, const VOID* ct, UINT32 n )     { return memcpy( s, ct, n ); }
VOID*       FrmMemmove( VOID* s, const VOID* ct, UINT32 n )    { return memmove( s, ct, n ); }
INT32       FrmMemcmp( const VOID* cs, const VOID* ct, UINT32 n )   { return memcmp( cs, ct, n ); }
const VOID* FrmMemchr( const VOID* cs, BYTE c, UINT32 n )      { return memchr( cs, c, n ); }
const VOID* FrmMemset( VOID* s, BYTE c, UINT32 n )             { return memset( s, c, n ); }

// String manipulation functions
CHAR*       FrmStrcpy( CHAR* s, const CHAR* ct )              { return strcpy( s, ct ); }
CHAR*       FrmStrncpy( CHAR* s, const CHAR* ct, UINT32 n )   { return strncpy( s, ct, n ); }
CHAR*       FrmStrcat( CHAR* s, const CHAR* ct )              { return strcat( s, ct ); }
CHAR*       FrmStrncat( CHAR* s, const CHAR* ct, UINT32 n )   { return strncat( s, ct, n ); }
INT32       FrmStrcmp( const CHAR* cs, const CHAR* ct )       { return strcmp( cs, ct ); }
INT32       FrmStricmp( const CHAR* cs, const CHAR* ct )      { return stricmp( cs, ct ); }
INT32       FrmStrncmp( const CHAR* cs, const CHAR* ct, UINT32 n )   { return strncmp( cs, ct, n ); }
const CHAR* FrmStrchr( const CHAR* cs, CHAR c )               { return strchr( cs, c ); }
const CHAR* FrmStrrchr( const CHAR* cs, CHAR c )              { return strrchr( cs, c ); }
UINT32      FrmStrspn( const CHAR* cs, const CHAR* ct )       { return strspn( cs, ct ); }
UINT32      FrmStrcspn( const CHAR* cs, const CHAR* ct )      { return strcspn( cs, ct ); }
const CHAR* FrmStrpbrk( const CHAR* cs, const CHAR* ct )      { return strpbrk( cs, ct ); }
const CHAR* FrmStrstr( const CHAR* cs, const CHAR* ct )       { return strstr( cs, ct ); }
UINT32      FrmStrlen( const CHAR* cs )                       { return strlen( cs ); }
CHAR*       FrmStrerror( INT32 n )                            { return strerror( n ); }
CHAR*       FrmStrtok( CHAR* s, const CHAR* ct )              { return strtok( s, ct ); }

