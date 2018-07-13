#include "Mesh.hpp"
#include "../Errors.hpp"

//#define EXTRA_CHECKS

namespace rendering
{
	Mesh::Mesh(Range<Vertex> vertices, Range<uint32_t> indices)
	{
		fill(vertices, indices);
	}

	void Mesh::fill(Range<Vertex> vertices, Range<uint32_t> indices)
	{
		m_vertices.clear();
		m_indices.clear();
		m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
		m_indices.insert(m_indices.end(), indices.begin(), indices.end());

        calculateOrientations();
	}

    void Mesh::calculateOrientations()
    {
        for (int i = 0; i < m_indices.size(); i += 3)
        {
            uint32_t i0 = m_indices[i + 0];
            uint32_t i1 = m_indices[i + 1];
            uint32_t i2 = m_indices[i + 2];

            // Triangle corners
            float3 p0   = m_vertices[i0].position;
            float3 p1   = m_vertices[i1].position;
            float3 p2   = m_vertices[i2].position;

            // Two edges
            float3 e1   = p1 - p0;
            float3 e2   = p2 - p0;
            
            // Normal
            float3 n = normalize(cross(e2, e1));

            // UV corners
            float2 uv0  = m_vertices[i0].uv;
            float2 uv1  = m_vertices[i1].uv;
            float2 uv2  = m_vertices[i2].uv;

            // UV edges
            float2 f1   = uv1 - uv0;
            float2 f2   = uv2 - uv0;

            // Bitangent candidate - can be found as a linear combination of the two edges, so that uv = (1, 0)
            float det   = f1[0] * f2[1] - f1[1] * f2[0];
            float c1    = f2[1] / det;
            float c2    = -f1[1] / det;

            float3 b    = normalize(c1 * e1 + c2 * e2);

#if EXTRA_CHECKS
            const float Epsilon = 1e-1f;

            float2 uu = c1 * f1 + c2 * f2;
            if (fabs(uu[0] - 1.f) > Epsilon || fabs(uu[1]) > Epsilon)
            {
                std::string msg("Fail: ");
                msg.append(std::to_string(uu[0])).append(" ");
                msg.append(std::to_string(uu[1])).append("\n");
                OutputDebugString(msg.c_str());
            }
#endif
            
            // Tangent candidate            
            float d1    = -f2[0] / det;
            float d2    = f1[0] / det;

            float3 t    = normalize(d1 * e1 + d2 * e2);

#if EXTRA_CHECKS
            float2 vv = d1 * f1 + d2 * f2;
            if (fabs(vv[0]) > Epsilon || fabs(vv[1] - 1.f) > Epsilon)
            {
                std::string msg("Fail: ");
                msg.append(std::to_string(vv[0])).append(" ");
                msg.append(std::to_string(vv[1])).append("\n");
                OutputDebugString(msg.c_str());
            }

            // Chech orthogonality            
            float nt = n.dot(t);
            float nb = n.dot(b);
            float tb = t.dot(b);
            if (fabs(nt) > Epsilon || fabs(nb) > Epsilon || fabs(tb) > Epsilon)
            {
                std::string msg("Orthogonality broken: ");
                msg.append(std::to_string(nt)).append(" ");
                msg.append(std::to_string(nb)).append(" ");
                msg.append(std::to_string(tb)).append("\n");
                OutputDebugString(msg.c_str());
            }
#endif

            // Check for UV-mirroring
            float bitangentSign = (n.dot(cross(b, t)) < 0.f) ? -1.f : 1.f;
            //b *= bitangentSign;

            // Bend the tangent frames to match the vertex normals
            float3 b0   = normalize(b - b.dot(m_vertices[i0].normal) * m_vertices[i0].normal);
            float3 b1   = normalize(b - b.dot(m_vertices[i1].normal) * m_vertices[i1].normal);
            float3 b2   = normalize(b - b.dot(m_vertices[i2].normal) * m_vertices[i2].normal);            

            // Tangent
            float3 t0   = cross(m_vertices[i0].normal, b0);
            float3 t1   = cross(m_vertices[i1].normal, b1);
            float3 t2   = cross(m_vertices[i2].normal, b2);

            // Pack in quaternion
            m_vertices[i0].orientation = Quaternion(Matrix3x3({b0, t0, m_vertices[i0].normal/*b, t, n*/})).toFloat4();
            m_vertices[i1].orientation = Quaternion(Matrix3x3({b1, t1, m_vertices[i1].normal/*b, t, n*/})).toFloat4();
            m_vertices[i2].orientation = Quaternion(Matrix3x3({b2, t2, m_vertices[i2].normal/*b, t, n*/})).toFloat4();

            m_vertices[i0].bitangentSign = bitangentSign;
            m_vertices[i1].bitangentSign = bitangentSign;
            m_vertices[i2].bitangentSign = bitangentSign;
        }
    }
}
