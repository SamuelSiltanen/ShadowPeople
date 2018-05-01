#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"

namespace graphics
{
	class DeviceImpl;
	class TextureImpl;

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
}
