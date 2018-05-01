#include "BufferImpl.hpp"
#include "DeviceImpl.hpp"
#include "../Errors.hpp"

namespace graphics
{
	BufferImpl::BufferImpl(DeviceImpl& device, const desc::Buffer& desc) :
		m_descriptor(desc)
	{
		D3D11_BUFFER_DESC dxdesc;

		dxdesc.ByteWidth = desc.descriptor().stride * desc.descriptor().elements;

		setUsageFlags(desc.descriptor().usage,
					  dxdesc.Usage, dxdesc.CPUAccessFlags, dxdesc.BindFlags);

		switch(desc.descriptor().type)
		{
		case desc::BufferType::Vertex:
			dxdesc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
			break;
		case desc::BufferType::Index:
			dxdesc.BindFlags |= D3D11_BIND_INDEX_BUFFER;
			break;
		case desc::BufferType::Constant:
			dxdesc.BindFlags |= D3D11_BIND_CONSTANT_BUFFER;
			break;
		case desc::BufferType::Regular:
		default:
			break;
		}

		if (desc.descriptor().usage != desc::Usage::GpuToCpuReadback)
		{
			dxdesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}

		dxdesc.MiscFlags = 0;
		if (desc.descriptor().indirectArgs)
		{
			dxdesc.MiscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		}
		if (desc.descriptor().raw)
		{
			dxdesc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		}
		if (desc.descriptor().structured)
		{
			dxdesc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			dxdesc.StructureByteStride = desc.descriptor().stride;
		}
		else
		{
			dxdesc.StructureByteStride = 0;
		}

		HRESULT hr = device.m_device->CreateBuffer(&dxdesc, NULL, &m_buffer);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateBuffer() failed!"), _T("Error"), NULL);
			m_buffer = nullptr;
		}
	}

	BufferImpl::~BufferImpl()
	{
		SAFE_RELEASE(m_buffer);
	}
}
