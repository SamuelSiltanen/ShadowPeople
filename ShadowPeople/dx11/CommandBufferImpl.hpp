#pragma once

#include <d3d11.h>
#include <cstdint>

#include "DX11Utils.hpp"
#include "../Types.hpp"
#include "../Errors.hpp"

namespace graphics
{
    class DeviceImpl;
    class TextureImpl;
    class TextureViewImpl;
    class BufferImpl;
    class MappingImpl;
    class GraphicsPipelineImpl;
    class ComputePipelineImpl;
    class ResourceViewImpl;

    class CommandBufferImpl
    {
    public:
        CommandBufferImpl(DeviceImpl& device);

        NO_COPY_CLASS(CommandBufferImpl);

        void clear(TextureViewImpl& view, float r, float g, float b, float a);
        void clear(TextureViewImpl& view, uint32_t r, uint32_t g, uint32_t b, uint32_t a);
        void clear(TextureViewImpl& view, float depth);
        void clear(TextureViewImpl& view, float depth, uint8_t stencil);
        void clear(TextureViewImpl& view, uint8_t stencil);

        void copy(TextureImpl& dst, const TextureImpl& src);
        void copy(TextureImpl& dst, const TextureImpl& src,
                  int3 dstCorner, Rect<int, 3> srcRect,
                  Subresource dstSubresource, Subresource srcSubresource);

        void copyToBackBuffer(const TextureImpl& src);
        template<typename T>
        void copyToConstantBuffer(BufferImpl& dst, Range<const T> src)
        {
            D3D11_MAPPED_SUBRESOURCE mapping;
            HRESULT hr = m_context.Map(dst.m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapping);
            SP_ASSERT_HR(hr, ERROR_CODE_MAP_DISCARD_FAILED);
            memcpy(mapping.pData, src.begin(), src.byteSize());
            m_context.Unmap(dst.m_buffer, 0);
        }

        void update(TextureImpl& dst, Range<const uint8_t> cpuData, Subresource dstSubresource = Subresource());
        void update(BufferImpl& dst, Range<const uint8_t> cpuData);

        void setRenderTargets();
        void setRenderTargets(TextureViewImpl& rtv);
        void setRenderTargets(TextureViewImpl& dsv, TextureViewImpl& rtv);

        void setVertexBuffer();
        void setVertexBuffer(BufferImpl& buffer, GraphicsPipelineImpl& pipeline);
        void setIndexBuffer();
        void setIndexBuffer(BufferImpl& buffer);

        std::shared_ptr<MappingImpl> map(BufferImpl& buf);

        void dispatch(ComputePipelineImpl& pipeline,
                      uint32_t threadGroupsX, uint32_t threadGroupsY, uint32_t threadGroupsZ);
        void dispatchIndirect(ComputePipelineImpl& pipeline,
                              const BufferImpl& argsBuffer, uint32_t argsOffset);

        void draw(GraphicsPipelineImpl& pipeline, uint32_t vertexCount, uint32_t startVertexOffset);
        void drawIndexed(GraphicsPipelineImpl& pipeline, uint32_t indexCount, uint32_t startIndexOffset,
                         uint32_t vertexOffset);
        void drawInstanced(GraphicsPipelineImpl& pipeline, uint32_t vertexCountPerInstance, uint32_t instanceCount,
                           uint32_t startVextexOffset, uint32_t startInstanceOffset);
        void drawIndexedInstanced(GraphicsPipelineImpl& pipeline, uint32_t vertexCountPerInstance,
                                  uint32_t instanceCount, uint32_t startVextexOffset, uint32_t vertexOffset,
                                  uint32_t startInstanceOffset);
        void drawInstancedIndirect(GraphicsPipelineImpl& pipeline, const BufferImpl& argsBuffer,
                                   uint32_t argsOffset);
        void drawIndexedInstancedIndirect(GraphicsPipelineImpl& pipeline, const BufferImpl& argsBuffer,
                                          uint32_t argsOffset);
    private:
        bool isSimilarForCopy(const TextureImpl& dst, const TextureImpl& src);

        void setupResources(ComputePipelineImpl& pipeline);
        void setupResources(GraphicsPipelineImpl& pipeline);
        void clearResources(ComputePipelineImpl& pipeline);
        void clearResources(GraphicsPipelineImpl& pipeline);

        void setViewport();

        void setDepthStencilState(GraphicsPipelineImpl& pipeline);
        void setBlendState(GraphicsPipelineImpl& pipeline);
        void setPrimitiveTopology(GraphicsPipelineImpl& pipeline);
        void setRasterizerState(GraphicsPipelineImpl& pipeline);

        struct RenderTargets
        {
            std::vector<const ResourceViewImpl*>    rtvs;
            const ResourceViewImpl*                 dsv;
            int2                                    renderTargetSize;
        };

        RenderTargets           m_currentRenderTargets;

        ID3D11DeviceContext&    m_context;

        // TODO: Perhaps not the whole device - we don't want to allow resource creation here
        DeviceImpl&             m_device;
    };
}
