#include "MappingImpl.hpp"

#include "DeviceImpl.hpp"
#include "BufferImpl.hpp"
#include "../Errors.hpp"

namespace graphics
{
	MappingImpl::MappingImpl(DeviceImpl& device, BufferImpl& buffer) :
		m_device(device),
		m_buffer(buffer)
	{
		HRESULT hr = m_device.m_context->Map(buffer.m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &m_mappedResource);
		SP_ASSERT_HR(hr, ERROR_CODE_MAP_DISCARD_FAILED);
	}

	MappingImpl::~MappingImpl()
	{
		m_device.m_context->Unmap(m_buffer.m_buffer, 0);
	}
}
