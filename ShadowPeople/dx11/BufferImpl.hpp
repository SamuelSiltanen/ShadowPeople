#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"

namespace graphics
{
	class DeviceImpl;

	class BufferImpl
	{
	public:
		BufferImpl(DeviceImpl& device, const desc::Buffer& desc);
		~BufferImpl();

		NO_COPY_CLASS(BufferImpl);

		const desc::Buffer& descriptor() const { return m_descriptor; }
	private:
		friend class BufferViewImpl;
		friend class CommandBufferImpl;

		ID3D11Buffer*	m_buffer;
		desc::Buffer	m_descriptor;
	};
}
