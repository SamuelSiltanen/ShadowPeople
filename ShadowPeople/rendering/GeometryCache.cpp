/*
    Copyright 2018 Samuel Siltanen
    GeometryCache.hpp
*/

#include "GeometryCache.hpp"

using namespace graphics;

namespace rendering
{
    constexpr uint32_t VertexBufferSize = (1 << 20);
    constexpr uint32_t IndexBufferSize  = (1 << 22);

    GeometryCache::GeometryCache(graphics::Device& device)
    {
        m_vertexBuffer = device.createBuffer(desc::Buffer()
            .elements(VertexBufferSize)
            .format<Vertex>()
            .structured(true)
            .usage(desc::Usage::GpuReadWrite));
        m_vertexBufferSRV = device.createBufferView(m_vertexBuffer,
            desc::BufferView(m_vertexBuffer.descriptor()).type(desc::ViewType::SRV));

        m_indexBuffer = device.createBuffer(desc::Buffer()
            .elements(IndexBufferSize)
            .format<uint32_t>()
            .type(desc::BufferType::Index)
            .usage(desc::Usage::GpuReadWrite));
    }

    int2 GeometryCache::preloadMesh(const Mesh& mesh)
    {
        int vertexOffset = static_cast<int>(m_vertices.size());
        m_vertices.insert(m_vertices.end(), mesh.vertices().begin(), mesh.vertices().end());

        int indexOffset = static_cast<int>(m_indices.size());
        for (uint32_t i = 0; i < mesh.indices().size(); i++)
        {
            uint32_t index = vertexOffset + mesh.indices()[i];
            m_indices.emplace_back(index);
        }

        return { indexOffset, static_cast<int>(mesh.indices().size()) };
    }

    void GeometryCache::updateGPUBuffers(graphics::CommandBuffer& gfx)
    {
        // TODO: Better logic here
        gfx.update(m_vertexBuffer, vectorAsByteRange(m_vertices));
        gfx.update(m_indexBuffer, vectorAsByteRange(m_indices));
    }
}
