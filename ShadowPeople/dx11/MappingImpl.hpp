#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"

namespace graphics
{
	class DeviceImpl;
	class BufferImpl;

	class MappingImpl
	{
	public:
		MappingImpl(DeviceImpl& device, BufferImpl& buffer);
		~MappingImpl();

		NO_COPY_CLASS(MappingImpl);

		void* data() { return m_mappedResource.pData; }
	private:
		DeviceImpl&					m_device;
		BufferImpl&					m_buffer;
		D3D11_MAPPED_SUBRESOURCE	m_mappedResource;
	};
}
