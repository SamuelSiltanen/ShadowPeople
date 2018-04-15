#pragma once

#include <d3d11.h>
#include <cstdint>
#include <memory>

#include "Descriptors.hpp"
#include "Types.hpp"

// We don't want to allow copying the implementation classes, 
// because that could mess up the reference counting by smart pointers
#define NO_COPY_CLASS(X)	X::X(const X&)			= delete; \
							X& operator=(const X&)	= delete;

namespace graphics
{
	class CommandBufferImpl;
	class TextureImpl;

	class DeviceImpl
	{
	public:
		DeviceImpl(HWND hWnd, unsigned width, unsigned height);
		~DeviceImpl();

		NO_COPY_CLASS(DeviceImpl)

		void submit(CommandBufferImpl& gfx);
		void present(int syncInterval);

		std::shared_ptr<TextureImpl> getBackBuffer();
	private:
		friend class TextureImpl;
		friend class TextureViewImpl;
		friend class BufferImpl;
		friend class BufferViewImpl;
		friend class CommandBufferImpl;

		IDXGISwapChain*			m_swapChain;
		ID3D11Device*			m_device;
		ID3D11DeviceContext*	m_context;
	};

	class TextureImpl
	{
	public:
		TextureImpl(DeviceImpl& device, const desc::Texture& desc);
		TextureImpl(ID3D11Texture2D* pTexture);
		~TextureImpl();

		NO_COPY_CLASS(TextureImpl)

		const desc::Texture& descriptor() const { return m_descriptor; }
	private:
		friend class TextureViewImpl;
		friend class CommandBufferImpl;

		void createTexture1D(DeviceImpl& device, const desc::Texture& desc);
		void createTexture2D(DeviceImpl& device, const desc::Texture& desc);
		void createTexture3D(DeviceImpl& device, const desc::Texture& desc);

		DXGI_SAMPLE_DESC setSampleDesc(const desc::Texture& desc);

		ID3D11Resource*	m_texture;
		desc::Texture	m_descriptor;
	};

	class TextureViewImpl
	{
	public:
		TextureViewImpl(DeviceImpl& device, const desc::TextureView& desc, const TextureImpl& texture);
		~TextureViewImpl();

		NO_COPY_CLASS(TextureViewImpl)

		const desc::TextureView& descriptor() { return m_descriptor; }
	private:
		friend class CommandBufferImpl;

		void createSRV(DeviceImpl& device, const desc::TextureView& desc, const TextureImpl& texture);
		void createUAV(DeviceImpl& device, const desc::TextureView& desc, const TextureImpl& texture);
		void createRTV(DeviceImpl& device, const desc::TextureView& desc, const TextureImpl& texture);
		void createDSV(DeviceImpl& device, const desc::TextureView& desc, const TextureImpl& texture);

		ID3D11View*			m_view;
		desc::TextureView	m_descriptor;
	};

	class BufferImpl
	{
	public:
		BufferImpl(DeviceImpl& device, const desc::Buffer& desc);
		~BufferImpl();

		NO_COPY_CLASS(BufferImpl);

		const desc::Buffer& descriptor() const { return m_descriptor; }
	private:
		friend class BufferViewImpl;

		ID3D11Buffer*	m_buffer;
		desc::Buffer	m_descriptor;
	};

	class BufferViewImpl
	{
	public:
		BufferViewImpl(DeviceImpl& device, const desc::BufferView& desc, const BufferImpl& buffer);
		~BufferViewImpl();

		NO_COPY_CLASS(BufferViewImpl)

		const desc::BufferView& descriptor() { return m_descriptor; }
	private:
		void createSRV(DeviceImpl& device, const desc::BufferView& desc, const BufferImpl& buffer);
		void createUAV(DeviceImpl& device, const desc::BufferView& desc, const BufferImpl& buffer);

		ID3D11View*			m_view;
		desc::BufferView	m_descriptor;
	};

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
		void copy(TextureImpl& dst, TextureImpl& src);
		void copy(TextureImpl& dst, TextureImpl& src,
				  int3 dstCorner, Rect<int, 3> srcRect,
				  Subresource dstSubresource, Subresource srcSubresource);
	private:
		ID3D11DeviceContext&	m_context;
	};
}
