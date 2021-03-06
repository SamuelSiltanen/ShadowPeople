#include "BufferImpl.hpp"
#include "DeviceImpl.hpp"
#include "../Errors.hpp"

#include <d3dcommon.h>

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

        // TODO: Assert if usage is IMMUTABLE and there is no initial data

		HRESULT hr = device.device()->CreateBuffer(&dxdesc, NULL, &m_buffer);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateBuffer() failed!"), _T("Error"), NULL);
			m_buffer = nullptr;
		}

        auto name = desc.descriptor().name;
        m_buffer->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
	}

	BufferImpl::~BufferImpl()
	{
		SAFE_RELEASE(m_buffer);
	}
}
