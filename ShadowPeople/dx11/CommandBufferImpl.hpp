#pragma once

#include <d3d11.h>
#include <cstdint>

#include "DX11Utils.hpp"
#include "../Types.hpp"

namespace graphics
{
	class DeviceImpl;
	class TextureImpl;
	class TextureViewImpl;
	class BufferImpl;
	class ComputePipelineImpl;

	struct ShaderResources;

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

		void dispatch(const ComputePipelineImpl& pipeline, const ShaderResources& resources,
					  uint32_t threadGroupsX, uint32_t threadGroupsY, uint32_t threadGroupsZ);
		void dispatchIndirect(const ComputePipelineImpl& pipeline, const ShaderResources& resources,
							  const BufferImpl& argsBuffer, uint32_t argsOffset);
	private:
		bool isSimilarForCopy(const TextureImpl& dst, const TextureImpl& src);

		ID3D11DeviceContext&	m_context;

		// TODO: Perhaps not the whole device - we don't want to allow resource creation here
		DeviceImpl&				m_device;
	};
}
