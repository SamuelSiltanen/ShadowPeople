#pragma once

#include <vector>
#include "../Types.hpp"

namespace rendering
{
	struct Vertex
	{
		float3	position;
		float3	normal;
		float2	uv;
	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(Range<Vertex> vertices, Range<uint32_t> indices);

		void fill(Range<Vertex> vertices, Range<uint32_t> indices);

        // TODO: Make these const ranges
        const std::vector<Vertex>& vertices() const { return m_vertices; }
        const std::vector<uint32_t>& indices() const { return m_indices; }
	private:
		std::vector<Vertex>		m_vertices;
		std::vector<uint32_t>	m_indices;
	};
}
