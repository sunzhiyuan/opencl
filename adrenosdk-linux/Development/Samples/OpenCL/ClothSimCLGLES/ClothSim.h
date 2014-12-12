//--------------------------------------------------------------------------------------
// File: ClothSim.h
// Desc: Cloth Simulation
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#pragma once

// Forward declaration of myself
class CClothSim;

//=============================================================================
// TYPEDEFS
//=============================================================================
struct Constraint
{
    UINT32          uiVertA;
    UINT32          uiVertB;
    FLOAT32         BaseDist;
};

struct ClothThreadParam
{
    FLOAT32         fltElapsed;
    CClothSim *     pClothSim;
};

struct SphereInfo
{
    BOOL            Active;
    BOOL            ShouldKill;
    FRMVECTOR4      vCenter;
    FRMVECTOR4      vVelocity;
    FLOAT32         Radius;
};



//=============================================================================
// CONSTANTS
//=============================================================================
// NOTE: must be power-of-2 for OpenCL simulation and also need to update
// these in ClothSimCLGLES.cl
#define CLOTH_POINTS_WIDTH      16
#define CLOTH_POINTS_HEIGHT     16

#define MAX_COLLISION_SPHERES   10

//=============================================================================
// CClothSim
//=============================================================================
class CClothSim
{
    // -----------------------
    // Constructors
    // -----------------------
public:
    CClothSim();
    virtual ~CClothSim();

    // -----------------------
    // Implementation
    // -----------------------
public:
    BOOL    Initialize();
    void    Reset();
    void    Destroy();

    void    AddWindPulse(float Strength, float *pDirection, float Duration);

    void    LaunchSphere(float ScreenPosX, float ScreenPosY, FRMMATRIX4X4 &matVp, FRMVECTOR3 &CameraPos);

    void    SubmitDraw( );

    void    Tick(float fltElapsed);
    void    SatisfyConstraints();
    void    UpdateNormals();
    void    UpdateVBOs();

protected:

    void    AddConstraint(UINT32 uiVertA, UINT32 uiVertB);

    BOOL  FindImpactPoint(float ScreenPosX, float ScreenPosY, FRMMATRIX4X4 &matVp, FRMVECTOR3 &ImpactPoint);

    void    AnimateSpheres(float fltElapsed);


    // -----------------------
    // Attributes
    // -----------------------

    float           m_fltSystemTime;

    float           m_TickTime;

    // Location
    FRMVECTOR3         m_MinValues;
    FRMVECTOR3         m_MaxValues;
    FRMVECTOR3         m_Center;
    FRMVECTOR3         m_BaseRadius;

    // The simulation
    UINT32          m_uiNumVerts;
    float *         m_pVerts;
    float *         m_pOldVerts;
    float *         m_pNorms;
    float *         m_pTangents;
    float *         m_pBitangents;
    float *         m_pTexCoords;

    float *         m_pBackNorms;
    float *         m_pBackTangents;
    float *         m_pBackBitangents;

    UINT32          m_uiNumIndicies;
    UINT16 *        m_pIndicies;

    UINT32          m_ConstraintIters;
    UINT32          m_uiNumConstraints;
    UINT32          m_uiAllocedConstraints;
    Constraint *    m_pConstraints;

    // VBO's
    UINT32          m_hPositionVBO;
    UINT32          m_hNormalVBO;
    UINT32          m_hTangentVBO;
    UINT32          m_hBitangentVBO;
    UINT32          m_hTextureVBO;
    UINT32          m_hIndexVBO;

    UINT32          m_hBackNormalVBO;
    UINT32          m_hBackTangentVBO;
    UINT32          m_hBackBitangentVBO;

    // Forces
    FRMVECTOR3 m_VelDamping;
    FRMVECTOR3 m_Gravity;
    FRMVECTOR3 m_CurrentWind;
    FRMVECTOR3 m_WindDir;

    // Wind Pulse
    float       m_PulseStartTime;
    float       m_PulseStrength;
    float       m_PulseDuration;
    FRMVECTOR3  m_PulseDirection;


    // Sphere information
    SphereInfo  m_SphereInfo[MAX_COLLISION_SPHERES];
};
