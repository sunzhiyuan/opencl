//--------------------------------------------------------------------------------------
// File: FrmMath.h
// Desc: 
//
// Author:     QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#pragma once
#include <stdlib.h>
#include <math.h>


// Let's use row-major matrices.
#define FRM_ROW_MAJOR

// Whether or not the glUniformMatrix API needs to transpose matrices
#ifdef FRM_ROW_MAJOR
    const BOOL FRM_TRANSPOSE = FALSE;
#else
    const BOOL FRM_TRANSPOSE = TRUE;
#endif


//--------------------------------------------------------------------------------------
// Degree/radian conversions
//--------------------------------------------------------------------------------------
const FLOAT32 FRM_PI = 3.1415926f;

#define FrmRadians( a )   ( (a) * (FRM_PI/180.0f) )
#define FrmDegrees( a )   ( (a) * (180.0f/FRM_PI) )


//--------------------------------------------------------------------------------------
// Name: srtuct FRMCOLOR
// Desc: A 4-component color, packed into a 32-bit value
//--------------------------------------------------------------------------------------
typedef struct _FRMCOLOR
{
    union
    {
        struct{ UINT8 r, g, b, a; };
        UINT32 v; // Swizzled as ABGR
    };

#ifdef __cplusplus
    _FRMCOLOR() {}
    _FRMCOLOR( UINT32 val ) { v = val; }
    _FRMCOLOR( FLOAT32 _r, FLOAT32 _g, FLOAT32 _b, FLOAT32 _a=1.0f ) { r = (UINT8)(255*_r); g = (UINT8)(255*_g); b = (UINT8)(255*_b); a = (UINT8)(255*_a); }
    _FRMCOLOR& operator = ( const _FRMCOLOR& val ) { v = val.v; return *this; }
    _FRMCOLOR& operator = ( const UINT32& val ) { v = val; return *this; }
#endif // __cplusplus
} FRMCOLOR;


//--------------------------------------------------------------------------------------
// Name: struct FRMVECTOR2
// Desc: A 2-component vector
//--------------------------------------------------------------------------------------
typedef struct _FRMVECTOR2
{
    union
    {
        struct{ FLOAT32 x, y; };
        struct{ FLOAT32 tu, tv; };
        FLOAT32 v[2];
    };

#ifdef __cplusplus
    _FRMVECTOR2() {}
    _FRMVECTOR2( FLOAT32 _x, FLOAT32 _y ) { x = _x; y = _y; }
    _FRMVECTOR2& operator  = ( const _FRMVECTOR2& val ) { x = val.x; y = val.y; return *this; }
    _FRMVECTOR2& operator += ( const _FRMVECTOR2& val ) { x += val.x; y += val.y; return *this; }
    _FRMVECTOR2& operator -= ( const _FRMVECTOR2& val ) { x -= val.x; y -= val.y; return *this; }
    _FRMVECTOR2& operator *= ( FLOAT32 val ) { x *= val; y *= val; return *this; }
    _FRMVECTOR2& operator /= ( FLOAT32 val ) { x /= val; y /= val; return *this; }
    _FRMVECTOR2  operator + () const { return _FRMVECTOR2(x,y); }
    _FRMVECTOR2  operator - () const { return _FRMVECTOR2(-x,-y); }
    _FRMVECTOR2  operator + ( const _FRMVECTOR2& val ) const { return _FRMVECTOR2( x+val.x, y+val.y ); }
    _FRMVECTOR2  operator - ( const _FRMVECTOR2& val ) const { return _FRMVECTOR2( x-val.x, y-val.y ); }
    _FRMVECTOR2  operator * ( FLOAT32 val ) const { return _FRMVECTOR2( x*val, y*val ); }
    _FRMVECTOR2  operator / ( FLOAT32 val ) const { return _FRMVECTOR2( x/val, y/val ); }
    friend _FRMVECTOR2 operator * ( FLOAT32 s, const _FRMVECTOR2& v ){ return _FRMVECTOR2( s*v.x, s*v.y ); }
#endif // __cplusplus
} FRMVECTOR2;


//--------------------------------------------------------------------------------------
// Name: struct FRMVECTOR3
// Desc: A 3-component vector
//--------------------------------------------------------------------------------------
typedef struct _FRMVECTOR3
{
    union
    {
        struct{ FLOAT32 x, y, z; };
        struct{ FLOAT32 r, g, b; };
        FLOAT32 v[3];
    };

#ifdef __cplusplus
    _FRMVECTOR3() {}
    _FRMVECTOR3( FLOAT32 _x, FLOAT32 _y, FLOAT32 _z ) { x = _x; y = _y; z = _z; }
    _FRMVECTOR3& operator  = ( const _FRMVECTOR3& val ) { x = val.x; y = val.y; z = val.z; return *this; }
    _FRMVECTOR3& operator += ( const _FRMVECTOR3& val ) { x += val.x; y += val.y; z += val.z; return *this; }
    _FRMVECTOR3& operator -= ( const _FRMVECTOR3& val ) { x -= val.x; y -= val.y; z -= val.z; return *this; }
    _FRMVECTOR3& operator *= ( FLOAT32 val ) { x *= val; y *= val; z *= val; return *this; }
    _FRMVECTOR3& operator /= ( FLOAT32 val ) { x /= val; y /= val; z /= val; return *this; }
    _FRMVECTOR3  operator + () const { return _FRMVECTOR3(x,y,z); }
    _FRMVECTOR3  operator - () const { return _FRMVECTOR3(-x,-y,-z); }
    _FRMVECTOR3  operator + ( const _FRMVECTOR3& val ) const { return _FRMVECTOR3( x+val.x, y+val.y, z+val.z ); }
    _FRMVECTOR3  operator - ( const _FRMVECTOR3& val ) const { return _FRMVECTOR3( x-val.x, y-val.y, z-val.z ); }
    _FRMVECTOR3  operator * ( FLOAT32 val ) const { return _FRMVECTOR3( x*val, y*val, z*val ); }
    _FRMVECTOR3  operator / ( FLOAT32 val ) const { return _FRMVECTOR3( x/val, y/val, z/val ); }
    friend _FRMVECTOR3 operator * ( FLOAT32 s, const _FRMVECTOR3& v ){ return _FRMVECTOR3( s*v.x, s*v.y, s*v.z ); }
#endif // __cplusplus
} FRMVECTOR3;


//--------------------------------------------------------------------------------------
// Name: struct FRMVECTOR4
// Desc: A 4-component vector
//--------------------------------------------------------------------------------------
typedef struct _FRMVECTOR4
{
    union
    {
        struct{ FLOAT32 x, y, z, w; };
        struct{ FLOAT32 r, g, b, a; };
        FLOAT32 v[4];
    };

#ifdef __cplusplus
    _FRMVECTOR4() {}
    _FRMVECTOR4( FLOAT32 _x, FLOAT32 _y, FLOAT32 _z, FLOAT32 _w ) { x = _x; y = _y; z = _z; w = _w; }
    _FRMVECTOR4& operator  = ( const _FRMVECTOR4& val ) { x = val.x; y = val.y; z = val.z; w = val.w; return *this; }
    _FRMVECTOR4& operator += ( const _FRMVECTOR4& val ) { x += val.x; y += val.y; z += val.z; w += val.w; return *this; }
    _FRMVECTOR4& operator -= ( const _FRMVECTOR4& val ) { x -= val.x; y -= val.y; z -= val.z; w -= val.w; return *this; }
    _FRMVECTOR4& operator *= ( FLOAT32 val ) { x *= val; y *= val; z *= val; w *= val; return *this; }
    _FRMVECTOR4& operator /= ( FLOAT32 val ) { x /= val; y /= val; z /= val; w /= val; return *this; }
    _FRMVECTOR4  operator + () const { return _FRMVECTOR4(x,y,z,w); }
    _FRMVECTOR4  operator - () const { return _FRMVECTOR4(-x,-y,-z,-w); }
    _FRMVECTOR4  operator + ( const _FRMVECTOR4& val ) const { return _FRMVECTOR4( x+val.x, y+val.y, z+val.z, w+val.w ); }
    _FRMVECTOR4  operator - ( const _FRMVECTOR4& val ) const { return _FRMVECTOR4( x-val.x, y-val.y, z-val.z, w-val.w ); }
    _FRMVECTOR4  operator * ( FLOAT32 val ) const { return _FRMVECTOR4( x*val, y*val, z*val, w*val ); }
    _FRMVECTOR4  operator / ( FLOAT32 val ) const { return _FRMVECTOR4( x/val, y/val, z/val, w/val ); }
    friend _FRMVECTOR4 operator * ( FLOAT32 s, const _FRMVECTOR4& v ){ return _FRMVECTOR4( s*v.x, s*v.y, s*v.z, s*v.w ); }
#endif // __cplusplus
} FRMVECTOR4;


//--------------------------------------------------------------------------------------
// Name: struct FRMMATRIX3X3
// Desc: A 3x3 matrix
//--------------------------------------------------------------------------------------
typedef struct _FRMMATRIX3X3
{
    FLOAT32 m[3][3];

#ifdef FRM_ROW_MAJOR
    FLOAT32& M(UINT32 r, UINT32 c) { return m[r][c]; }
#else
    FLOAT32& M(UINT32 r, UINT32 c) { return m[c][r]; }
#endif
} FRMMATRIX3X3;


//--------------------------------------------------------------------------------------
// Name: struct FRMMATRIX4X3
// Desc: A 4x3 matrix
//--------------------------------------------------------------------------------------
typedef struct _FRMMATRIX4X3
{
#ifdef FRM_ROW_MAJOR
    FLOAT32 m[4][3];
#else
    FLOAT32 m[3][4];
#endif

#ifdef FRM_ROW_MAJOR
    FLOAT32& M(UINT32 r, UINT32 c) { return m[r][c]; }
#else
    FLOAT32& M(UINT32 r, UINT32 c) { return m[c][r]; }
#endif

} FRMMATRIX4X3;


//--------------------------------------------------------------------------------------
// Name: struct FRMMATRIX4X4
// Desc: A 4x4 matrix
//--------------------------------------------------------------------------------------
typedef struct _FRMMATRIX4X4
{
    FLOAT32 m[4][4];

#ifdef FRM_ROW_MAJOR
    FLOAT32& M(UINT32 r, UINT32 c) { return m[r][c]; }
#else
    FLOAT32& M(UINT32 r, UINT32 c) { return m[c][r]; }
#endif

} FRMMATRIX4X4;


//--------------------------------------------------------------------------------------
// Scalar math functions
//--------------------------------------------------------------------------------------
__inline FLOAT32    FrmFloor( FLOAT32 value );
__inline FLOAT32    FrmRand();
__inline FLOAT32    FrmSin( FLOAT32 deg );
__inline FLOAT32    FrmCos( FLOAT32 deg );
__inline FLOAT32    FrmTan( FLOAT32 deg );
__inline FLOAT32    FrmSqrt( FLOAT32 value );


//--------------------------------------------------------------------------------------
// Common helper functions
//--------------------------------------------------------------------------------------
__inline FLOAT32    FrmLerp( FLOAT32 t, FLOAT32 a, FLOAT32 b );
__inline FRMCOLOR   FrmLerp( FLOAT32 t, const FRMCOLOR& a, const FRMCOLOR& b );
__inline FRMVECTOR3 FrmSphrand( FLOAT32 fRadius );
__inline FLOAT32    ApplyVariance( FLOAT32 fValue, FLOAT32 fVariance );
__inline FLOAT32    FrmSCurve(const float t);
__inline FLOAT32    FrmVector2Dot( FRMVECTOR2& vVector0, FRMVECTOR2& vVector1 );
__inline FLOAT32    FrmVector2Length( FRMVECTOR2& vVector );
__inline FRMVECTOR2 FrmVector2Normalize( FRMVECTOR2& vVector );


//--------------------------------------------------------------------------------------
// Operations for 3-component vectors
//--------------------------------------------------------------------------------------
__inline FRMVECTOR3 FrmVector3Mul( FRMVECTOR3& vVector, FLOAT32 fScalar );
__inline FRMVECTOR3 FrmVector3Add( FRMVECTOR3& vVector0, FRMVECTOR3& vVector1 );
__inline FRMVECTOR3 FrmVector3Cross( FRMVECTOR3& vVector0, FRMVECTOR3& vVector1 );
__inline FLOAT32    FrmVector3Dot( FRMVECTOR3& vVector0, FRMVECTOR3& vVector1 );
__inline FLOAT32    FrmVector3Length( FRMVECTOR3& vVector );
__inline FRMVECTOR3 FrmVector3Normalize( FRMVECTOR3& vVector );
__inline FRMVECTOR4 FrmVector3Transform( FRMVECTOR3& vSrcVector, FRMMATRIX4X4& matSrcMatrix );
__inline FRMVECTOR3 FrmVector3TransformCoord( FRMVECTOR3& vSrcVector, FRMMATRIX4X4& matSrcMatrix );
__inline FRMVECTOR3 FrmVector3TransformNormal( FRMVECTOR3& vSrcVector, FRMMATRIX4X4& matSrcMatrix );


//--------------------------------------------------------------------------------------
// Operations for 4-component vectors
//--------------------------------------------------------------------------------------
__inline FRMVECTOR4 FrmVector4Transform( FRMVECTOR4& vSrcVector, FRMMATRIX4X4& matSrcMatrix );


//--------------------------------------------------------------------------------------
// Operations for 4x4 matrices
//--------------------------------------------------------------------------------------
__inline FRMMATRIX4X4 FrmMatrixIdentity();
__inline FRMMATRIX3X3 FrmMatrixNormalOrthonormal( FRMMATRIX4X4& matSrcMatrix );
__inline FRMMATRIX3X3 FrmMatrixNormal( FRMMATRIX4X4& matSrcMatrix );
__inline FRMMATRIX4X4 FrmMatrixMultiply( FRMMATRIX4X4& matSrcMatrixA, FRMMATRIX4X4& matSrcMatrixB );
__inline FRMMATRIX4X4 FrmMatrixInverse( FRMMATRIX4X4& matSrcMatrix );
__inline FRMMATRIX4X4 FrmMatrixInverseOrthonormal( FRMMATRIX4X4& matSrcMatrix );
__inline FRMMATRIX4X4 FrmMatrixTranspose( FRMMATRIX4X4& matSrcMatrix );
__inline FRMMATRIX4X4 FrmMatrixTranslate( FRMVECTOR3& vVector );
__inline FRMMATRIX4X4 FrmMatrixTranslate( FLOAT32 tx, FLOAT32 ty, FLOAT32 tz );
__inline FRMMATRIX4X4 FrmMatrixScale( FRMVECTOR3& vVector );
__inline FRMMATRIX4X4 FrmMatrixScale( FLOAT32 sx, FLOAT32 sy, FLOAT32 sz );
__inline FRMMATRIX4X4 FrmMatrixRotate( FLOAT32 fAngle, FRMVECTOR3& vAxis );
__inline FRMMATRIX4X4 FrmMatrixRotate( FLOAT32 fAngle, FLOAT32 rx, FLOAT32 ry, FLOAT32 rz );
__inline FRMMATRIX4X4 FrmMatrixLookAtLH( FRMVECTOR3& vPosition, FRMVECTOR3& vLookAt, FRMVECTOR3& vUp );
__inline FRMMATRIX4X4 FrmMatrixLookAtRH( FRMVECTOR3& vPosition, FRMVECTOR3& vLookAt, FRMVECTOR3& vUp );
__inline FRMMATRIX4X4 FrmMatrixPerspectiveFovLH( FLOAT32 fFOVy, FLOAT32 fAspect, FLOAT32 zNear, FLOAT32 zFar );
__inline FRMMATRIX4X4 FrmMatrixPerspectiveFovRH( FLOAT32 fFOVy, FLOAT32 fAspect, FLOAT32 zNear, FLOAT32 zFar );


//--------------------------------------------------------------------------------------
// Scalar math functions
//--------------------------------------------------------------------------------------
FLOAT32 FrmFloor( FLOAT32 value ) { return floorf( value ); }
FLOAT32 FrmRand()                 { return rand() * ( 1.0f / RAND_MAX ); }
FLOAT32 FrmSin( FLOAT32 deg )     { return sinf( deg ); }
FLOAT32 FrmCos( FLOAT32 deg )     { return cosf( deg ); }
FLOAT32 FrmTan( FLOAT32 deg )     { return tanf( deg ); }
FLOAT32 FrmSqrt( FLOAT32 value )  { return sqrtf( value ); }

    
//--------------------------------------------------------------------------------------
// Name: FrmLerp()
// Desc: 
//--------------------------------------------------------------------------------------
FLOAT32 FrmLerp( FLOAT32 t, FLOAT32 a, FLOAT32 b )
{
    return a + t * ( b - a );
}

FRMCOLOR FrmLerp( FLOAT32 t, const FRMCOLOR& vColor1, const FRMCOLOR& vColor2 )
{
    FRMCOLOR vResult;
    vResult.r = vColor1.r + (UINT8)( t * ( vColor2.r - vColor1.r ) );
    vResult.g = vColor1.g + (UINT8)( t * ( vColor2.g - vColor1.g ) );
    vResult.b = vColor1.b + (UINT8)( t * ( vColor2.b - vColor1.b ) );
    vResult.a = vColor1.a + (UINT8)( t * ( vColor2.a - vColor1.a ) );
    return vResult;
}


//--------------------------------------------------------------------------------------
// Name: FrmSphrand()
// Desc: Returns a random point within a sphere centered at (0,0)
//--------------------------------------------------------------------------------------
FRMVECTOR3 FrmSphrand( FLOAT32 fRadius )
{
    // Calculate a random normalized vector
    FRMVECTOR3 vResult;
    vResult.x = 2.0f * FrmRand() - 1.0f;
    vResult.y = 2.0f * FrmRand() - 1.0f;
    vResult.z = 2.0f * FrmRand() - 1.0f;

    FLOAT32 fLength = FrmVector3Length( vResult );

    // Scale by a number between 0 and radius
    return fRadius * FrmRand() * vResult / fLength;
}


//--------------------------------------------------------------------------------------
// Name: ApplyVariance()
// Desc: Applies the specified amount of random variance to the specified value.
//--------------------------------------------------------------------------------------
FLOAT32 ApplyVariance( FLOAT32 fValue, FLOAT32 fVariance )
{
    return fValue - 0.5f * fVariance + FrmRand() * fVariance;
}

FLOAT32 FrmSCurve( const FLOAT32 t )
{
    return t * t * (3 - 2 * t);
}


//--------------------------------------------------------------------------------------
// Name: FrmVector2()
// Desc: FRMVECTOR2 functions
//--------------------------------------------------------------------------------------
FLOAT32 FrmVector2Dot( FRMVECTOR2& vVector0, FRMVECTOR2& vVector1 )
{
    return ( vVector0.x * vVector1.x +
             vVector0.y * vVector1.y );
}


FLOAT32 FrmVector2Length( FRMVECTOR2& vVector )
{
    return vVector.x * vVector.x + vVector.y * vVector.y;
}


FRMVECTOR2 FrmVector2Normalize( FRMVECTOR2& vVector )
{
    FLOAT32 fLength = FrmSqrt( FrmVector2Length( vVector ) );
    if( fLength != 0.0f )
        fLength = 1.0f / fLength;
    
    return vVector * fLength;
}


//--------------------------------------------------------------------------------------
// Name: FrmVector3()
// Desc: FRMVECTOR3 functions
//--------------------------------------------------------------------------------------
FRMVECTOR3 FrmVector3Normalize( FRMVECTOR3& vVector )
{
    FLOAT32 fLength = FrmVector3Length( vVector );
    if( fLength != 0.0f )
        fLength = 1.0f / fLength;
    
    return vVector * fLength;
}


FRMVECTOR3 FrmVector3Mul( FRMVECTOR3& vVector, FLOAT32 fScalar )
{
    FRMVECTOR3 vResult;
    vResult.x = vVector.x * fScalar;
    vResult.y = vVector.y * fScalar;
    vResult.z = vVector.z * fScalar;
    return vResult;
}


FRMVECTOR3 FrmVector3Add( FRMVECTOR3& vVector0, FRMVECTOR3& vVector1 )
{
    FRMVECTOR3 vResult;
    vResult.x = vVector0.x + vVector1.x;
    vResult.y = vVector0.y + vVector1.y;
    vResult.z = vVector0.z + vVector1.z;
    return vResult;
}


FRMVECTOR3 FrmVector3Cross( FRMVECTOR3& vVector0, FRMVECTOR3& vVector1 )
{
    FRMVECTOR3 vResult;
    vResult.x = vVector0.y * vVector1.z - vVector0.z * vVector1.y;
    vResult.y = vVector0.z * vVector1.x - vVector0.x * vVector1.z;
    vResult.z = vVector0.x * vVector1.y - vVector0.y * vVector1.x;
    return vResult;
}


FLOAT32 FrmVector3Dot( FRMVECTOR3& vVector0, FRMVECTOR3& vVector1 )
{
    return ( vVector0.x * vVector1.x +
             vVector0.y * vVector1.y +
             vVector0.z * vVector1.z );
}


FLOAT32 FrmVector3Length( FRMVECTOR3& vVector )
{
    return FrmSqrt( vVector.x * vVector.x +
                    vVector.y * vVector.y +
                    vVector.z * vVector.z );
}


//--------------------------------------------------------------------------------------
// Name: FrmMatrix()
// Desc: FRMMATRIX4X4 functions
//--------------------------------------------------------------------------------------
FRMMATRIX4X4 FrmMatrixIdentity()
{
    FRMMATRIX4X4 matResult;

    matResult.M(0,0) = 1.0f;
    matResult.M(0,1) = 0.0f;
    matResult.M(0,2) = 0.0f;
    matResult.M(0,3) = 0.0f;

    matResult.M(1,0) = 0.0f;
    matResult.M(1,1) = 1.0f;
    matResult.M(1,2) = 0.0f;
    matResult.M(1,3) = 0.0f;

    matResult.M(2,0) = 0.0f;
    matResult.M(2,1) = 0.0f;
    matResult.M(2,2) = 1.0f;
    matResult.M(2,3) = 0.0f;

    matResult.M(3,0) = 0.0f;
    matResult.M(3,1) = 0.0f;
    matResult.M(3,2) = 0.0f;
    matResult.M(3,3) = 1.0f;

    return matResult;
}


FRMMATRIX4X4 FrmMatrixLookAtLH( FRMVECTOR3& vPosition, FRMVECTOR3& vLookAt, FRMVECTOR3& vUp )
{
    FRMVECTOR3 ZAxis = vLookAt - vPosition;
    FRMVECTOR3 XAxis = FrmVector3Cross( vUp, ZAxis );
    FRMVECTOR3 YAxis = FrmVector3Cross( ZAxis, XAxis );

    XAxis = FrmVector3Normalize( XAxis );
    YAxis = FrmVector3Normalize( YAxis );
    ZAxis = FrmVector3Normalize( ZAxis );

    FRMMATRIX4X4 matResult;

    matResult.M(0,0) = XAxis.x;
    matResult.M(0,1) = YAxis.x;
    matResult.M(0,2) = ZAxis.x;
    matResult.M(0,3) = 0.0f;

    matResult.M(1,0) = XAxis.y;
    matResult.M(1,1) = YAxis.y;
    matResult.M(1,2) = ZAxis.y;
    matResult.M(1,3) = 0.0f;

    matResult.M(2,0) = XAxis.z;
    matResult.M(2,1) = YAxis.z;
    matResult.M(2,2) = ZAxis.z;
    matResult.M(2,3) = 0.0f;

    matResult.M(3,0) = -FrmVector3Dot( XAxis, vPosition );
    matResult.M(3,1) = -FrmVector3Dot( YAxis, vPosition );
    matResult.M(3,2) = -FrmVector3Dot( ZAxis, vPosition );
    matResult.M(3,3) = 1.0f;

    return matResult;
}


FRMMATRIX4X4 FrmMatrixLookAtRH( FRMVECTOR3& vPosition, FRMVECTOR3& vLookAt, FRMVECTOR3& vUp )
{
    FRMVECTOR3 ZAxis = vLookAt - vPosition;
    FRMVECTOR3 XAxis = FrmVector3Cross( vUp, ZAxis );
    FRMVECTOR3 YAxis = FrmVector3Cross( ZAxis, XAxis );
    XAxis = FrmVector3Normalize( XAxis );
    YAxis = FrmVector3Normalize( YAxis );
    ZAxis = FrmVector3Normalize( ZAxis );

    FRMMATRIX4X4 matResult;

    matResult.M(0,0) = -XAxis.x;
    matResult.M(0,1) = +YAxis.x;
    matResult.M(0,2) = -ZAxis.x;
    matResult.M(0,3) = 0.0f;

    matResult.M(1,0) = -XAxis.y;
    matResult.M(1,1) = +YAxis.y;
    matResult.M(1,2) = -ZAxis.y;
    matResult.M(1,3) = 0.0f;

    matResult.M(2,0) = -XAxis.z;
    matResult.M(2,1) = +YAxis.z;
    matResult.M(2,2) = -ZAxis.z;
    matResult.M(2,3) = 0.0f;

    matResult.M(3,0) = +FrmVector3Dot( XAxis, vPosition );
    matResult.M(3,1) = -FrmVector3Dot( YAxis, vPosition );
    matResult.M(3,2) = +FrmVector3Dot( ZAxis, vPosition );
    matResult.M(3,3) = 1.0f;

    return matResult;
}


//--------------------------------------------------------------------------------------
// Name: FrmVector3TransformNormal()
// Desc: Transforms a normal through a matrix
//--------------------------------------------------------------------------------------
FRMVECTOR3 FrmVector3TransformNormal( FRMVECTOR3& vSrcVector, FRMMATRIX4X4& matSrcMatrix )
{
    FRMVECTOR3 vDstVector;
    vDstVector.x = matSrcMatrix.M(0,0) * vSrcVector.x +
                   matSrcMatrix.M(1,0) * vSrcVector.y +
                   matSrcMatrix.M(2,0) * vSrcVector.z;
    vDstVector.y = matSrcMatrix.M(0,1) * vSrcVector.x +
                   matSrcMatrix.M(1,1) * vSrcVector.y +
                   matSrcMatrix.M(2,1) * vSrcVector.z;
    vDstVector.z = matSrcMatrix.M(0,2) * vSrcVector.x +
                   matSrcMatrix.M(1,2) * vSrcVector.y +
                   matSrcMatrix.M(2,2) * vSrcVector.z;
    return FrmVector3Normalize( vDstVector );
}


//--------------------------------------------------------------------------------------
// Name: FrmVector3TransformCoord()
// Desc: Transforms a 3-dimensional coordinate through a matrix
//--------------------------------------------------------------------------------------
FRMVECTOR3 FrmVector3TransformCoord( FRMVECTOR3& vSrcVector, FRMMATRIX4X4& matSrcMatrix )
{
    FRMVECTOR3 vDstVector;
    vDstVector.x = matSrcMatrix.M(0,0) * vSrcVector.x +
                   matSrcMatrix.M(1,0) * vSrcVector.y +
                   matSrcMatrix.M(2,0) * vSrcVector.z +
                   matSrcMatrix.M(3,0);
    vDstVector.y = matSrcMatrix.M(0,1) * vSrcVector.x +
                   matSrcMatrix.M(1,1) * vSrcVector.y +
                   matSrcMatrix.M(2,1) * vSrcVector.z +
                   matSrcMatrix.M(3,1);
    vDstVector.z = matSrcMatrix.M(0,2) * vSrcVector.x +
                   matSrcMatrix.M(1,2) * vSrcVector.y +
                   matSrcMatrix.M(2,2) * vSrcVector.z +
                   matSrcMatrix.M(3,2);
    return vDstVector;
}


//--------------------------------------------------------------------------------------
// Name: FrmVector3Transform()
// Desc: Transforms a vector through a matrix
//--------------------------------------------------------------------------------------
FRMVECTOR4 FrmVector3Transform( FRMVECTOR3& vSrcVector, FRMMATRIX4X4& matSrcMatrix )
{
    FRMVECTOR4 vDstVector;
    vDstVector.x = matSrcMatrix.M(0,0) * vSrcVector.x +
                   matSrcMatrix.M(1,0) * vSrcVector.y +
                   matSrcMatrix.M(2,0) * vSrcVector.z +
                   matSrcMatrix.M(3,0);
    vDstVector.y = matSrcMatrix.M(0,1) * vSrcVector.x +
                   matSrcMatrix.M(1,1) * vSrcVector.y +
                   matSrcMatrix.M(2,1) * vSrcVector.z +
                   matSrcMatrix.M(3,1);
    vDstVector.z = matSrcMatrix.M(0,2) * vSrcVector.x +
                   matSrcMatrix.M(1,2) * vSrcVector.y +
                   matSrcMatrix.M(2,2) * vSrcVector.z +
                   matSrcMatrix.M(3,2);
    vDstVector.w = 1.0f;
    return vDstVector;
}

FRMVECTOR4 FrmVector4Transform( FRMVECTOR4& vSrcVector, FRMMATRIX4X4& matSrcMatrix )
{
    FRMVECTOR4 vDstVector;
    vDstVector.x = matSrcMatrix.M(0,0) * vSrcVector.x +
                   matSrcMatrix.M(1,0) * vSrcVector.y +
                   matSrcMatrix.M(2,0) * vSrcVector.z +
                   matSrcMatrix.M(3,0) * vSrcVector.w;
    vDstVector.y = matSrcMatrix.M(0,1) * vSrcVector.x +
                   matSrcMatrix.M(1,1) * vSrcVector.y +
                   matSrcMatrix.M(2,1) * vSrcVector.z +
                   matSrcMatrix.M(3,1) * vSrcVector.w;
    vDstVector.z = matSrcMatrix.M(0,2) * vSrcVector.x +
                   matSrcMatrix.M(1,2) * vSrcVector.y +
                   matSrcMatrix.M(2,2) * vSrcVector.z +
                   matSrcMatrix.M(3,2) * vSrcVector.w;
    vDstVector.w = matSrcMatrix.M(0,3) * vSrcVector.x +
                   matSrcMatrix.M(1,3) * vSrcVector.y +
                   matSrcMatrix.M(2,3) * vSrcVector.z +
                   matSrcMatrix.M(3,3) * vSrcVector.w;
    return vDstVector;
}


//--------------------------------------------------------------------------------------
// Name: FrmMatrixPerspectiveFovLH()
// Desc: Sets the matrix to a perspective projection
//--------------------------------------------------------------------------------------
FRMMATRIX4X4 FrmMatrixPerspectiveFovLH( FLOAT32 fFOVy, FLOAT32 fAspect, 
                                        FLOAT32 zNear, FLOAT32 zFar )
{
    FLOAT32 SinFov = FrmSin( 0.5f * fFOVy );
    FLOAT32 CosFov = FrmCos( 0.5f * fFOVy );
    FLOAT32 h = CosFov / SinFov;
    FLOAT32 w  = h / fAspect;

    FRMMATRIX4X4 matResult;

    matResult.M(0,0) = w;
    matResult.M(0,1) = 0.0f;
    matResult.M(0,2) = 0.0f;
    matResult.M(0,3) = 0.0f;

    matResult.M(1,0) = 0.0f;
    matResult.M(1,1) = h;
    matResult.M(1,2) = 0.0f;
    matResult.M(1,3) = 0.0f;

    matResult.M(2,0) = 0.0f;
    matResult.M(2,1) = 0.0f;
    matResult.M(2,2) = -( zFar+zNear ) / ( zFar-zNear );
    matResult.M(2,3) = -( 2.0f*zFar*zNear ) / ( zFar-zNear );

    matResult.M(3,0) = 0.0f;
    matResult.M(3,1) = 0.0f;
    matResult.M(3,2) = -1.0f;
    matResult.M(3,3) = 0.0f;

    return matResult;
}


//--------------------------------------------------------------------------------------
// Name: FrmMatrixPerspectiveFovRH()
// Desc: Sets the matrix to a perspective projection
//--------------------------------------------------------------------------------------
FRMMATRIX4X4 FrmMatrixPerspectiveFovRH( FLOAT32 fFOVy, FLOAT32 fAspect,
                                        FLOAT32 zNear, FLOAT32 zFar )
{
    FLOAT32 SinFov = FrmSin( 0.5f * fFOVy );
    FLOAT32 CosFov = FrmCos( 0.5f * fFOVy );
    FLOAT32 h = CosFov / SinFov;
    FLOAT32 w  = h / fAspect;

    FRMMATRIX4X4 matResult;

    matResult.M(0,0) = w;
    matResult.M(0,1) = 0.0f;
    matResult.M(0,2) = 0.0f;
    matResult.M(0,3) = 0.0f;

    matResult.M(1,0) = 0.0f;
    matResult.M(1,1) = h;
    matResult.M(1,2) = 0.0f;
    matResult.M(1,3) = 0.0f;

    matResult.M(2,0) = 0.0f;
    matResult.M(2,1) = 0.0f;
    matResult.M(2,2) = -( zFar ) / ( zFar-zNear );
    matResult.M(2,3) = -1.0f;

    matResult.M(3,0) = 0.0f;
    matResult.M(3,1) = 0.0f;
    matResult.M(3,2) = -( zFar * zNear ) / ( zFar-zNear );
    matResult.M(3,3) = 0.0f;

    return matResult;
}


//--------------------------------------------------------------------------------------
// Name: FrmMatrixMultiply()
// Desc: Multiplies two matrices.
//--------------------------------------------------------------------------------------
FRMMATRIX4X4 FrmMatrixMultiply( FRMMATRIX4X4& matSrcMatrixA, FRMMATRIX4X4& matSrcMatrixB )
{
    FRMMATRIX4X4  DstMatrix;
    FRMMATRIX4X4* pSrc1 = &matSrcMatrixA;
    FRMMATRIX4X4* pSrc2 = &matSrcMatrixB;

    DstMatrix.M(0,0) = pSrc1->M(0,0)*pSrc2->M(0,0) + pSrc1->M(0,1)*pSrc2->M(1,0) + pSrc1->M(0,2)*pSrc2->M(2,0) + pSrc1->M(0,3)*pSrc2->M(3,0);
    DstMatrix.M(0,1) = pSrc1->M(0,0)*pSrc2->M(0,1) + pSrc1->M(0,1)*pSrc2->M(1,1) + pSrc1->M(0,2)*pSrc2->M(2,1) + pSrc1->M(0,3)*pSrc2->M(3,1);
    DstMatrix.M(0,2) = pSrc1->M(0,0)*pSrc2->M(0,2) + pSrc1->M(0,1)*pSrc2->M(1,2) + pSrc1->M(0,2)*pSrc2->M(2,2) + pSrc1->M(0,3)*pSrc2->M(3,2);
    DstMatrix.M(0,3) = pSrc1->M(0,0)*pSrc2->M(0,3) + pSrc1->M(0,1)*pSrc2->M(1,3) + pSrc1->M(0,2)*pSrc2->M(2,3) + pSrc1->M(0,3)*pSrc2->M(3,3);
    DstMatrix.M(1,0) = pSrc1->M(1,0)*pSrc2->M(0,0) + pSrc1->M(1,1)*pSrc2->M(1,0) + pSrc1->M(1,2)*pSrc2->M(2,0) + pSrc1->M(1,3)*pSrc2->M(3,0);
    DstMatrix.M(1,1) = pSrc1->M(1,0)*pSrc2->M(0,1) + pSrc1->M(1,1)*pSrc2->M(1,1) + pSrc1->M(1,2)*pSrc2->M(2,1) + pSrc1->M(1,3)*pSrc2->M(3,1);
    DstMatrix.M(1,2) = pSrc1->M(1,0)*pSrc2->M(0,2) + pSrc1->M(1,1)*pSrc2->M(1,2) + pSrc1->M(1,2)*pSrc2->M(2,2) + pSrc1->M(1,3)*pSrc2->M(3,2);
    DstMatrix.M(1,3) = pSrc1->M(1,0)*pSrc2->M(0,3) + pSrc1->M(1,1)*pSrc2->M(1,3) + pSrc1->M(1,2)*pSrc2->M(2,3) + pSrc1->M(1,3)*pSrc2->M(3,3);
    DstMatrix.M(2,0) = pSrc1->M(2,0)*pSrc2->M(0,0) + pSrc1->M(2,1)*pSrc2->M(1,0) + pSrc1->M(2,2)*pSrc2->M(2,0) + pSrc1->M(2,3)*pSrc2->M(3,0);
    DstMatrix.M(2,1) = pSrc1->M(2,0)*pSrc2->M(0,1) + pSrc1->M(2,1)*pSrc2->M(1,1) + pSrc1->M(2,2)*pSrc2->M(2,1) + pSrc1->M(2,3)*pSrc2->M(3,1);
    DstMatrix.M(2,2) = pSrc1->M(2,0)*pSrc2->M(0,2) + pSrc1->M(2,1)*pSrc2->M(1,2) + pSrc1->M(2,2)*pSrc2->M(2,2) + pSrc1->M(2,3)*pSrc2->M(3,2);
    DstMatrix.M(2,3) = pSrc1->M(2,0)*pSrc2->M(0,3) + pSrc1->M(2,1)*pSrc2->M(1,3) + pSrc1->M(2,2)*pSrc2->M(2,3) + pSrc1->M(2,3)*pSrc2->M(3,3);
    DstMatrix.M(3,0) = pSrc1->M(3,0)*pSrc2->M(0,0) + pSrc1->M(3,1)*pSrc2->M(1,0) + pSrc1->M(3,2)*pSrc2->M(2,0) + pSrc1->M(3,3)*pSrc2->M(3,0);
    DstMatrix.M(3,1) = pSrc1->M(3,0)*pSrc2->M(0,1) + pSrc1->M(3,1)*pSrc2->M(1,1) + pSrc1->M(3,2)*pSrc2->M(2,1) + pSrc1->M(3,3)*pSrc2->M(3,1);
    DstMatrix.M(3,2) = pSrc1->M(3,0)*pSrc2->M(0,2) + pSrc1->M(3,1)*pSrc2->M(1,2) + pSrc1->M(3,2)*pSrc2->M(2,2) + pSrc1->M(3,3)*pSrc2->M(3,2);
    DstMatrix.M(3,3) = pSrc1->M(3,0)*pSrc2->M(0,3) + pSrc1->M(3,1)*pSrc2->M(1,3) + pSrc1->M(3,2)*pSrc2->M(2,3) + pSrc1->M(3,3)*pSrc2->M(3,3);

    return DstMatrix;
}


//--------------------------------------------------------------------------------------
// Name: DET2X2()
// Desc: Helper function to compute the determinant of a 2x2 matrix
//--------------------------------------------------------------------------------------
__inline FLOAT32 DET2X2( FLOAT32 m00, FLOAT32 m01, 
                         FLOAT32 m10, FLOAT32 m11 )
{
    return m00 * m11 - m01 * m10;
}


//--------------------------------------------------------------------------------------
// Name: DET3X3()
// Desc: Helper function to compute the determinant of a 3x3 matrix
//--------------------------------------------------------------------------------------
__inline FLOAT32 DET3X3( FLOAT32 m00, FLOAT32 m01, FLOAT32 m02,
                         FLOAT32 m10, FLOAT32 m11, FLOAT32 m12,
                         FLOAT32 m20, FLOAT32 m21, FLOAT32 m22 )
{
    return m00 * DET2X2( m11, m12, m21, m22 ) -
           m10 * DET2X2( m01, m02, m21, m22 ) +
           m20 * DET2X2( m01, m02, m11, m12 );
}


//--------------------------------------------------------------------------------------
// Name: DET4X4()
// Desc: Helper function to compute the determinant of a 4X4 matrix
//--------------------------------------------------------------------------------------
__inline FLOAT32 DET4X4( FLOAT32 m00, FLOAT32 m01, FLOAT32 m02, FLOAT32 m03,
                         FLOAT32 m10, FLOAT32 m11, FLOAT32 m12, FLOAT32 m13,
                         FLOAT32 m20, FLOAT32 m21, FLOAT32 m22, FLOAT32 m23,
                         FLOAT32 m30, FLOAT32 m31, FLOAT32 m32, FLOAT32 m33 )
{
    return m00 * DET3X3( m11, m12, m13, m21, m22, m23, m31, m32, m33 ) -
           m10 * DET3X3( m01, m02, m03, m21, m22, m23, m31, m32, m33 ) +
           m20 * DET3X3( m01, m02, m03, m11, m12, m13, m31, m32, m33 ) -
           m30 * DET3X3( m01, m02, m03, m11, m12, m13, m21, m22, m23 );
}


//--------------------------------------------------------------------------------------
// Name: FrmMatrixNormalOrthonormal()
// Desc: Generates a matrix for transforming normals.
//--------------------------------------------------------------------------------------
FRMMATRIX3X3 FrmMatrixNormalOrthonormal( FRMMATRIX4X4& matSrcMatrix )
{
    // Note that we assume that the input matrix is orthonormal, so that we just need
    // to extract the 3x3 rotation core of the 4x4 source matrix

    FRMMATRIX3X3 matResult;

    matResult.M(0,0) = matSrcMatrix.M(0,0);
    matResult.M(0,1) = matSrcMatrix.M(0,1);
    matResult.M(0,2) = matSrcMatrix.M(0,2);
    
    matResult.M(1,0) = matSrcMatrix.M(1,0);
    matResult.M(1,1) = matSrcMatrix.M(1,1);
    matResult.M(1,2) = matSrcMatrix.M(1,2);
    
    matResult.M(2,0) = matSrcMatrix.M(2,0);
    matResult.M(2,1) = matSrcMatrix.M(2,1);
    matResult.M(2,2) = matSrcMatrix.M(2,2);

    return matResult;
}

//--------------------------------------------------------------------------------------
// Name: FrmMatrixNormal()
// Desc: Generates a matrix for transforming normals.
//--------------------------------------------------------------------------------------
FRMMATRIX3X3 FrmMatrixNormal( FRMMATRIX4X4& matSrcMatrix )
{
    // The generic case for the normal matrix is the transpose of the matrix inverse.
    // The matrix inverse, meanwhile, is the adjoint of the matrix scale by its
    // determinant

    FRMMATRIX3X3 DstMatrix;
    FRMMATRIX4X4* s = &matSrcMatrix;

    // Compute the matrix' determinant
    FLOAT32 fDeterminant = DET3X3( s->M(0,0), s->M(0,1), s->M(0,2), 
                                   s->M(1,0), s->M(1,1), s->M(1,2), 
                                   s->M(2,0), s->M(2,1), s->M(2,2) );
    FLOAT32 fScale = 1.0f / fDeterminant;

    // Divide the adjoint of the matrix by its determinant and transpose the result
    DstMatrix.M(0,0) = +DET2X2( s->M(1,1), s->M(1,2),  s->M(2,1), s->M(2,2) ) * fScale;
    DstMatrix.M(0,1) = -DET2X2( s->M(1,0), s->M(1,2),  s->M(2,0), s->M(2,2) ) * fScale;
    DstMatrix.M(0,2) = +DET2X2( s->M(1,0), s->M(1,1),  s->M(2,0), s->M(2,1) ) * fScale;
    
    DstMatrix.M(1,0) = -DET2X2( s->M(0,1), s->M(0,2),  s->M(2,1), s->M(2,2) ) * fScale;
    DstMatrix.M(1,1) = +DET2X2( s->M(0,0), s->M(0,2),  s->M(2,0), s->M(2,2) ) * fScale;
    DstMatrix.M(1,2) = -DET2X2( s->M(0,0), s->M(0,1),  s->M(2,0), s->M(2,1) ) * fScale;
    
    DstMatrix.M(2,0) = +DET2X2( s->M(0,1), s->M(0,2),  s->M(1,1), s->M(1,2) ) * fScale;
    DstMatrix.M(2,1) = -DET2X2( s->M(0,0), s->M(0,2),  s->M(1,0), s->M(1,2) ) * fScale;
    DstMatrix.M(2,2) = +DET2X2( s->M(0,0), s->M(0,1),  s->M(1,0), s->M(1,1) ) * fScale;
    
    return DstMatrix;
}


//--------------------------------------------------------------------------------------
// Name: FrmMatrixInverse()
// Desc: Computes the inverse of an orthonormal matrix
//--------------------------------------------------------------------------------------
FRMMATRIX4X4 FrmMatrixInverse( FRMMATRIX4X4& matSrcMatrix )
{
    FRMMATRIX4X4 DstMatrix;
    FRMMATRIX4X4* s = &matSrcMatrix;

    // Compute the matrix' determinant
    FLOAT32 fDeterminant = DET4X4( s->M(0,0), s->M(0,1), s->M(0,2), s->M(0,3),
                                   s->M(1,0), s->M(1,1), s->M(1,2), s->M(1,3),
                                   s->M(2,0), s->M(2,1), s->M(2,2), s->M(2,3),
                                   s->M(3,0), s->M(3,1), s->M(3,2), s->M(3,3) );
    FLOAT32 fScale = 1.0f / fDeterminant;

    // Divide the adjoint of the matrix by its determinant
    DstMatrix.M(0,0) = +DET3X3( s->M(1,1), s->M(1,2), s->M(1,3),   s->M(2,1), s->M(2,2), s->M(2,3),   s->M(3,1), s->M(3,2), s->M(3,3) ) * fScale;
    DstMatrix.M(0,1) = -DET3X3( s->M(1,0), s->M(1,2), s->M(1,3),   s->M(2,0), s->M(2,2), s->M(2,3),   s->M(3,0), s->M(3,2), s->M(3,3) ) * fScale;
    DstMatrix.M(0,2) = +DET3X3( s->M(1,0), s->M(1,1), s->M(1,3),   s->M(2,0), s->M(2,1), s->M(2,3),   s->M(3,0), s->M(3,1), s->M(3,3) ) * fScale;
    DstMatrix.M(0,3) = -DET3X3( s->M(1,0), s->M(1,1), s->M(1,2),   s->M(2,0), s->M(2,1), s->M(2,2),   s->M(3,0), s->M(3,1), s->M(3,2) ) * fScale;
    
    DstMatrix.M(1,0) = -DET3X3( s->M(0,1), s->M(0,2), s->M(0,3),   s->M(2,1), s->M(2,2), s->M(2,3),   s->M(3,1), s->M(3,2), s->M(3,3) ) * fScale;
    DstMatrix.M(1,1) = +DET3X3( s->M(0,0), s->M(0,2), s->M(0,3),   s->M(2,0), s->M(2,2), s->M(2,3),   s->M(3,0), s->M(3,2), s->M(3,3) ) * fScale;
    DstMatrix.M(1,2) = -DET3X3( s->M(0,0), s->M(0,1), s->M(0,3),   s->M(2,0), s->M(2,1), s->M(2,3),   s->M(3,0), s->M(3,1), s->M(3,3) ) * fScale;
    DstMatrix.M(1,3) = +DET3X3( s->M(0,0), s->M(0,1), s->M(0,2),   s->M(2,0), s->M(2,1), s->M(2,2),   s->M(3,0), s->M(3,1), s->M(3,2) ) * fScale;
    
    DstMatrix.M(2,0) = +DET3X3( s->M(0,1), s->M(0,2), s->M(0,3),   s->M(1,1), s->M(1,2), s->M(1,3),   s->M(3,1), s->M(3,2), s->M(3,3) ) * fScale;
    DstMatrix.M(2,1) = -DET3X3( s->M(0,0), s->M(0,2), s->M(0,3),   s->M(1,0), s->M(1,2), s->M(1,3),   s->M(3,0), s->M(3,2), s->M(3,3) ) * fScale;
    DstMatrix.M(2,2) = +DET3X3( s->M(0,0), s->M(0,1), s->M(0,3),   s->M(1,0), s->M(1,1), s->M(1,3),   s->M(3,0), s->M(3,1), s->M(3,3) ) * fScale;
    DstMatrix.M(2,3) = -DET3X3( s->M(0,0), s->M(0,1), s->M(0,2),   s->M(1,0), s->M(1,1), s->M(1,2),   s->M(3,0), s->M(3,1), s->M(3,2) ) * fScale;
    
    DstMatrix.M(3,0) = -DET3X3( s->M(0,1), s->M(0,2), s->M(0,3),   s->M(1,1), s->M(1,2), s->M(1,3),   s->M(2,1), s->M(2,2), s->M(2,3) ) * fScale;
    DstMatrix.M(3,1) = +DET3X3( s->M(0,0), s->M(0,2), s->M(0,3),   s->M(1,0), s->M(1,2), s->M(1,3),   s->M(2,0), s->M(2,2), s->M(2,3) ) * fScale;
    DstMatrix.M(3,2) = -DET3X3( s->M(0,0), s->M(0,1), s->M(0,3),   s->M(1,0), s->M(1,1), s->M(1,3),   s->M(2,0), s->M(2,1), s->M(2,3) ) * fScale;
    DstMatrix.M(3,3) = +DET3X3( s->M(0,0), s->M(0,1), s->M(0,2),   s->M(1,0), s->M(1,1), s->M(1,2),   s->M(2,0), s->M(2,1), s->M(2,2) ) * fScale;

    return DstMatrix;
}


//--------------------------------------------------------------------------------------
// Name: FrmMatrixInverseOrthonormal()
// Desc: Computes the inverse of an orthonormal matrix
//--------------------------------------------------------------------------------------
FRMMATRIX4X4 FrmMatrixInverseOrthonormal( FRMMATRIX4X4& matSrcMatrix )
{
    FRMMATRIX4X4 DstMatrix;
    FRMMATRIX4X4* s = &matSrcMatrix;

    // Transpose the 3x3 rotation submatrix
    DstMatrix.M(0,0) = s->M(0,0);  
    DstMatrix.M(0,1) = s->M(1,0);  
    DstMatrix.M(0,2) = s->M(2,0);
    DstMatrix.M(0,3) = 0.0f; 
    
    DstMatrix.M(1,0) = s->M(0,1);
    DstMatrix.M(1,1) = s->M(1,1);
    DstMatrix.M(1,2) = s->M(2,1);
    DstMatrix.M(1,3) = 0.0f; 
    
    DstMatrix.M(2,0) = s->M(0,2);
    DstMatrix.M(2,1) = s->M(1,2);
    DstMatrix.M(2,2) = s->M(2,2);
    DstMatrix.M(2,3) = 0.0f; 

    // Invert the matrix translation
    DstMatrix.M(0,0) = -s->M(3,0)*s->M(0,0) - s->M(3,1)*s->M(0,1) - s->M(3,2)*s->M(0,2);
    DstMatrix.M(1,1) = -s->M(3,0)*s->M(1,0) - s->M(3,1)*s->M(1,1) - s->M(3,2)*s->M(1,2);
    DstMatrix.M(2,2) = -s->M(3,0)*s->M(2,0) - s->M(3,1)*s->M(2,1) - s->M(3,2)*s->M(2,2);
    DstMatrix.M(3,3) = 1.0f;

    return DstMatrix;
}


//--------------------------------------------------------------------------------------
// Name: FrmMatrixTranspose()
// Desc: Return the transpose of a matrix
//--------------------------------------------------------------------------------------
FRMMATRIX4X4 FrmMatrixTranspose( FRMMATRIX4X4& matSrcMatrix )
{
    FRMMATRIX4X4 DstMatrix;

    DstMatrix.M(0,0) = matSrcMatrix.M(0,0);
    DstMatrix.M(0,1) = matSrcMatrix.M(1,0);
    DstMatrix.M(0,2) = matSrcMatrix.M(2,0);
    DstMatrix.M(0,3) = matSrcMatrix.M(3,0);

    DstMatrix.M(1,0) = matSrcMatrix.M(0,1);
    DstMatrix.M(1,1) = matSrcMatrix.M(1,1);
    DstMatrix.M(1,2) = matSrcMatrix.M(2,1);
    DstMatrix.M(1,3) = matSrcMatrix.M(3,1);

    DstMatrix.M(2,0) = matSrcMatrix.M(0,2);
    DstMatrix.M(2,1) = matSrcMatrix.M(1,2);
    DstMatrix.M(2,2) = matSrcMatrix.M(2,2);
    DstMatrix.M(2,3) = matSrcMatrix.M(3,2);

    DstMatrix.M(3,0) = matSrcMatrix.M(0,3);
    DstMatrix.M(3,1) = matSrcMatrix.M(1,3);
    DstMatrix.M(3,2) = matSrcMatrix.M(2,3);
    DstMatrix.M(3,3) = matSrcMatrix.M(3,3);

    return DstMatrix;
}


//--------------------------------------------------------------------------------------
// Name: FrmMatrixTranslate()
// Desc: Builds a translation matrix
//--------------------------------------------------------------------------------------
FRMMATRIX4X4 FrmMatrixTranslate( FRMVECTOR3& vVector )
{
    return FrmMatrixTranslate( vVector.x, vVector.y, vVector.z );
}

FRMMATRIX4X4 FrmMatrixTranslate( FLOAT32 tx, FLOAT32 ty, FLOAT32 tz )
{
    FRMMATRIX4X4 matResult;

    matResult.M(0,0) = 1.0f;
    matResult.M(0,1) = 0.0f;
    matResult.M(0,2) = 0.0f;
    matResult.M(0,3) = 0.0f;

    matResult.M(1,0) = 0.0f;
    matResult.M(1,1) = 1.0f;
    matResult.M(1,2) = 0.0f;
    matResult.M(1,3) = 0.0f;

    matResult.M(2,0) = 0.0f;
    matResult.M(2,1) = 0.0f;
    matResult.M(2,2) = 1.0f;
    matResult.M(2,3) = 0.0f;

    matResult.M(3,0) = tx;
    matResult.M(3,1) = ty;
    matResult.M(3,2) = tz;
    matResult.M(3,3) = 1.0f;

    return matResult;
}


//--------------------------------------------------------------------------------------
// Name: FrmMatrixScale()
// Desc: Builds a scaling matrix
//--------------------------------------------------------------------------------------
FRMMATRIX4X4 FrmMatrixScale( FRMVECTOR3& vVector )
{
    return FrmMatrixScale( vVector.x, vVector.y, vVector.z );
}

FRMMATRIX4X4 FrmMatrixScale( FLOAT32 sx, FLOAT32 sy, FLOAT32 sz )
{
    FRMMATRIX4X4 matResult;

    matResult.M(0,0) = sx;
    matResult.M(0,1) = 0.0f;
    matResult.M(0,2) = 0.0f;
    matResult.M(0,3) = 0.0f;

    matResult.M(1,0) = 0.0f;
    matResult.M(1,1) = sy;
    matResult.M(1,2) = 0.0f;
    matResult.M(1,3) = 0.0f;

    matResult.M(2,0) = 0.0f;
    matResult.M(2,1) = 0.0f;
    matResult.M(2,2) = sz;
    matResult.M(2,3) = 0.0f;

    matResult.M(3,0) = 0.0f;
    matResult.M(3,1) = 0.0f;
    matResult.M(3,2) = 0.0f;
    matResult.M(3,3) = 1.0f;

    return matResult;
}


//--------------------------------------------------------------------------------------
// Name: FrmMatrixRotate()
// Desc: Builds a rotation matrix
//--------------------------------------------------------------------------------------
FRMMATRIX4X4 FrmMatrixRotate( FLOAT32 fAngle, FRMVECTOR3& vAxis )
{
    return FrmMatrixRotate( fAngle, vAxis.x, vAxis.y, vAxis.z );
}

FRMMATRIX4X4 FrmMatrixRotate( FLOAT32 fAngle, FLOAT32 rx, FLOAT32 ry, FLOAT32 rz )
{
    FLOAT32 c = FrmCos( fAngle );
    FLOAT32 s = FrmSin( fAngle );
    FLOAT32 t = 1 - c;

    // Normalize the rotation axis
    FLOAT32 len = FrmSqrt( rx*rx + ry*ry + rz*rz );
    rx /= len;
    ry /= len;
    rz /= len;

    // Build the rotation matrix
    FRMMATRIX4X4 matResult;
    matResult.M(0,0) = t * rx * rx + c;
    matResult.M(1,0) = t * rx * ry + s * rz;
    matResult.M(2,0) = t * rx * rz - s * ry;
    matResult.M(3,0) = 0.0f;
    matResult.M(0,1) = t * ry * rx - s * rz;
    matResult.M(1,1) = t * ry * ry + c;
    matResult.M(2,1) = t * ry * rz + s * rx;
    matResult.M(3,1) = 0.0f;
    matResult.M(0,2) = t * rz * rx + s * ry;
    matResult.M(1,2) = t * rz * ry - s * rx;
    matResult.M(2,2) = t * rz * rz + c;
    matResult.M(3,2) = 0.0f;
    matResult.M(0,3) = 0.0f;
    matResult.M(1,3) = 0.0f;
    matResult.M(2,3) = 0.0f;
    matResult.M(3,3) = 1.0f;
    
    return matResult;
}

