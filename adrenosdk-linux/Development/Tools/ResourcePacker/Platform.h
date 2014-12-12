#ifndef __PLATFORM_H
#define __PLATFORM_H

//--------------------------------------------------------------------------------------
// Common types
//--------------------------------------------------------------------------------------
typedef unsigned char      BYTE;

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
typedef unsigned short     USHORT;
typedef unsigned short     UINT16;
typedef unsigned int       UINT;
typedef unsigned int       UINT32;

typedef float              FLOAT;
typedef float              FLOAT32;
typedef double             FLOAT64;
typedef double             DOUBLE;

typedef int                BOOL;

#ifndef min
#define min(A,B) ((A) < (B) ? (A) : (B))
#endif

#ifndef max
#define max(A,B) ((A) > (B) ? (A) : (B))
#endif

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

#ifndef HRESULT
#define HRESULT long
#endif

#ifndef E_FAIL
#define E_FAIL 0x80000008L
#endif


#endif
