//-----------------------------------------------------------------------------
// 
//  VertexFormat
//	
//  Copyright (c) 2013 QUALCOMM Technologies, Inc.
//  All Rights Reserved. QUALCOMM Proprietary/GTDR
//	
//-----------------------------------------------------------------------------

#ifndef MCE_FBX_VERTEX_FORMAT_H
#define MCE_FBX_VERTEX_FORMAT_H

#include "VertexProperty.h"
#include "Base.h"

//-----------------------------------------------------------------------------

namespace MCE
{
    namespace FBX
    {
        class VertexFormat
        {
            public:
                 VertexFormat();
                ~VertexFormat();

                void                    AddProperty     ( int usage, int num_values = 1 );

                int                     NumProperties   () const;
                const VertexProperty&   GetProperty     ( int index ) const;

                uint32                  VertexStride    () const;

                bool                    HasPosition     () const { return m_has_position; }
                bool                    HasNormal       () const { return m_has_normal; }
                bool                    HasBinormal     () const { return m_has_binormal; }
                bool                    HasTangent      () const { return m_has_tangent; }
                bool                    HasSkinIndices  () const { return m_has_skin_indices; }
                bool                    HasSkinWeights  () const { return m_has_skin_weights; }
                bool                    HasColors       () const { return m_has_colors; }
                bool                    HasUVs          () const { return m_has_uvs; }

                int                     NumSkinIndices  () const { return m_num_skin_indices; }
                int                     NumSkinWeights  () const { return m_num_skin_weights; }
                int                     NumColors       () const { return m_num_colors; }
                int                     NumUVs          () const { return m_num_uvs; }

            private:
                VertexPropertyArray m_properties;

                bool m_has_position;
                bool m_has_normal;
                bool m_has_binormal;
                bool m_has_tangent;
                bool m_has_skin_indices;
                bool m_has_skin_weights;
                bool m_has_colors;
                bool m_has_uvs;

                int  m_num_skin_indices;
                int  m_num_skin_weights;
                int  m_num_colors;
                int  m_num_uvs;
        };
    }
}

//-----------------------------------------------------------------------------

#endif