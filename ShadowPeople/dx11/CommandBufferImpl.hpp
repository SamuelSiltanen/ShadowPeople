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
	class ComputePipelineImpl;

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

		void dispatch(ComputePipelineImpl& pipeline,
					  uint32_t threadGroupsX, uint32_t threadGroupsY, uint32_t threadGroupsZ);
		void dispatchIndirect(ComputePipelineImpl& pipeline,
							  const BufferImpl& argsBuffer, uint32_t argsOffset);
	private:
		bool isSimilarForCopy(const TextureImpl& dst, const TextureImpl& src);

		void setupResources(ComputePipelineImpl& pipeline);
		void clearResources(ComputePipelineImpl& pipeline);

		ID3D11DeviceContext&	m_context;

		// TODO: Perhaps not the whole device - we don't want to allow resource creation here
		DeviceImpl&				m_device;
	};
}
