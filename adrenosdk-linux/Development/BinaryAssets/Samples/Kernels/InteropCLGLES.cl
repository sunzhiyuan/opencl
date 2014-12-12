//--------------------------------------------------------------------------------------
// File: InteropCLGLES.cl
// Desc: Simple kernel that calculates graph in OpenCL to be bound to OpenGL ES VBO
//
// Author:       QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc.
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#define PI (3.1415926f)

//--------------------------------------------------------------------------------------
// Name: ComputeGraph()
// Desc: Compute points on a graph.  The results of this simple kernel are used as an
// OpenGL ES VBO to demonstrate OpenCL-OpenGL ES interop
//--------------------------------------------------------------------------------------
__kernel void ComputeGraph(__global float4* pVBO,       // VBO for output
                           const float timeDelta )      // Accumulated time delta
{
    int i = get_global_id(0);
    int numSegments = get_global_size(0);
    float t = ( (float)i / (float) (numSegments - 1) );
        
    // t goes from [-1, 1]
    t = (t * 2.0f - 1.0f);

    // Output this vertex
    pVBO[i].x = t;
    pVBO[i].y = sin( PI * (t + timeDelta ) );
    pVBO[i].z = 0.0f;
    pVBO[i].w = 1.0f;
}
