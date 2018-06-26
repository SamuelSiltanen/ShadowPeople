#include "Mesh.hpp"

namespace rendering
{
	Mesh::Mesh(Range<Vertex>& vertices, Range<uint32_t>& indices)
	{
		m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
		m_indices.insert(m_indices.end(), indices.begin(), indices.end());
	}
}
