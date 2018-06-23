#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"
#include "ResourceViewImpl.hpp"

namespace graphics
{
	class DeviceImpl;

	class BufferImpl;

	class BufferViewImpl : public ResourceViewImpl
	{
	public:
		BufferViewImpl(DeviceImpl& device, const desc::BufferView::Descriptor& desc, const BufferImpl& buffer);
		~BufferViewImpl();

		NO_COPY_CLASS(BufferViewImpl)

		const desc::BufferView::Descriptor& descriptor() { return m_descriptor; }

		const ID3D11View* view() const override { return m_view; }
	private:
		void createSRV(DeviceImpl& device, const desc::BufferView::Descriptor& desc, const BufferImpl& buffer);
		void createUAV(DeviceImpl& device, const desc::BufferView::Descriptor& desc, const BufferImpl& buffer);

		ID3D11View*						m_view;
		desc::BufferView::Descriptor	m_descriptor;
	};
}