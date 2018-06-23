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
		NO_MOVE_CLASS(DeviceImpl)

		void submit(CommandBufferImpl& gfx);
		void present(int syncInterval);

		int2 swapChainSize();

		ID3D11Device*		 device();
		ID3D11DeviceContext* context();

		std::shared_ptr<TextureImpl> getBackBuffer();
	private:
		IDXGISwapChain*			m_swapChain;
		ID3D11Device*			m_device;
		ID3D11DeviceContext*	m_context;
	};
}