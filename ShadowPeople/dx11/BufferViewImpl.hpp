#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"

namespace graphics
{
	class DeviceImpl;

	class BufferImpl;

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
}