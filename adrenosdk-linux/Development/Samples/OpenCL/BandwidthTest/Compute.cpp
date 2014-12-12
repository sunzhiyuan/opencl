//--------------------------------------------------------------------------------------
// File: Compute.cpp
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2009 QUALCOMM Incorporated.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include <FrmComputeApplication.h>
#include <FrmUtils.h>
#include "Compute.h"


//--------------------------------------------------------------------------------------
// Name: FrmCreateApplicationInstance()
// Desc: Global function to create the application instance
//--------------------------------------------------------------------------------------
CFrmComputeApplication* FrmCreateComputeApplicationInstance()
{
    return new CSample( "BandwidthTest" );
}


//--------------------------------------------------------------------------------------
// Name: CSample()
// Desc: Constructor
//--------------------------------------------------------------------------------------
CSample::CSample( const CHAR* strName ) : CFrmComputeApplication( strName )
{
    m_commandQueue = 0;
    m_nNumBytes = (1 << 24); // 16MB
    m_nNumTests = 50;
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

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: HostToDevice()
// Desc: Test host-to-device copy performance
//--------------------------------------------------------------------------------------
BOOL CSample::HostToDevice( size_t numBytes )
{
    // Create host buffer
    BYTE* pHostBuf = new BYTE [ numBytes ];
    for ( size_t i = 0; i < numBytes; i++ )
    {
        pHostBuf[i] = (i & 0xff);
    }

    cl_int errNum = 0;
    cl_mem deviceBuffer;

    deviceBuffer = clCreateBuffer( m_context, CL_MEM_READ_WRITE, numBytes, NULL, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error allocating device buffer" );
        delete [] pHostBuf;
        return FALSE;
    }

    clFinish( m_commandQueue );

    m_Timer.Reset();
    m_Timer.Start();

    // Copy from host -> device
    for ( UINT32 i = 0; i < m_nNumTests; i++ )
    {
        errNum = clEnqueueWriteBuffer( m_commandQueue, deviceBuffer, CL_FALSE, 0, numBytes, pHostBuf, 0, NULL, NULL );
        if (errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error writing device buffer" );
            clReleaseMemObject( deviceBuffer );
            delete [] pHostBuf;
            return FALSE;
        }
    }

    // Finish any outstanding writes
    clFinish( m_commandQueue );
    FLOAT32 elapsedTime = m_Timer.Stop();

    ShowPerformance( "Results: Host -> Device", elapsedTime );
    delete [] pHostBuf;
    clReleaseMemObject( deviceBuffer );
    return TRUE;
}


//--------------------------------------------------------------------------------------
// Name: DeviceToHost()
// Desc: Test device-to-host copy performance
//--------------------------------------------------------------------------------------
BOOL CSample::DeviceToHost( size_t numBytes )
{
    // Create host buffer
    BYTE* pHostBuf = new BYTE [ numBytes ];
    for ( size_t i = 0; i < numBytes; i++ )
    {
        pHostBuf[i] = (i & 0xff);
    }

    cl_int errNum = 0;
    cl_mem deviceBuffer;

    // Copy the contents of the host buffer into a device buffer
    deviceBuffer = clCreateBuffer( m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, numBytes, pHostBuf, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error allocating device buffer" );
        delete [] pHostBuf;
        return FALSE;
    }

    clFinish( m_commandQueue );

    m_Timer.Reset();
    m_Timer.Start();

    // Copy from device -> host
    for ( UINT32 i = 0; i < m_nNumTests; i++ )
    {
        errNum = clEnqueueReadBuffer( m_commandQueue, deviceBuffer, CL_FALSE, 0, numBytes, pHostBuf, 0, NULL, NULL );
        if (errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error writing device buffer" );
            clReleaseMemObject( deviceBuffer );
            delete [] pHostBuf;
            return FALSE;
        }
    }

    // Finish any outstanding reads
    clFinish( m_commandQueue );
    FLOAT32 elapsedTime = m_Timer.Stop();

    ShowPerformance( "Results: Device -> Host", elapsedTime );
    delete [] pHostBuf;
    clReleaseMemObject( deviceBuffer );

    return TRUE;
}

//--------------------------------------------------------------------------------------
// Name: DeviceToDevice()
// Desc: Test device-to-device copy performance
//--------------------------------------------------------------------------------------
BOOL CSample::DeviceToDevice( size_t numBytes )
{
    // Create host buffer
    BYTE* pHostBuf = new BYTE [ numBytes ];
    for ( size_t i = 0; i < numBytes; i++ )
    {
        pHostBuf[i] = (i & 0xff);
    }

    cl_int errNum = 0;
    cl_mem deviceBufferSrc;

    // Copy the contents of the host buffer into a device buffer
    deviceBufferSrc = clCreateBuffer( m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, numBytes, pHostBuf, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error allocating source device buffer" );
        delete [] pHostBuf;
        return FALSE;
    }
    delete [] pHostBuf;

    // Create another device buffer to copy into
    cl_mem deviceBufferDst = 0;
    deviceBufferDst = clCreateBuffer( m_context, CL_MEM_READ_WRITE, numBytes, NULL, &errNum );
    if ( errNum != CL_SUCCESS )
    {
        FrmLogMessage( "Error allocating destination device buffer" );
        clReleaseMemObject( deviceBufferSrc );
        return FALSE;
    }

    clFinish( m_commandQueue );

    m_Timer.Reset();
    m_Timer.Start();

    // Copy from device -> host
    for ( UINT32 i = 0; i < m_nNumTests; i++ )
    {
        errNum = clEnqueueCopyBuffer( m_commandQueue, deviceBufferSrc, deviceBufferDst, 0, 0, numBytes, 0, NULL, NULL );
        if (errNum != CL_SUCCESS )
        {
            FrmLogMessage( "Error copying device buffer" );
            clReleaseMemObject( deviceBufferSrc );
            clReleaseMemObject( deviceBufferDst );
            return FALSE;
        }
    }

    // Finish any outstanding reads
    clFinish( m_commandQueue );
    FLOAT32 elapsedTime = m_Timer.Stop();

    ShowPerformance( "Results: Device -> Device", elapsedTime );
    clReleaseMemObject( deviceBufferSrc );
    clReleaseMemObject( deviceBufferDst );

    return TRUE;
}
//--------------------------------------------------------------------------------------
// Name: ShowPerformance()
// Desc: Output performance in MB/s
//--------------------------------------------------------------------------------------
VOID CSample::ShowPerformance( const CHAR* label, FLOAT32 elapsedTime)
{
    size_t dataSizeMB = (m_nNumTests * m_nNumBytes) / (1 << 20);
    FLOAT32 mbPerSec = ((FLOAT32)dataSizeMB) / elapsedTime;
    char outputStr[1024];
    FrmSprintf( outputStr, sizeof(outputStr),  "%s : %.1f MB/s\n", label, mbPerSec );
    FrmLogMessage( outputStr );
}




//--------------------------------------------------------------------------------------
// Name: Destroy()
// Desc:
//--------------------------------------------------------------------------------------
VOID CSample::Destroy()
{
    if ( m_commandQueue != 0 )
    {
        clReleaseCommandQueue( m_commandQueue );
        m_commandQueue = 0;
    }
}



//--------------------------------------------------------------------------------------
// Name: Compute()
// Desc:
//--------------------------------------------------------------------------------------
BOOL CSample::Compute()
{
    BOOL result = TRUE;

    // Test Host -> Device memory bandwidth
    result &= HostToDevice( m_nNumBytes );

    // Test Device -> Host memory bandwidth
    result &= DeviceToHost( m_nNumBytes );

    // Test Device -> Device memory bandwidth
    result &= DeviceToDevice( m_nNumBytes );

    return result;

}

