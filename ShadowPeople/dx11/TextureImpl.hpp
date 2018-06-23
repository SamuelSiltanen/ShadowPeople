#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"

namespace graphics
{
	class DeviceImpl;

	class TextureImpl
	{
	public:
		TextureImpl(DeviceImpl& device, const desc::Texture& desc);
		TextureImpl(ID3D11Texture2D* pTexture);
		~TextureImpl();

		NO_COPY_CLASS(TextureImpl)

		const desc::Texture::Descriptor& descriptor() const { return m_descriptor.descriptor(); }
	private:
		friend class TextureViewImpl;
		friend class CommandBufferImpl;

		void createTexture1D(DeviceImpl& device, const desc::Texture& desc);
		void createTexture2D(DeviceImpl& device, const desc::Texture& desc);
		void createTexture3D(DeviceImpl& device, const desc::Texture& desc);

		ID3D11Resource*	m_texture;
		desc::Texture	m_descriptor;
	};
}
