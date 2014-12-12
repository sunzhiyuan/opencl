//--------------------------------------------------------------------------------------
// File: PostProcessCLGLES.cl
// Desc: Postprocess OpenGL ES rendered FBO and store result in texture
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

//=============================================================================
// CONSTANTS - from ClothSim C++ code mirrored in OpenCL
//=============================================================================
#define CLOTH_POINTS_WIDTH      16
#define CLOTH_POINTS_HEIGHT     16

//--------------------------------------------------------------------------------------
// Name: SatisfyConstraints()
// Desc: OpenCL implementation of satisfy constraints portion of cloth simulation
//--------------------------------------------------------------------------------------
__kernel void SatisfyConstraints( __global float4* pVerts,                                  
                                  __global float4* pOldVerts,
                                  __global float* pBaseDists,
                                  __global unsigned int* pBaseDistNeighborIndex,
                                  __global unsigned int* pBaseDistStartIndex,
                                  __global unsigned int* pBaseDistEndIndex )
{
    int vertId = get_global_id(0);

    if( vertId < ((CLOTH_POINTS_WIDTH * CLOTH_POINTS_HEIGHT) - CLOTH_POINTS_WIDTH) )
    {
        float4 oldVert = pOldVerts[ vertId ];
        unsigned int startIdx = pBaseDistStartIndex[ vertId ];
        unsigned int endIdx = pBaseDistEndIndex[ vertId ];

        for( unsigned int i = startIdx; i < endIdx; i++ )
        {
            unsigned int neighborId = pBaseDistNeighborIndex[i];
            float baseDist = pBaseDists[i];
            float4 neighborVert = pOldVerts[ neighborId ];
            float4 vecDiff = neighborVert - (oldVert);
            float dist = sqrt( vecDiff.x * vecDiff.x + vecDiff.y * vecDiff.y + vecDiff.z * vecDiff.z );
            if( dist != 0.0f )
            {
                float4 correctAdd = vecDiff * (dist - baseDist) / dist ;
                correctAdd *= 0.5f;
                oldVert += correctAdd;
            }
        }
        pVerts[vertId] = oldVert;
    }
}

//--------------------------------------------------------------------------------------
// Name: VerletIntegration()
// Desc: OpenCL implementation of verlet integration portion of Cloth Simulation
//--------------------------------------------------------------------------------------
__kernel void VerletIntegration( float ElapsedSquared,
                                 float4 Acceleration,
                                 float4 VelDamping,
                                 float4 WindDir,
                                 float4 CurrentWind,
                                 __global float4* pVerts,
                                 __global float4* pOldVerts,
                                 __global float4* pNormals)
{
    int uiIndx = get_global_id(0); // Vertex index
    
    // Add wind forces
    float4 Normal = pNormals[uiIndx];
    
    float Amount = fabs( dot( Normal, WindDir ) );
    Acceleration += (CurrentWind * Amount);

    // Verlet Integration
    float4 Temp = pVerts[uiIndx];
    float4 Current = pVerts[uiIndx];
    float4 Old = pOldVerts[uiIndx];

    float4 New = Current + (Current - Old) * VelDamping + Acceleration * ElapsedSquared;

    pVerts[uiIndx] = New;
    pOldVerts[uiIndx] = Temp;
}

//--------------------------------------------------------------------------------------
// Name: UpdateNormals()
// Desc: OpenCL implementation to generate normal/binormal/tangent data for cloth
//--------------------------------------------------------------------------------------
__kernel void UpdateNormals( __global float4* pVerts,
                             __global float4* pNorms,
                             __global float4* pTangents,
                             __global float4* pBitangents,
                             __global float4* pBackNorms,
                             __global float4* pBackTangents,
                             __global float4* pBackBitangents )
{
    int uiX = get_global_id(0);
    int uiY = get_global_id(1);

    int WhichVertex = uiX + uiY * CLOTH_POINTS_WIDTH;

    float4 Current = pVerts[WhichVertex];
    float4 Right = pVerts[WhichVertex + 1];
    float4 Above = pVerts[(WhichVertex + CLOTH_POINTS_WIDTH)];

    float4 vecA = Right - Current;
    float4 vecB = Above - Current;

    if(length(vecA) > 0.0f)
    {
        vecA = normalize(vecA);
        vecB = normalize(vecB);

        float4 Norm = cross(vecA, vecB);
            
        Norm *= -1.0f;
        vecA *= -1.0f;

        pNorms[WhichVertex] = Norm;
        pTangents[WhichVertex] = vecA;

        float4 Bitan = cross(vecA, Norm);
        pBitangents[WhichVertex] = Bitan;

        // Back is inverted
        Norm *= -1.0f;
        vecA *= -1.0f;

        pBackNorms[WhichVertex] = Norm;
        pBackTangents[WhichVertex] = vecA;

        Bitan = cross(vecA, Norm);
        pBackBitangents[WhichVertex] = Bitan;

    }

    // Fix the right edge
    if ( uiX == (CLOTH_POINTS_WIDTH - 2) )
    {
        // Fix the right edge we just skipped over
        WhichVertex = (CLOTH_POINTS_WIDTH - 1) + uiY * CLOTH_POINTS_WIDTH;

        Current = pVerts[WhichVertex];
        float4 Left = pVerts[(WhichVertex - 1)];
        Above = pVerts[(WhichVertex + CLOTH_POINTS_WIDTH)];

        vecA = Left - Current;
        vecB = Above - Current;

        vecA = normalize(vecA);
        vecB = normalize(vecB);

        float4 Norm = cross(vecB, vecA);

        Norm *= -1.0f;

        pNorms[WhichVertex] = Norm;

        // Notice this is inverted since tangent is supposed to go to the right
        pTangents[WhichVertex + 0] = vecA;

        float4 Bitan = cross(vecA, Norm);
        pBitangents[WhichVertex] = Bitan;

        Norm *= -1.0f;
        vecA *= -1.0f;
        pBackNorms[WhichVertex] = Norm;

        // Notice this is inverted since tangent is supposed to go to the right
        pBackTangents[WhichVertex] = vecA;

        Bitan = cross(vecA, Norm);
        pBackBitangents[WhichVertex] = Bitan;
    }

    // Fix the top row (match row below it)
    if ( uiY == (CLOTH_POINTS_HEIGHT - 2) )
    {
        WhichVertex = uiX + (CLOTH_POINTS_HEIGHT - 1) * CLOTH_POINTS_WIDTH;
        int BelowVertex = WhichVertex - CLOTH_POINTS_WIDTH;

        pNorms[WhichVertex ] = pNorms[BelowVertex];
        pTangents[WhichVertex] = pTangents[BelowVertex];
        pBitangents[WhichVertex] = pBitangents[BelowVertex];
        
        pBackNorms[WhichVertex] = pBackNorms[BelowVertex];        
        pBackTangents[WhichVertex] = pBackTangents[BelowVertex];
        pBackBitangents[WhichVertex] = pBackBitangents[BelowVertex];

        // Also do the last one if this is the 0'th
        if (uiX == 0)
        {
            uiX = CLOTH_POINTS_WIDTH - 1;
            WhichVertex = uiX + (CLOTH_POINTS_HEIGHT - 1) * CLOTH_POINTS_WIDTH;
            int BelowVertex = WhichVertex - CLOTH_POINTS_WIDTH;

            pNorms[WhichVertex ] = pNorms[BelowVertex];
            pTangents[WhichVertex] = pTangents[BelowVertex];
            pBitangents[WhichVertex] = pBitangents[BelowVertex];
            
            pBackNorms[WhichVertex] = pBackNorms[BelowVertex];        
            pBackTangents[WhichVertex] = pBackTangents[BelowVertex];
            pBackBitangents[WhichVertex] = pBackBitangents[BelowVertex];


        }
    }
}