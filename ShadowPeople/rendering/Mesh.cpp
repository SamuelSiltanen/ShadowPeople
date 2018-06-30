#include "Mesh.hpp"

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
	}
}
