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
#include "FFT1D.h"
#include "Compute.h"

//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmComputeApplication* FrmCreateComputeApplicationInstance()
{
    return new CSample( "FFT1D" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmComputeApplication( strName )
{
    m_commandQueue = 0;

    m_pFFT1D = NULL;
    m_pInputArray = NULL;
    m_pForwardRefArray = NULL;
    m_nArraySize = 4096; // Must be a power of 2

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

    // Initialize the FFT1D kernels
    m_pFFT1D = new CFFT1D(m_Timer);
    if ( FALSE == m_pFFT1D->Initialize( m_context, m_devices[0], m_commandQueue ) )
    {
        return FALSE;
    }

    // Create 1D waveform for testing
    m_pInputArray = new FFTComplex[ m_nArraySize ];
    for( INT32 x = 0; x < m_nArraySize; x++ )
    {
        FLOAT32 f;

        // Create a 1D waveform based on x,y.  This formula is arbitrary for testing...
        f = FrmSin( x * 0.5f ) + FrmCos( x * 1.5f ) + FrmSin( 2.0f * FrmCos( x * 10.f )  );
        m_pInputArray[x].fReal = f;
        m_pInputArray[x].fImaginary = f / 2.0f;
    }

    if( RunTests() )
    {
        m_pForwardRefArray = new FFTComplex[ m_nArraySize ];


        // Compute reference FFT result on CPU
        m_pFFT1D->Forward( m_pInputArray, m_nArraySize, m_pForwardRefArray, TRUE );


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
    delete m_pFFT1D;
}

//--------------------------------------------------------------------------------------
// Name: CompareToRef()
// Desc: Compare results from OpenCL to reference results
//--------------------------------------------------------------------------------------
BOOL CSample::CompareToRef( INT32 nArraySize, FFTComplex* pRefData, FFTComplex* pData,
                            FLOAT32 fEpsilon )
{
    BOOL result = TRUE;

    for( cl_int j = 0; j < nArraySize; j++ )
    {
        FFTComplex refVal = pRefData[j];
        FFTComplex val = pData[j];

        if( FrmAbs( refVal.fReal - val.fReal ) > fEpsilon ||
            FrmAbs( refVal.fImaginary - val.fImaginary ) > fEpsilon )
        {
            char str[256];
            FrmSprintf( str, sizeof(str), "Reference test failure, ref = (%f, %f), result = (%f, %f).\n",
                refVal.fReal, refVal.fImaginary, val.fReal, val.fImaginary);
            FrmLogMessage( str );
            result = FALSE;
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
    BOOL ret = false;
    FFTComplex* pOutArray = new FFTComplex[ m_nArraySize ];
    FFTComplex* pInvOutArray = new FFTComplex[ m_nArraySize ];

    FrmLogMessage( "Computing FFT with OpenCL ...\n" );
    m_Timer.Reset();
    // Compute forward FFT using OpenCL
    m_pFFT1D->Forward( m_pInputArray, m_nArraySize, pOutArray );

    FrmSprintf( str, sizeof(str), "Results: 1D FFT on '%d' elements in '%.6f' seconds.\n",
            m_nArraySize, m_Timer.GetTime() );
    FrmLogMessage( str );

    // Compare OpenCL result to reference result
    if( RunTests() )
    {
        BOOL result = CompareToRef( m_nArraySize, m_pForwardRefArray, pOutArray, 2.0f );
        if ( result == FALSE )
        {
            goto cleanup;
        }
    }

    FrmLogMessage( "Computing Inverse FFT with OpenCL ...\n" );
    m_Timer.Reset();
    // Compute inverse FFT using OpenCL - use the output of the FFT (pOutArray) as input
    m_pFFT1D->Inverse( pOutArray, m_nArraySize, pInvOutArray );

    FrmSprintf( str, sizeof(str), "Results: 1D IFFT on '%d' elements in '%.6f' seconds.\n",
            m_nArraySize, m_Timer.GetTime() );
    FrmLogMessage( str );

    // Compare OpenCL result to reference result - IFFT on FFT should yeild input data
    if( RunTests() )
    {
        BOOL result = CompareToRef( m_nArraySize, m_pInputArray, pInvOutArray, 2.0f );
        if ( result == FALSE )
        {
            goto cleanup;
        }
    }

    ret = TRUE;
cleanup:
    delete [] pOutArray;
    delete [] pInvOutArray;
    return TRUE;
}

