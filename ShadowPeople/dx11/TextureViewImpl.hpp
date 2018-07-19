/*
    Copyright 2018 Samuel Siltanen
    TextureViewImpl.hpp
*/

#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"
#include "ResourceViewImpl.hpp"

namespace graphics
{
	class DeviceImpl;
	class TextureImpl;

	class TextureViewImpl : public ResourceViewImpl
	{
	public:
		TextureViewImpl(DeviceImpl& device, const TextureImpl& texture, const desc::TextureView::Descriptor& desc);
		~TextureViewImpl();

		NO_COPY_CLASS(TextureViewImpl)

		const desc::TextureView::Descriptor& descriptor() { return m_descriptor; }
		const TextureImpl& texture() const { return m_texture; }
		const ID3D11View* view() const override { return m_view; }
	private:
		friend class CommandBufferImpl;

		void createSRV(DeviceImpl& device, const desc::TextureView::Descriptor& desc, const TextureImpl& texture);
		void createUAV(DeviceImpl& device, const desc::TextureView::Descriptor& desc, const TextureImpl& texture);
		void createRTV(DeviceImpl& device, const desc::TextureView::Descriptor& desc, const TextureImpl& texture);
		void createDSV(DeviceImpl& device, const desc::TextureView::Descriptor& desc, const TextureImpl& texture);

		ID3D11View*						m_view;
		desc::TextureView::Descriptor	m_descriptor;
		const TextureImpl&				m_texture;
	};
}
