//--------------------------------------------------------------------------------------
// File: ClothSimCL.h
// Desc: Cloth Simulation using OpenCL
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#pragma once

#include "ClothSim.h"



//=============================================================================
// CClothSim
//=============================================================================
class CClothSimCL : public CClothSim
{
    // -----------------------
    // Constructors
    // -----------------------
public:
    CClothSimCL();
    virtual ~CClothSimCL();

    // -----------------------
    // Implementation
    // -----------------------
public:
    VOID SetUseOpenCL( bool bUseOpenCL );
    BOOL GetUseOpenCL() const;

    BOOL Initialize( cl_context context, cl_device_id device );
    void Destroy();
    void Tick(float fltElapsed);
    void UpdateVBOs();


protected:

    BOOL InitKernels( cl_context context, cl_device_id device );
    BOOL InitConstraintsBaseDists( cl_context context );
    BOOL SatisfyConstraintsCL();
    BOOL VerletIntegrationCL( float fltElapsed );
    BOOL UpdateNormalsCL();
    BOOL AcquireReleaseVBOs(bool bAcquire);

protected:

    typedef enum
    {
        SATISFY_CONSTRAINTS,
        VERLET_INTEGRATION,
        UPDATE_NORMALS,
        NUM_KERNELS
    } Kernels;

    typedef enum
    {
        CUR_POSITION,
        PREV_POSITION,
        NORMAL,
        TANGENT,
        BITANGENT,
        TEXTURE,
        BACKNORMAL,
        BACKTANGENT,
        BACKBITANGENT,
        NUM_VBOS
    } ClothVBOs;

    // VBO for previous position
    GLuint m_hPrevPositionVBO;

     // OpenCL command queue
    cl_command_queue m_commandQueue;

    // OpenCL program
    cl_program m_program;

    // OpenCL kernel
    cl_kernel m_kernels[NUM_KERNELS];

    // OpenCL memory objects pointing to VBOs
    cl_mem m_vboMem[NUM_VBOS];

    // OpenCL memory object to hold base distances
    cl_mem m_baseDistMem;

    // OpenCL memory object to hold neighbor indices
    cl_mem m_baseDistNeighborIndexMem;

    // OpenCL Memory object to hold start base distance indices per vert
    cl_mem m_baseDistStartIndexMem;

    // OpenCL Memory object to hold start base distance indices per vert
    cl_mem m_baseDistEndIndexMem;

    // OpenCL memory object to hold copy of positions for
    // SatisfyConstraints()
    cl_mem m_vertsCopyMem;

    // Whether to use OpenCL or CPU simulation
    bool m_bUseOpenCL;
};
