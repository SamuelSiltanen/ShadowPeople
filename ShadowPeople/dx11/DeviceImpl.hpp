#pragma once

#include <d3d11.h>
#include <memory>

#include "DX11Utils.hpp"

namespace graphics
{
	class CommandBufferImpl;
	class TextureImpl;

	class DeviceImpl
	{
	public:
		DeviceImpl(HWND hWnd, int2 screenSize);
		~DeviceImpl();

		NO_COPY_CLASS(DeviceImpl)

		void submit(CommandBufferImpl& gfx);
		void present(int syncInterval);

		int2 swapChainSize();
	private:
		friend class TextureImpl;
		friend class TextureViewImpl;
		friend class BufferImpl;
		friend class BufferViewImpl;
		friend class SamplerImpl;
		friend class CommandBufferImpl;
		friend class ComputePipelineImpl;
		friend class GraphicsPipelineImpl;
		friend class ShaderManagerImpl;

		std::shared_ptr<TextureImpl> getBackBuffer();

		IDXGISwapChain*			m_swapChain;
		ID3D11Device*			m_device;
		ID3D11DeviceContext*	m_context;
	};
}