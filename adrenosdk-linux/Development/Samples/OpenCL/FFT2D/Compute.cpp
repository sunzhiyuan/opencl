//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013-2014 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include <FrmComputeApplication.h>
#include <FrmUtils.h>
#include <OpenCL/FrmKernel.h>
#include "FFT2D.h"
#include "Compute.h"

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmComputeApplication* FrmCreateComputeApplicationInstance()
{
    return new CSample( "FFT2D" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmComputeApplication( strName )
{
    m_commandQueue = 0;

    m_pFFT2D = NULL;
    m_pInputArray = NULL;
    m_pForwardRefArray = NULL;

    m_nWidth = 1024; // Must be a power of 2
    m_nHeight = 1024; // Must be a power of 2
}


//--------------------------------------------------------------------------------------
// Name: Initialize()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Initialize()
{
    cl_int errNum;

 if(!FrmOpenConsole())
  return FALSE;

    // Create the command queue
    m_commandQueue = clCreateCommandQueue( m_context, m_devices[0], CL_QUEUE_PROFILING_ENABLE, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Failed to create command queue" );
        return FALSE;
    }

    // Initialize the FFT2D kernels
    m_pFFT2D = new CFFT2D(m_Timer);
    if ( FALSE == m_pFFT2D->Initialize( m_context, m_devices[0], m_commandQueue ) )
    {
        return FALSE;
    }

    // Create 2D waveform for testing
    m_pInputArray = new FFTComplex[ m_nWidth * m_nHeight ];

    for( INT32 y = 0; y < m_nHeight; y++ )
    {
        for( INT32 x = 0; x < m_nWidth; x++ )
        {
            FLOAT32 f;

            // Create a 2D waveform based on x,y.  This formula is arbitrary for testing...
            f = ( FrmSin( x * 0.5f ) + FrmCos( y * 1.5f ) + FrmSin( 2.0f * FrmCos( ( x + y ) * 10.f )  ) ) / 1000.0f;
            m_pInputArray[y * m_nWidth + x].fReal = f;
            m_pInputArray[y * m_nWidth + x].fImaginary = f / 2.0f;
        }
    }

    if( RunTests() )
    {
        m_pForwardRefArray = new FFTComplex[ m_nWidth * m_nHeight ];


        // Compute reference FFT result on CPU
        m_pFFT2D->Forward( m_pInputArray, m_nWidth, m_nHeight, m_pForwardRefArray, TRUE );

    }

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if( m_commandQueue != 0 )
    {
        clReleaseCommandQueue( m_commandQueue );
        m_commandQueue = 0;
    }

    delete [] m_pInputArray;
    delete [] m_pForwardRefArray;
    delete m_pFFT2D;
}

//--------------------------------------------------------------------------------------
// Name: CompareToRef()
// Desc: Compare results from OpenCL to reference results
//--------------------------------------------------------------------------------------
BOOL CSample::CompareToRef( INT32 nWidth, INT32 nHeight, FFTComplex* pRefData, FFTComplex* pData,
                            FLOAT32 fEpsilon )
{
    BOOL result = TRUE;

    for( INT32 y = 0; y < nHeight; y++ )
    {
        for( INT32 x = 0; x < nWidth; x++ )
        {
            FFTComplex refVal = pRefData[y * m_nWidth + x];
            FFTComplex val = pData[y * m_nWidth + x];

            if( FrmAbs( refVal.fReal - val.fReal ) > fEpsilon ||
                FrmAbs( refVal.fImaginary - val.fImaginary ) > fEpsilon )
            {
                char str[256];
                FrmSprintf( str, sizeof(str), "Reference test failure,(%d, %d): ref = (%f, %f), result = (%f, %f).\n",
                    y, x, refVal.fReal, refVal.fImaginary, val.fReal, val.fImaginary);
                FrmLogMessage( str );
                result = FALSE;
            }
        }
    }

    return result;
}

//--------------------------------------------------------------------------------------
// Name: Compute()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Compute()
{
    char str[256];
    bool ret = FALSE;
    FFTComplex* pOutArray = new FFTComplex[ m_nWidth * m_nHeight ];

    FrmLogMessage( "Computing FFT with OpenCL ...\n" );
    m_Timer.Reset();
    // Compute forward FFT using OpenCL
    m_pFFT2D->Forward( m_pInputArray, m_nWidth, m_nHeight, pOutArray );

    FrmSprintf( str, sizeof(str), "Results: 2D FFT on '%d x %d' elements in '%.6f' seconds.\n",
            m_nWidth, m_nHeight, m_Timer.GetTime() );
    FrmLogMessage( str );

    // Compare OpenCL result to reference result
    if( RunTests() )
    {
        BOOL result = CompareToRef( m_nWidth, m_nHeight, m_pForwardRefArray, pOutArray, 0.5f );
        if ( result == FALSE )
        {
            goto cleanup;
        }
    }

    FrmLogMessage( "Computing Inverse FFT with OpenCL ...\n" );
    m_Timer.Reset();
    // Compute inverse FFT using OpenCL - use the output of the FFT (pOutArray) as input
    m_pFFT2D->Inverse( pOutArray, m_nWidth, m_nHeight, pOutArray );

    FrmSprintf( str, sizeof(str), "Results: 2D IFFT on '%d x %d' elements in '%.6f' seconds.\n",
            m_nWidth, m_nHeight, m_Timer.GetTime() );
    FrmLogMessage( str );

    // Compare OpenCL result to reference result - IFFT on FFT should yeild input data
    if( RunTests() )
    {
        BOOL result = CompareToRef( m_nWidth, m_nHeight, m_pInputArray, pOutArray, 0.5f );
        if ( result == FALSE )
        {
            goto cleanup;
        }
    }
    ret = TRUE;

cleanup:
    delete [] pOutArray;
    return ret;
}

