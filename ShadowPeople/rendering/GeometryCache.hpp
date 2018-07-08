/*
    Copyright 2018 Samuel Siltanen
    GeometryCache.hpp
*/

#pragma once

#include "../graphics/Graphics.hpp"

#include "Mesh.hpp"

#include <vector>

namespace rendering
{
    class GeometryCache
    {
    public:
        GeometryCache(graphics::Device& device);

        int2 preloadMesh(const Mesh& mesh);

        void updateGPUBuffers(graphics::CommandBuffer& gfx);

        const graphics::BufferView& vertexBuffer() const { return m_vertexBufferSRV; }
        const graphics::Buffer& indexBuffer() const { return m_indexBuffer; }
    private:
        std::vector<Vertex>     m_vertices;
        std::vector<uint32_t>   m_indices;

        graphics::Buffer        m_vertexBuffer;
        graphics::BufferView    m_vertexBufferSRV;
        graphics::Buffer        m_indexBuffer;
    };
}
