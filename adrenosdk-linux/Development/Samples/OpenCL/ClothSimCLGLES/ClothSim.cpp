//--------------------------------------------------------------------------------------
// File: ClothSim.cpp
// Desc: Cloth Simulation
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved.
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------
#include <FrmPlatform.h>
#include <FrmUtils.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <CL/cl_gl.h>
#include <OpenGLES/FrmResourceGLES.h>
#include "ClothSim.h"

#define loadXYZ(a, b)  { (a).x = (b)[0]; (a).y = (b)[1]; (a).z = (b)[2]; }
#define loadXYZW(a, b) { (a).x = (b)[0]; (a).y = (b)[1]; (a).z = (b)[2]; (a).w = (b)[3]; }

//=============================================================================
// GLOBALS
//=============================================================================

//-----------------------------------------------------------------------------
CClothSim::CClothSim()
//-----------------------------------------------------------------------------
{
    m_fltSystemTime = 0.0f;
    m_TickTime = 0.0f;

    // Location
    m_MinValues = FRMVECTOR3(-1.0f, -1.0f, -1.0f);
    m_MaxValues = FRMVECTOR3( 1.0f,  1.0f,  1.0f);
    m_Center = FRMVECTOR3(0.0f, 1.0f, 0.0f);
    m_BaseRadius = FRMVECTOR3(0.5f, 0.5f, 0.0f);

    // The simulation
    m_uiNumVerts = 0;
    m_pVerts = NULL;
    m_pOldVerts = NULL;
    m_pNorms = NULL;
    m_pTangents = NULL;
    m_pBitangents = NULL;
    m_pTexCoords = NULL;

    m_pBackNorms = NULL;
    m_pBackTangents = NULL;
    m_pBackBitangents = NULL;

    m_uiNumIndicies = 0;
    m_pIndicies = NULL;

    m_ConstraintIters = 18;
    m_uiNumConstraints = 0;
    m_uiAllocedConstraints = 0;
    m_pConstraints = NULL;

    // VBO's
    m_hPositionVBO = 0;
    m_hNormalVBO = 0;
    m_hTangentVBO = 0;
    m_hTextureVBO = 0;
    m_hIndexVBO = 0;

    m_hBackNormalVBO = 0;
    m_hBackTangentVBO = 0;

    // Forces
    m_VelDamping = FRMVECTOR3(0.99f, 0.99f, 0.99f);
    m_Gravity = FRMVECTOR3(0.0f, -9.8f, 0.0f);
    m_CurrentWind = FRMVECTOR3(0.5f, 0.0f, -0.5f);
    m_WindDir = FRMVECTOR3(0.0f, 0.0f, 1.0f);

    // Wind Pulse
    m_PulseStartTime = 0.0f;
    m_PulseStrength = 0.0f;
    m_PulseDuration = 0.0f;
    m_PulseDirection = FRMVECTOR3(0.0f, 0.0f, 0.0f);

    // Sphere information
    for(UINT32 uiIndx = 0; uiIndx < MAX_COLLISION_SPHERES; uiIndx++)
    {
        m_SphereInfo[uiIndx].Active = false;
        m_SphereInfo[uiIndx].ShouldKill = false;
        m_SphereInfo[uiIndx].vCenter = FRMVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
        m_SphereInfo[uiIndx].vVelocity = FRMVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
        m_SphereInfo[uiIndx].Radius = 0.0f;
    }
}

//-----------------------------------------------------------------------------
CClothSim::~CClothSim()
//-----------------------------------------------------------------------------
{
    Destroy();
}


//-----------------------------------------------------------------------------
BOOL CClothSim::Initialize()
//-----------------------------------------------------------------------------
{
    // Adjust the DeltaY since stretching the bottom a little
    float DeltaYAdjust = 0.9f;
    float DeltaX = (m_MaxValues.v[0] - m_MinValues.v[0]) / (float)(CLOTH_POINTS_WIDTH - 1);
    float DeltaY = DeltaYAdjust * (m_MaxValues.v[1] - m_MinValues.v[1]) / (float)(CLOTH_POINTS_HEIGHT - 1);
    float DeltaZ = (m_MaxValues.v[2] - m_MinValues.v[2]) / (float)(CLOTH_POINTS_WIDTH - 1);

    float TexStartY = 0.2f;
    float TexRangeY = 0.8f;
    float TexDeltaX = 1.0f / (float)(CLOTH_POINTS_WIDTH - 1);
    float TexDeltaY = TexRangeY / (float)(CLOTH_POINTS_HEIGHT - 1);

    m_uiNumVerts = CLOTH_POINTS_WIDTH * CLOTH_POINTS_HEIGHT;
    m_pVerts = (float *)malloc(m_uiNumVerts * 4 * sizeof(float));
    m_pOldVerts = (float *)malloc(m_uiNumVerts * 4 * sizeof(float));
    m_pNorms = (float *)malloc(m_uiNumVerts * 4 * sizeof(float));
    m_pTangents = (float *)malloc(m_uiNumVerts * 4 * sizeof(float));
    m_pBitangents = (float *)malloc(m_uiNumVerts * 4 * sizeof(float));
    m_pTexCoords = (float *)malloc(m_uiNumVerts * 2 * sizeof(float));

    m_pBackNorms = (float *)malloc(m_uiNumVerts * 4 * sizeof(float));
    m_pBackTangents = (float *)malloc(m_uiNumVerts * 4 * sizeof(float));
    m_pBackBitangents = (float *)malloc(m_uiNumVerts * 4 * sizeof(float));

    if( m_pVerts == NULL ||
        m_pOldVerts == NULL ||
        m_pNorms == NULL ||
        m_pTangents == NULL ||
        m_pBitangents == NULL ||
        m_pTexCoords == NULL ||
        m_pBackNorms == NULL ||
        m_pBackTangents == NULL ||
        m_pBackBitangents == NULL)
    {
        // m_uiNumClothSystems has not been bumped so we can just leave
        FrmLogMessage("Unable to allocate memory for Cloth mesh!");
        return false;
    }

    memset(m_pVerts, 0, m_uiNumVerts * 4 * sizeof(float));
    memset(m_pOldVerts, 0, m_uiNumVerts * 4 * sizeof(float));
    memset(m_pNorms, 0, m_uiNumVerts * 4 * sizeof(float));
    memset(m_pTangents, 0, m_uiNumVerts * 4 * sizeof(float));
    memset(m_pBitangents, 0, m_uiNumVerts * 4 * sizeof(float));
    memset(m_pTexCoords, 0, m_uiNumVerts * 2 * sizeof(float));

    memset(m_pBackNorms, 0, m_uiNumVerts * 4 * sizeof(float));
    memset(m_pBackTangents, 0, m_uiNumVerts * 4 * sizeof(float));
    memset(m_pBackBitangents, 0, m_uiNumVerts * 4 * sizeof(float));

    // Build up the vertices...
    for (UINT32 uiY = 0; uiY < CLOTH_POINTS_HEIGHT; uiY++)
    {
        // Look at the first 20% of the mesh and push it down a little more to
        // get a deeper curve than on top.
        float YLerp = (float)uiY / (float)CLOTH_POINTS_HEIGHT;
        YLerp /= 0.2f;
        if(YLerp > 1.0f)
            YLerp = 1.0f;
        YLerp = 1.0f - YLerp;

        for (UINT32 uiX = 0; uiX < CLOTH_POINTS_WIDTH; uiX++)
        {

            UINT32 WhichVertex = uiX + uiY * CLOTH_POINTS_WIDTH;

            // Go from 0 to Pi
            // float SLerp = ((float)uiX - (float)CLOTH_POINTS_WIDTH / 2.0f) / (float)CLOTH_POINTS_WIDTH;
            float SLerp = (float)uiX / (float)CLOTH_POINTS_WIDTH;
            SLerp = 0.05f * sinf(SLerp * 3.14159f);

            m_pVerts[WhichVertex * 4 + 0] = m_MinValues.v[0] + (float)uiX * DeltaX;
            m_pVerts[WhichVertex * 4 + 1] = m_MinValues.v[1] + (float)uiY * DeltaY - SLerp - (3.0f * YLerp * SLerp);
            m_pVerts[WhichVertex * 4 + 2] = m_MinValues.v[2] + (float)uiX * DeltaZ;
            m_pVerts[WhichVertex * 4 + 3] = 1.0f;

            // To account for the slightly smaller values
            m_pVerts[WhichVertex * 4 + 1] += (1.0f - DeltaYAdjust) * (m_MaxValues.v[1] - m_MinValues.v[1]);

            m_pOldVerts[WhichVertex * 4 + 0] = m_pVerts[WhichVertex * 4 + 0];
            m_pOldVerts[WhichVertex * 4 + 1] = m_pVerts[WhichVertex * 4 + 1];
            m_pOldVerts[WhichVertex * 4 + 2] = m_pVerts[WhichVertex * 4 + 2];
            m_pOldVerts[WhichVertex * 4 + 3] = m_pVerts[WhichVertex * 4 + 3];

#ifdef SQUARE_CLOTH
            m_pVerts[WhichVertex * 4 + 0] = MinX + (float)uiX * DeltaX;
            m_pVerts[WhichVertex * 4 + 1] = MinY + (float)uiY * DeltaY;
            m_pVerts[WhichVertex * 4 + 2] = Center[2];
            m_pVerts[WhichVertex * 4 + 3] = 1.0f;

            m_pOldVerts[WhichVertex * 4 + 0] = MinX + (float)uiX * DeltaX;
            m_pOldVerts[WhichVertex * 4 + 1] = MinY + (float)uiY * DeltaY;
            m_pOldVerts[WhichVertex * 4 + 2] = Center[2];
            m_pOldVerts[WhichVertex * 4 + 3] = 1.0f;
#endif // SQUARE_CLOTH

            m_pNorms[WhichVertex * 4 + 0] = 0.0f;
            m_pNorms[WhichVertex * 4 + 1] = 0.0f;
            m_pNorms[WhichVertex * 4 + 2] = 1.0f;

            m_pTangents[WhichVertex * 4 + 0] = 1.0f;
            m_pTangents[WhichVertex * 4 + 1] = 0.0f;
            m_pTangents[WhichVertex * 4 + 2] = 0.0f;

            m_pBitangents[WhichVertex * 4 + 0] = -1.0f;
            m_pBitangents[WhichVertex * 4 + 1] = 0.0f;
            m_pBitangents[WhichVertex * 4 + 2] = 0.0f;

            m_pBackNorms[WhichVertex * 4 + 0] = 0.0f;
            m_pBackNorms[WhichVertex * 4 + 1] = 0.0f;
            m_pBackNorms[WhichVertex * 4 + 2] = -1.0f;

            m_pBackTangents[WhichVertex * 4 + 0] = -1.0f;
            m_pBackTangents[WhichVertex * 4 + 1] = 0.0f;
            m_pBackTangents[WhichVertex * 4 + 2] = 0.0f;

            m_pBackBitangents[WhichVertex * 4 + 0] = -1.0f;
            m_pBackBitangents[WhichVertex * 4 + 1] = 0.0f;
            m_pBackBitangents[WhichVertex * 4 + 2] = 0.0f;

            m_pTexCoords[WhichVertex * 2 + 0] = (float)uiX * TexDeltaX;
            // m_pTexCoords[WhichVertex * 2 + 1] = TexStartY + (float)uiY * TexDeltaY;

            // Since we are pushing some of the bottom verts down, adjust the UV so it doesn't stretch
            m_pTexCoords[WhichVertex * 2 + 1] = TexStartY + TexRangeY * (m_pVerts[WhichVertex * 4 + 1] - m_MinValues.v[1]) / (m_MaxValues.v[1] - m_MinValues.v[1]) - 0.1f;
        }
    }

#ifdef FOLDING_IN
    // We want the bottom of the cloth to look like a parabola
    float KonstA = 1.0f;    // How flat is the parabola?
    FRMVECTOR3 FlatCenter = FRMVECTOR3(m_Center[0], 0.0f, m_Center[2]);
    for (UINT32 uiY = 0; uiY < CLOTH_POINTS_HEIGHT; uiY++)
    {
        float MaxX = (float)uiY / (float)CLOTH_POINTS_HEIGHT;
        MaxX /= KonstA;
        MaxX = sqrt(MaxX);
        for (UINT32 uiX = 0; uiX < CLOTH_POINTS_WIDTH; uiX++)
        {

            UINT32 WhichVertex = uiX + uiY * CLOTH_POINTS_WIDTH;

            FRMVECTOR3 OneVector = FRMVECTOR3(m_pVerts[WhichVertex * 4 + 0], 0.0f, m_pVerts[WhichVertex * 4 + 2]);
            FRMVECTOR3 ToCenter = OneVector - FlatCenter;
            float Dist = length(ToCenter);
            ToCenter = normalize(ToCenter);

            if(Dist > MaxX)
            {
                OneVector = FlatCenter + MaxX * ToCenter;
                m_pVerts[WhichVertex * 4 + 0] = OneVector[0];
                m_pVerts[WhichVertex * 4 + 2] = OneVector[2];

                m_pOldVerts[WhichVertex * 4 + 0] = m_pVerts[WhichVertex * 4 + 0];
                m_pOldVerts[WhichVertex * 4 + 2] = m_pVerts[WhichVertex * 4 + 2];

                float NewU = abs(m_pVerts[WhichVertex * 4 + 0] - FlatCenter[0]) / (m_MaxValues[0] - m_MinValues[0]);
                if(ToCenter[0] < 0.0f)
                    m_pTexCoords[WhichVertex * 2 + 0] = 0.5f - NewU;
                else
                    m_pTexCoords[WhichVertex * 2 + 0] = 0.5f + NewU;

            }
        }
    }
#endif // FOLDING_IN



    // ... and then the indicies
    m_uiNumIndicies = CLOTH_POINTS_WIDTH * 2 * (CLOTH_POINTS_HEIGHT - 1) + 2 * (CLOTH_POINTS_HEIGHT - 2);
    m_pIndicies = (UINT16 *)malloc(m_uiNumIndicies * sizeof(UINT16));
    if(m_pIndicies == NULL)
    {
        FrmLogMessage("Unable to allocate memory for Cloth mesh indicies!");
        return false;
    }

    UINT16* pData = m_pIndicies;
    for (UINT32 uiY = 0; uiY < CLOTH_POINTS_HEIGHT - 1; uiY++)
    {
        //find the beginning index of the strip (note the extra 2 indices per strip for degenerate triangles)
        UINT32 nStripStart = CLOTH_POINTS_WIDTH * 2 * uiY + 2 * uiY;

        if (uiY > 0)
        {
            //add two degenerate indices at the end of the last strip
            pData[nStripStart - 2] = (CLOTH_POINTS_WIDTH - 1) + (uiY    )*(CLOTH_POINTS_WIDTH); //last index from previous strip
            pData[nStripStart - 1] = 0                        + (uiY    )*(CLOTH_POINTS_WIDTH); //first index from this strip
        }

        for (UINT32 uiX = 0; uiX < CLOTH_POINTS_WIDTH; uiX++)
        {
            pData[nStripStart + uiX * 2 + 0] = uiX + (uiY    )*(CLOTH_POINTS_WIDTH);
            pData[nStripStart + uiX * 2 + 1] = uiX + (uiY + 1)*(CLOTH_POINTS_WIDTH);
            UINT32 uiDebug = 5150;
        }
    }


    // Build the constraints
    // 1. Structural: Each vert to it's neighbors
    for (UINT32 uiY = 0; uiY < CLOTH_POINTS_HEIGHT; uiY++)
    {
        for (UINT32 uiX = 0; uiX < CLOTH_POINTS_WIDTH; uiX++)
        {
            UINT32 uiVertA = uiX + uiY * CLOTH_POINTS_WIDTH;

            // Right Neighbor
            if(uiX < CLOTH_POINTS_WIDTH - 1)
            {
                UINT32 uiVertB = uiVertA + 1;
                AddConstraint(uiVertA, uiVertB);
            }


            // Top Neighbor
            if(uiY < CLOTH_POINTS_HEIGHT - 1)
            {
                UINT32 uiVertB = uiVertA + CLOTH_POINTS_WIDTH;
                AddConstraint(uiVertA, uiVertB);
            }
        }
    }

    // 2. Shear: Each vert to it's diagonal neighbors
    for (UINT32 uiY = 0; uiY < CLOTH_POINTS_HEIGHT; uiY++)
    {
        for (UINT32 uiX = 0; uiX < CLOTH_POINTS_WIDTH; uiX++)
        {
            UINT32 uiVertA = uiX + uiY * CLOTH_POINTS_WIDTH;

            // Up Left Neighbor
            if(uiX > 0 && uiY < CLOTH_POINTS_HEIGHT - 1)
            {
                UINT32 uiVertB = uiVertA + CLOTH_POINTS_WIDTH - 1;
                AddConstraint(uiVertA, uiVertB);
            }


            // Up Right Neighbor
            if(uiX < CLOTH_POINTS_WIDTH - 1 && uiY < CLOTH_POINTS_HEIGHT - 1)
            {
                UINT32 uiVertB = uiVertA + CLOTH_POINTS_WIDTH + 1;
                AddConstraint(uiVertA, uiVertB);
            }
        }
    }

    // 3. Bending: Each vert to it's second (Right, Up, Diagonal) neighbors
    // NOTE:Skipping every other one! "uiY += 2" vs "uiY++"
    for (UINT32 uiY = 0; uiY < CLOTH_POINTS_HEIGHT; uiY += 2)
    {
        for (UINT32 uiX = 0; uiX < CLOTH_POINTS_WIDTH; uiX += 2)
        {
            UINT32 uiVertA = uiX + uiY * CLOTH_POINTS_WIDTH;

            // Right Neighbor
            if(uiX < CLOTH_POINTS_WIDTH - 2)
            {
                UINT32 uiVertB = uiVertA + 2;
                AddConstraint(uiVertA, uiVertB);
            }


            // Top Neighbor
            if(uiY < CLOTH_POINTS_HEIGHT - 2)
            {
                UINT32 uiVertB = uiVertA + CLOTH_POINTS_WIDTH + CLOTH_POINTS_WIDTH;
                AddConstraint(uiVertA, uiVertB);
            }

            // Up Left Neighbor
            if(uiX > 1 && uiY < CLOTH_POINTS_HEIGHT - 2)
            {
                UINT32 uiVertB = uiVertA + 2 * CLOTH_POINTS_WIDTH - 2;
                AddConstraint(uiVertA, uiVertB);
            }


            // Up Right Neighbor
            if(uiX < CLOTH_POINTS_WIDTH - 2 && uiY < CLOTH_POINTS_HEIGHT - 2)
            {
                UINT32 uiVertB = uiVertA + 2 * CLOTH_POINTS_WIDTH + 2;
                AddConstraint(uiVertA, uiVertB);
            }
        }
    }

    // Create the VBO's
    glGenBuffers( 1, &m_hPositionVBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_hPositionVBO );
    glBufferData( GL_ARRAY_BUFFER, m_uiNumVerts * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pVerts );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glGenBuffers( 1, &m_hNormalVBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_hNormalVBO );
    glBufferData( GL_ARRAY_BUFFER, m_uiNumVerts * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pNorms );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glGenBuffers( 1, &m_hTangentVBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_hTangentVBO );
    glBufferData( GL_ARRAY_BUFFER, m_uiNumVerts * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pTangents );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glGenBuffers( 1, &m_hBitangentVBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_hBitangentVBO );
    glBufferData( GL_ARRAY_BUFFER, m_uiNumVerts * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pBitangents );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glGenBuffers( 1, &m_hBackNormalVBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_hBackNormalVBO );
    glBufferData( GL_ARRAY_BUFFER, m_uiNumVerts * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pBackNorms );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glGenBuffers( 1, &m_hBackTangentVBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_hBackTangentVBO );
    glBufferData( GL_ARRAY_BUFFER, m_uiNumVerts * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pBackTangents );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glGenBuffers( 1, &m_hBackBitangentVBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_hBackBitangentVBO );
    glBufferData( GL_ARRAY_BUFFER, m_uiNumVerts * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pBackBitangents );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glGenBuffers( 1, &m_hTextureVBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_hTextureVBO );
    glBufferData( GL_ARRAY_BUFFER, m_uiNumVerts * 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 2 * sizeof(float), m_pTexCoords );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glGenBuffers( 1, &m_hIndexVBO );
    glBindBuffer( GL_ARRAY_BUFFER, m_hIndexVBO );
    glBufferData( GL_ARRAY_BUFFER, m_uiNumIndicies * sizeof(UINT16), NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumIndicies * sizeof(UINT16), m_pIndicies );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    return true;
}

//-----------------------------------------------------------------------------
void CClothSim::AddConstraint(UINT32 uiVertA, UINT32 uiVertB)
//-----------------------------------------------------------------------------
{
    Constraint NewConstraint;

    NewConstraint.uiVertA = uiVertA;
    NewConstraint.uiVertB = uiVertB;

    FRMVECTOR3 vecA; loadXYZ(vecA, &m_pVerts[uiVertA * 4]);
    FRMVECTOR3 vecB; loadXYZ(vecB, &m_pVerts[uiVertB * 4]);
    FRMVECTOR3 vecDiff = vecB - vecA;

    NewConstraint.BaseDist = FrmVector3Length( vecDiff );

    if((m_uiNumConstraints + 1) < m_uiAllocedConstraints)
    {
        // Already room in the allocated list, add this one on to the end
        memcpy(&m_pConstraints[m_uiNumConstraints], &NewConstraint, sizeof(Constraint));
        m_uiNumConstraints++;
        return;
    }

    // Need to expand the list
    UINT32 uiNumToAlloc = 5000;

    // Now that the Constraint is filled out, add to the list.
    UINT32 uiSize = (m_uiAllocedConstraints + uiNumToAlloc) * sizeof(Constraint);
    Constraint *pNewList = (Constraint *)malloc(uiSize);
    if(pNewList == NULL)
    {
        FrmLogMessage("Error allocating memory for list of Constraints");
        return;
    }

    m_uiAllocedConstraints += uiNumToAlloc;

    memset(pNewList, 0, m_uiAllocedConstraints * sizeof(Constraint));

    // Copy over the existing list
    if (m_uiNumConstraints > 0)
    {
        memcpy(pNewList, m_pConstraints, m_uiNumConstraints * sizeof(Constraint));
    }

    // Put the new one on the end
    memcpy(&pNewList[m_uiNumConstraints], &NewConstraint, sizeof(Constraint));

    // Release the old list and point to new list
    if(m_pConstraints)
        free(m_pConstraints);

    m_pConstraints = pNewList;

    // Bump the Constraint count
    m_uiNumConstraints++;
}

//-----------------------------------------------------------------------------
void CClothSim::Reset()
//-----------------------------------------------------------------------------
{
    FrmLogMessage("Resetting CClothSim");

}

//-----------------------------------------------------------------------------
void CClothSim::Destroy()
//-----------------------------------------------------------------------------
{
    m_uiNumVerts = 0;
    m_uiNumIndicies = 0;

    if(m_pVerts != NULL)
    {
        free(m_pVerts);
        m_pVerts = NULL;
    }

    if(m_pOldVerts != NULL)
    {
        free(m_pOldVerts);
        m_pOldVerts = NULL;
    }

    if(m_pNorms != NULL)
    {
        free(m_pNorms);
        m_pNorms = NULL;
    }

    if(m_pTangents != NULL)
    {
        free(m_pTangents);
        m_pTangents = NULL;
    }

    if(m_pBitangents != NULL)
    {
        free(m_pBitangents);
        m_pBitangents = NULL;
    }

    if(m_pBackNorms != NULL)
    {
        free(m_pBackNorms);
        m_pBackNorms = NULL;
    }

    if(m_pBackTangents != NULL)
    {
        free(m_pBackTangents);
        m_pBackTangents = NULL;
    }

    if(m_pBackBitangents != NULL)
    {
        free(m_pBackBitangents);
        m_pBackBitangents = NULL;
    }

    if(m_pTexCoords != NULL)
    {
        free(m_pTexCoords);
        m_pTexCoords = NULL;
    }

    if(m_pIndicies != NULL)
    {
        free(m_pIndicies);
        m_pIndicies = NULL;
    }

    if(m_hPositionVBO)
    {
        glDeleteBuffers( 1, &m_hPositionVBO );
        m_hPositionVBO = 0;
    }

    if(m_hNormalVBO)
    {
        glDeleteBuffers( 1, &m_hNormalVBO );
        m_hNormalVBO = 0;
    }

    if(m_hTangentVBO)
    {
        glDeleteBuffers( 1, &m_hTangentVBO );
        m_hTangentVBO = 0;
    }

    if(m_hBackNormalVBO)
    {
        glDeleteBuffers( 1, &m_hBackNormalVBO );
        m_hBackNormalVBO = 0;
    }

    if(m_hBackTangentVBO)
    {
        glDeleteBuffers( 1, &m_hBackTangentVBO );
        m_hBackTangentVBO = 0;
    }

    if(m_hTextureVBO)
    {
        glDeleteBuffers( 1, &m_hTextureVBO );
        m_hTextureVBO = 0;
    }

    if(m_hIndexVBO)
    {
        glDeleteBuffers( 1, &m_hIndexVBO );
        m_hIndexVBO = 0;
    }

    if(m_pConstraints)
    {
        free(m_pConstraints);
        m_pConstraints = NULL;
    }


}


//-----------------------------------------------------------------------------
void CClothSim::UpdateVBOs()
//-----------------------------------------------------------------------------
{
    // Update all VBO's here
    glBindBuffer( GL_ARRAY_BUFFER, m_hPositionVBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pVerts );

    glBindBuffer( GL_ARRAY_BUFFER, m_hNormalVBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pNorms );

    glBindBuffer( GL_ARRAY_BUFFER, m_hBackNormalVBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pBackNorms );

    glBindBuffer( GL_ARRAY_BUFFER, m_hTangentVBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pTangents );

    glBindBuffer( GL_ARRAY_BUFFER, m_hBackTangentVBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pBackTangents );

    glBindBuffer( GL_ARRAY_BUFFER, m_hBitangentVBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pBitangents );

    glBindBuffer( GL_ARRAY_BUFFER, m_hBackBitangentVBO );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_uiNumVerts * 4 * sizeof(float), m_pBackBitangents );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

}

//-----------------------------------------------------------------------------
void CClothSim::Tick(float fltElapsed)
//-----------------------------------------------------------------------------
{
    if(fltElapsed == 0.0f)
    {
        m_TickTime = 0.0f;
        return;
    }

    m_fltSystemTime += fltElapsed;

    // Update spheres
    AnimateSpheres(fltElapsed);

    // Update wind
    m_CurrentWind.v[0] = 0.25f * cosf(m_fltSystemTime);
    m_CurrentWind.v[1] = 0.2f + 0.1f * cosf(m_fltSystemTime);
    m_CurrentWind.v[2] = 0.3f * sinf(0.5f * m_fltSystemTime);

    // Do we have an active pulse?
    if(m_fltSystemTime - m_PulseStartTime <= m_PulseDuration)
    {
        m_CurrentWind += m_PulseStrength * m_PulseDirection;
    }

    // No more wind forces, safe to get normalized direction
    m_WindDir = FrmVector3Normalize(m_CurrentWind);


    // For Each Vert:
    //      Make sure constraints are satisfied
    //      Add in gravity
    //      TODO: Add in Wind force
    //      Calculate new position using Verlet Integration
    //      Check for collision with objects

    // When satisfying constraints, need to run several loops to make
    // sure they are all satisfied.
    for(UINT32 uiIndx = 0; uiIndx < m_ConstraintIters; uiIndx++)
    {
        SatisfyConstraints();
    }


    float ElapsedSquared = fltElapsed * fltElapsed;

    // NOTE: Not doing top row
    for(UINT32 uiIndx = 0; uiIndx < m_uiNumVerts - CLOTH_POINTS_WIDTH; uiIndx++)
    {
        FRMVECTOR3 Acceleration = m_Gravity;
        Acceleration *= 0.25f;

        // Add wind forces
        FRMVECTOR3 Normal; loadXYZ(Normal, &m_pNorms[uiIndx * 3]);

        float Amount = fabs(FrmVector3Dot(Normal, m_WindDir));
        Acceleration += (m_CurrentWind * Amount);

        // Verlet Integration
        float TempX = m_pVerts[uiIndx * 4 + 0];
        float TempY = m_pVerts[uiIndx * 4 + 1];
        float TempZ = m_pVerts[uiIndx * 4 + 2];

        float CurrentX = m_pVerts[uiIndx * 4 + 0];
        float CurrentY = m_pVerts[uiIndx * 4 + 1];
        float CurrentZ = m_pVerts[uiIndx * 4 + 2];

        float OldX = m_pOldVerts[uiIndx * 4 + 0];
        float OldY = m_pOldVerts[uiIndx * 4 + 1];
        float OldZ = m_pOldVerts[uiIndx * 4 + 2];

        float NewX = CurrentX + (CurrentX - OldX) * m_VelDamping.v[0] + Acceleration.v[0] * ElapsedSquared;
        float NewY = CurrentY + (CurrentY - OldY) * m_VelDamping.v[1] + Acceleration.v[1] * ElapsedSquared;
        float NewZ = CurrentZ + (CurrentZ - OldZ) * m_VelDamping.v[2] + Acceleration.v[2] * ElapsedSquared;

        m_pVerts[uiIndx * 4 + 0] = NewX;
        m_pVerts[uiIndx * 4 + 1] = NewY;
        m_pVerts[uiIndx * 4 + 2] = NewZ;

        m_pOldVerts[uiIndx * 4 + 0] = TempX;
        m_pOldVerts[uiIndx * 4 + 1] = TempY;
        m_pOldVerts[uiIndx * 4 + 2] = TempZ;

        // Check for collision with Spheres
        for(UINT32 uiSphereIndx = 0; uiSphereIndx < MAX_COLLISION_SPHERES; uiSphereIndx++)
        {
            SphereInfo *pOneSphere = &m_SphereInfo[uiSphereIndx];
            if(!pOneSphere->Active)
                continue;

            FRMVECTOR3 Position = FRMVECTOR3(NewX, NewY, NewZ);
            FRMVECTOR3 ToCenter;
            ToCenter.x = Position.x - pOneSphere->vCenter.x;
            ToCenter.y = Position.y - pOneSphere->vCenter.y;
            ToCenter.z = Position.z - pOneSphere->vCenter.z;
            float Dist = FrmVector3Length(ToCenter);
            if(Dist <= pOneSphere->Radius)
            {
                // This particle is inside the sphere
                // Move to the surface
                ToCenter = FrmVector3Normalize(ToCenter);
                ToCenter *= pOneSphere->Radius;
                ToCenter *= 1.05f;
                FRMVECTOR3 NewPos;
                NewPos.x = pOneSphere->vCenter.x + ToCenter.x;
                NewPos.y = pOneSphere->vCenter.y + ToCenter.y;
                NewPos.z = pOneSphere->vCenter.z + ToCenter.z;

                m_pVerts[uiIndx * 4 + 0] = NewPos.v[0];
                m_pVerts[uiIndx * 4 + 1] = NewPos.v[1];
                m_pVerts[uiIndx * 4 + 2] = NewPos.v[2];

                // If we set the old positions we don't get the impulse behavior
                m_pOldVerts[uiIndx * 4 + 0] = NewPos.v[0];
                m_pOldVerts[uiIndx * 4 + 1] = NewPos.v[1];
                m_pOldVerts[uiIndx * 4 + 2] = NewPos.v[2];

                //pOneSphere->vVelocity[0] *= 0.5f;
                //pOneSphere->vVelocity[2] *= 0.999f;

                // After initial impact, kill this sphere
                pOneSphere->ShouldKill = true;
            }
        }
    }


    // Now that the verts have changed, update the normals
    UpdateNormals();

}

//-----------------------------------------------------------------------------
void CClothSim::SatisfyConstraints()
//-----------------------------------------------------------------------------
{
    for (UINT32 uiIndx = 0; uiIndx < m_uiNumConstraints; uiIndx++)
    {
        Constraint *pConstraint = &m_pConstraints[uiIndx];
        FRMVECTOR3 vecA; loadXYZ(vecA, (float *)&m_pVerts[pConstraint->uiVertA * 4]);
        FRMVECTOR3 vecB; loadXYZ(vecB, (float *)&m_pVerts[pConstraint->uiVertB * 4]);
        FRMVECTOR3 vecDiff = vecB - vecA;

        float CurrentDist = FrmVector3Length(vecDiff);
        if(CurrentDist == 0.0f)
            continue;

        FRMVECTOR3 Correct = vecDiff * (1.0f - pConstraint->BaseDist / CurrentDist);
        Correct *= 0.5f;

        if(pConstraint->uiVertA < CLOTH_POINTS_WIDTH * (CLOTH_POINTS_HEIGHT - 1) )
        {
            m_pVerts[pConstraint->uiVertA * 4 + 0] += Correct.v[0];
            m_pVerts[pConstraint->uiVertA * 4 + 1] += Correct.v[1];
            m_pVerts[pConstraint->uiVertA * 4 + 2] += Correct.v[2];
        }

        if(pConstraint->uiVertB < CLOTH_POINTS_WIDTH * (CLOTH_POINTS_HEIGHT - 1) )
        {
            m_pVerts[pConstraint->uiVertB * 4 + 0] -= Correct.v[0];
            m_pVerts[pConstraint->uiVertB * 4 + 1] -= Correct.v[1];
            m_pVerts[pConstraint->uiVertB * 4 + 2] -= Correct.v[2];
        }

    }
}

//-----------------------------------------------------------------------------
void CClothSim::UpdateNormals()
//-----------------------------------------------------------------------------
{
    // Calculate normal using cross product.
    // For right, invert the cross product calculation

    for (UINT32 uiY = 0; uiY < CLOTH_POINTS_HEIGHT - 1; uiY++)
    {
        for (UINT32 uiX = 0; uiX < CLOTH_POINTS_WIDTH - 1; uiX++)
        {

            UINT32 WhichVertex = uiX + uiY * CLOTH_POINTS_WIDTH;

            FRMVECTOR3 Current; loadXYZ(Current, &m_pVerts[WhichVertex * 4]);
            FRMVECTOR3 Right; loadXYZ(Right, &m_pVerts[(WhichVertex + 1) * 4]);
            FRMVECTOR3 Above; loadXYZ(Above, &m_pVerts[(WhichVertex + CLOTH_POINTS_WIDTH) * 4]);

            FRMVECTOR3 vecA = Right - Current;
            FRMVECTOR3 vecB = Above - Current;

            if(FrmVector3Length(vecA) <= 0.0f)
                continue;

            vecA = FrmVector3Normalize(vecA);
            vecB = FrmVector3Normalize(vecB);

            FRMVECTOR3 Norm = FrmVector3Cross(vecA, vecB);

            // To save multiplies since from here on out always using "WhichVertex * 4"
            WhichVertex *= 4;

            Norm *= -1.0f;
            vecA *= -1.0f;

            m_pNorms[WhichVertex + 0] = Norm.v[0];
            m_pNorms[WhichVertex + 1] = Norm.v[1];
            m_pNorms[WhichVertex + 2] = Norm.v[2];

            m_pTangents[WhichVertex + 0] = vecA.v[0];
            m_pTangents[WhichVertex + 1] = vecA.v[1];
            m_pTangents[WhichVertex + 2] = vecA.v[2];

            FRMVECTOR3 Bitan = FrmVector3Cross(vecA, Norm);
            m_pBitangents[WhichVertex + 0] = Bitan.v[0];
            m_pBitangents[WhichVertex + 1] = Bitan.v[1];
            m_pBitangents[WhichVertex + 2] = Bitan.v[2];

            // Back is inverted
            Norm *= -1.0f;
            vecA *= -1.0f;

            m_pBackNorms[WhichVertex + 0] = Norm.v[0];
            m_pBackNorms[WhichVertex + 1] = Norm.v[1];
            m_pBackNorms[WhichVertex + 2] = Norm.v[2];

            m_pBackTangents[WhichVertex + 0] = vecA.v[0];
            m_pBackTangents[WhichVertex + 1] = vecA.v[1];
            m_pBackTangents[WhichVertex + 2] = vecA.v[2];

            Bitan = FrmVector3Cross(vecA, Norm);
            m_pBackBitangents[WhichVertex + 0] = Bitan.v[0];
            m_pBackBitangents[WhichVertex + 1] = Bitan.v[1];
            m_pBackBitangents[WhichVertex + 2] = Bitan.v[2];
        }

        // Fix the right edge we just skipped over
        UINT32 WhichVertex = (CLOTH_POINTS_WIDTH - 1) + uiY * CLOTH_POINTS_WIDTH;

        FRMVECTOR3 Current; loadXYZ(Current, &m_pVerts[WhichVertex * 4]);
        FRMVECTOR3 Left; loadXYZ(Left, &m_pVerts[(WhichVertex - 1) * 4]);
        FRMVECTOR3 Above; loadXYZ(Above, &m_pVerts[(WhichVertex + CLOTH_POINTS_WIDTH) * 4]);

        FRMVECTOR3 vecA = Left - Current;
        FRMVECTOR3 vecB = Above - Current;

        vecA = FrmVector3Normalize(vecA);
        vecB = FrmVector3Normalize(vecB);

        FRMVECTOR3 Norm = FrmVector3Cross(vecB, vecA);

        // To save multiplies since from here on out always using "WhichVertex * 4"
        WhichVertex *= 4;

        Norm *= -1.0f;

        m_pNorms[WhichVertex + 0] = Norm.v[0];
        m_pNorms[WhichVertex + 1] = Norm.v[1];
        m_pNorms[WhichVertex + 2] = Norm.v[2];

        // Notice this is inverted since tangent is supposed to go to the right
        m_pTangents[WhichVertex + 0] = vecA.v[0];
        m_pTangents[WhichVertex + 1] = vecA.v[1];
        m_pTangents[WhichVertex + 2] = vecA.v[2];

        FRMVECTOR3 Bitan = FrmVector3Cross(vecA, Norm);
        m_pBitangents[WhichVertex + 0] = Bitan.v[0];
        m_pBitangents[WhichVertex + 1] = Bitan.v[1];
        m_pBitangents[WhichVertex + 2] = Bitan.v[2];


        Norm *= -1.0f;
        vecA *= -1.0f;
        m_pBackNorms[WhichVertex + 0] = Norm.v[0];
        m_pBackNorms[WhichVertex + 1] = Norm.v[1];
        m_pBackNorms[WhichVertex + 2] = Norm.v[2];

        // Notice this is inverted since tangent is supposed to go to the right
        m_pBackTangents[WhichVertex + 0] = vecA.v[0];
        m_pBackTangents[WhichVertex + 1] = vecA.v[1];
        m_pBackTangents[WhichVertex + 2] = vecA.v[2];

        Bitan = FrmVector3Cross(vecA, Norm);
        m_pBackBitangents[WhichVertex + 0] = Bitan.v[0];
        m_pBackBitangents[WhichVertex + 1] = Bitan.v[1];
        m_pBackBitangents[WhichVertex + 2] = Bitan.v[2];

    }

    // Fix the top row (match row below it)
    for (UINT32 uiX = 0; uiX < CLOTH_POINTS_WIDTH; uiX++)
    {
        UINT32 WhichVertex = uiX + (CLOTH_POINTS_HEIGHT - 1) * CLOTH_POINTS_WIDTH;
        UINT32 BelowVertex = WhichVertex - CLOTH_POINTS_WIDTH;

        // To save multiplies since from here on out always using "WhichVertex * 4"
        WhichVertex *= 4;
        BelowVertex *= 4;

        m_pNorms[WhichVertex + 0] = m_pNorms[BelowVertex + 0];
        m_pNorms[WhichVertex + 1] = m_pNorms[BelowVertex + 1];
        m_pNorms[WhichVertex + 2] = m_pNorms[BelowVertex + 2];

        m_pTangents[WhichVertex + 0] = m_pTangents[BelowVertex + 0];
        m_pTangents[WhichVertex + 1] = m_pTangents[BelowVertex + 1];
        m_pTangents[WhichVertex + 2] = m_pTangents[BelowVertex + 2];

        m_pBitangents[WhichVertex + 0] = m_pBitangents[BelowVertex + 0];
        m_pBitangents[WhichVertex + 1] = m_pBitangents[BelowVertex + 1];
        m_pBitangents[WhichVertex + 2] = m_pBitangents[BelowVertex + 2];

        m_pBackNorms[WhichVertex + 0] = m_pBackNorms[BelowVertex + 0];
        m_pBackNorms[WhichVertex + 1] = m_pBackNorms[BelowVertex + 1];
        m_pBackNorms[WhichVertex + 2] = m_pBackNorms[BelowVertex + 2];

        m_pBackTangents[WhichVertex + 0] = m_pBackTangents[BelowVertex + 0];
        m_pBackTangents[WhichVertex + 1] = m_pBackTangents[BelowVertex + 1];
        m_pBackTangents[WhichVertex + 2] = m_pBackTangents[BelowVertex + 2];

        m_pBackBitangents[WhichVertex + 0] = m_pBackBitangents[BelowVertex + 0];
        m_pBackBitangents[WhichVertex + 1] = m_pBackBitangents[BelowVertex + 1];
        m_pBackBitangents[WhichVertex + 2] = m_pBackBitangents[BelowVertex + 2];
    }


}

//-----------------------------------------------------------------------------
void CClothSim::SubmitDraw()
//-----------------------------------------------------------------------------
{
    INT32 iAttribSlot;

    for(UINT32 uiFront = 0; uiFront < 2; uiFront++)
    {
        // Set the attributes
        iAttribSlot = FRM_VERTEX_POSITION;
        if(iAttribSlot >= 0)
        {
            glBindBuffer( GL_ARRAY_BUFFER, m_hPositionVBO );
            glVertexAttribPointer( iAttribSlot, 4, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray( iAttribSlot );
        }

        iAttribSlot = FRM_VERTEX_NORMAL;
        if(iAttribSlot >= 0)
        {
            if(uiFront)
            {
                glBindBuffer( GL_ARRAY_BUFFER, m_hNormalVBO );
            }
            else
            {
                glBindBuffer( GL_ARRAY_BUFFER, m_hBackNormalVBO );
            }
            glVertexAttribPointer( iAttribSlot, 4, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray( iAttribSlot );
        }

        iAttribSlot = FRM_VERTEX_TANGENT;
        if(iAttribSlot >= 0)
        {
            if(uiFront)
            {
                glBindBuffer( GL_ARRAY_BUFFER, m_hTangentVBO );
            }
            else
            {
                glBindBuffer( GL_ARRAY_BUFFER, m_hBackTangentVBO );
            }
            glVertexAttribPointer( iAttribSlot, 4, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray( iAttribSlot );
        }

        iAttribSlot = FRM_VERTEX_BINORMAL;
        if(iAttribSlot >= 0)
        {
            if(uiFront)
            {
                glBindBuffer( GL_ARRAY_BUFFER, m_hBitangentVBO );
            }
            else
            {
                glBindBuffer( GL_ARRAY_BUFFER, m_hBackBitangentVBO );
            }
            glVertexAttribPointer( iAttribSlot, 4, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray( iAttribSlot );
        }

        iAttribSlot = FRM_VERTEX_TEXCOORD0;
        if(iAttribSlot >= 0)
        {
            glBindBuffer( GL_ARRAY_BUFFER, m_hTextureVBO );
            glVertexAttribPointer( iAttribSlot, 2, GL_FLOAT, 0, 0, 0);
            glEnableVertexAttribArray( iAttribSlot );
        }

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_hIndexVBO );

        if(uiFront)
        {
            glFrontFace(GL_CCW);
        }
        else
        {
            glFrontFace(GL_CW);
        }
        // glDrawElements(GL_TRIANGLE_STRIP, m_uiNumIndicies, GL_UNSIGNED_SHORT, m_pIndicies);
        glDrawElements(GL_TRIANGLE_STRIP, m_uiNumIndicies, GL_UNSIGNED_SHORT, 0);

        // Unbind the VBO
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

        for(UINT32 i = 0; i <= FRM_VERTEX_COLOR3; ++i)
        {
            glDisableVertexAttribArray(i);
        }

    }   // Front/Back render loop

    // Make sure the winding is back where it should be
    glFrontFace(GL_CCW);

}

//-----------------------------------------------------------------------------
void CClothSim::AnimateSpheres(float fltElapsed)
//-----------------------------------------------------------------------------
{
    FRMVECTOR3 Position;
    for(UINT32 uiSphereIndx = 0; uiSphereIndx < MAX_COLLISION_SPHERES; uiSphereIndx++)
    {
        SphereInfo *pOneSphere = &m_SphereInfo[uiSphereIndx];

        if(pOneSphere->ShouldKill)
        {
            pOneSphere->Active = false;
            pOneSphere->ShouldKill = false;
        }
        if(!pOneSphere->Active)
            continue;

        // TODO: What is the definition of dead!
        if( pOneSphere->vCenter.v[0] > 10.0f || pOneSphere->vCenter.v[0] < -10.0f ||
            pOneSphere->vCenter.v[1] > 10.0f || pOneSphere->vCenter.v[1] < -10.0f ||
            pOneSphere->vCenter.v[2] > 10.0f || pOneSphere->vCenter.v[2] < -10.0f)
        {
            // This one just died!
            // FrmLogMessage("Sphere %d just died (%0.2f, %0.2f, %0.2f)", uiSphereIndx, pOneSphere->vCenter[0], pOneSphere->vCenter[1], pOneSphere->vCenter[2]);
            pOneSphere->Active = false;
            pOneSphere->ShouldKill = false;
            continue;
        }

        // Adjust the velocity before applying to position
        // pOneSphere->vVelocity[1] += (0.1f * m_Gravity[1] * fltElapsed);
        pOneSphere->vCenter += pOneSphere->vVelocity * fltElapsed;
    }

}

//-----------------------------------------------------------------------------
void CClothSim::AddWindPulse(float Strength, float *pDirection, float Duration)
//-----------------------------------------------------------------------------
{
    m_PulseStartTime = m_fltSystemTime;
    m_PulseStrength = Strength;
    m_PulseDuration = Duration;
    m_PulseDirection = FRMVECTOR3(pDirection[0], pDirection[1], pDirection[2]);
    m_PulseDirection = FrmVector3Normalize(m_PulseDirection);

    float Test = FrmVector3Length(m_PulseDirection);
    if(Test < 0.999f)
    {
        char str[1024];
        FrmSprintf( str, sizeof(str), "Invalid Wind Direction: (%0.2f, %0.2f, %0.2f)", pDirection[0], pDirection[1], pDirection[2]);
        FrmLogMessage( str );
        m_PulseStartTime = 0.0f;
    }
}

//-----------------------------------------------------------------------------
void CClothSim::LaunchSphere(float ScreenPosX, float ScreenPosY, FRMMATRIX4X4 &matVp, FRMVECTOR3 &CameraPos)
//-----------------------------------------------------------------------------
{
    // FrmLogMessage("LaunchSphere(%0.2f, %0.2f)", xPos, yPos);

    // Rather than launch a sphere, add a wind pulse from camera
    FRMVECTOR3 SpherePos = CameraPos;
    FRMVECTOR3 ImpactPoint;
    FindImpactPoint(ScreenPosX, ScreenPosY, matVp, ImpactPoint);

    // Don't really want to start at camera position since this is supposed to be a touch
    FRMVECTOR3 ToImpact = ImpactPoint - CameraPos;
    ToImpact = FrmVector3Normalize(ToImpact);

    float Strength = 5.0f;
    float Direction[3];
    float Duration = 1.0f;

    Direction[0] = ToImpact.v[0];
    Direction[1] = ToImpact.v[1];
    Direction[2] = ToImpact.v[2];

    AddWindPulse(Strength, Direction, Duration);

#ifdef WORKING_BUT_NOT_SUPPORTED_NOW

    // Find the first one that is not active
    SphereInfo *pOneSphere = NULL;
    for(UINT32 uiIndx = 0; uiIndx < MAX_COLLISION_SPHERES && pOneSphere == NULL; uiIndx++)
    {
        if(!m_SphereInfo[uiIndx].Active)
        {
            //FrmLogMessage("Sphere %d just launched", uiIndx);
            pOneSphere = &m_SphereInfo[uiIndx];
        }
    }

    if(pOneSphere == NULL)
    {
        // Rather than not launch anything, just stomp on the first one
        //FrmLogMessage("Sphere 0 just launched");
        pOneSphere = &m_SphereInfo[0];
    }

    // This one is now active
    pOneSphere->Active = true;
    pOneSphere->ShouldKill = false;

    // TODO: How big should the sphere be?  Currently based on the radius of cloth
    pOneSphere->Radius = 1.0f * m_BaseRadius[0];

    FRMVECTOR3 SpherePos = CameraPos;
    FRMVECTOR3 ImpactPoint;
    FindImpactPoint(ScreenPosX, ScreenPosY, matVp, ImpactPoint);

    // Don't really want to start at camera position since this is supposed to be a touch
    FRMVECTOR3 FromImpact = CameraPos - ImpactPoint;
    FromImpact = normalize(FromImpact);

    // TODO: What is the sphere velocity?
    // Negative value on multiplier since going toward impact point
    pOneSphere->vVelocity = -0.5f * FromImpact;

    FromImpact *= 0.75f * pOneSphere->Radius;
    pOneSphere->vCenter = ImpactPoint + FromImpact;

#endif // WORKING_BUT_NOT_SUPPORTED_NOW
}

//-----------------------------------------------------------------------------
BOOL CClothSim::FindImpactPoint(float ScreenPosX, float ScreenPosY, FRMMATRIX4X4 &matVp, FRMVECTOR3 &ImpactPoint)
//-----------------------------------------------------------------------------
{
    UINT32 WhichVertex = 0;
    FRMVECTOR4 TL, TR, LC;

    // Top Left
    WhichVertex = (CLOTH_POINTS_HEIGHT - 1) * CLOTH_POINTS_WIDTH;
    loadXYZW(TL, &m_pVerts[WhichVertex * 4]);
    TL = FrmVector4Transform(TL, matVp);
    TL /= TL.w;

    // Top Right
    WhichVertex = (CLOTH_POINTS_WIDTH - 1) + (CLOTH_POINTS_HEIGHT - 1) * CLOTH_POINTS_WIDTH;
    loadXYZW(TR, &m_pVerts[WhichVertex * 4]);
    TR = FrmVector4Transform(TR, matVp);
    TR /= TR.w;

    // Lower Center
    WhichVertex = CLOTH_POINTS_WIDTH / 2;
    loadXYZW(LC, &m_pVerts[WhichVertex * 4]);
    LC = FrmVector4Transform(LC, matVp);
    LC /= LC.w;

    // Passed in screen coordinates go from 0->1,  Left->Right and Top->Bottom
    // Screen coordinate go from -1->1, Left->Right and Bottom->Top
    float NewScreenPosX = 2.0f * ScreenPosX - 1.0f;
    float NewScreenPosY = 2.0f * (1.0f - ScreenPosY) - 1.0f;

    float XLerp = (NewScreenPosX - TL.v[0]) / (TR.v[0] - TL.v[0]);
    float YLerp = (NewScreenPosY - LC.v[1]) / (TL.v[1] - LC.v[1]);

    // This is not an exact science!  The closer to the center of the screen the better,
    // be the XLerp values can be off
#if TEMP
    L_HUDClampFloat(XLerp, 0.0f, 1.0f);
    L_HUDClampFloat(YLerp, 0.0f, 1.0f);
#endif
    // For now, we are returning the center of the simulation
    ImpactPoint = m_Center;

    // Finally, compute the impact point
    ImpactPoint.v[0] = m_MinValues.v[0] + XLerp * 2.0f * m_BaseRadius.v[0];
    ImpactPoint.v[1] = m_MinValues.v[1] + YLerp * 2.0f * m_BaseRadius.v[1];
    ImpactPoint.v[2] = m_Center.v[2];

    return true;
}
