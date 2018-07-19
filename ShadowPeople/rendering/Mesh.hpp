/*
    Copyright 2018 Samuel Siltanen
    Mesh.hpp
*/

#pragma once

#include <vector>
#include "../Types.hpp"
#include "../cpugpu/GeometryTypes.h"

namespace rendering
{
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
        void calculateOrientations();

		std::vector<Vertex>		m_vertices;
		std::vector<uint32_t>	m_indices;
	};
}
