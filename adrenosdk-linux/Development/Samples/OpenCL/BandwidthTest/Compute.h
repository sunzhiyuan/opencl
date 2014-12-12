//--------------------------------------------------------------------------------------
// File: Compute.h
// Desc:
//
// Author:      QUALCOMM, Advanced Content Group - Snapdragon SDK
//
//               Copyright (c) 2009 QUALCOMM Incorporated.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#ifndef COMPUTE_H
#define COMPUTE_H

//--------------------------------------------------------------------------------------
// Name: class CSample
// Desc: The main application class for this sample
//--------------------------------------------------------------------------------------
class CSample : public CFrmComputeApplication
{

public:
    virtual BOOL Initialize();
    virtual VOID Destroy();
    virtual BOOL Compute();

    CSample( const CHAR* strName );
private:

    BOOL HostToDevice( size_t numBytes );
    BOOL DeviceToHost( size_t numBytes );
    BOOL DeviceToDevice( size_t numBytes );

    VOID ShowPerformance( const CHAR* label, FLOAT32 elapsedTime );


private:

    // Timer for timing performance
    CFrmTimer m_Timer;

    // Number of bytes to test
    size_t m_nNumBytes;

    // Number of times to test
    UINT32 m_nNumTests;

    // OpenCL command queue
    cl_command_queue m_commandQueue;
};


#endif // COMPUTE_H
