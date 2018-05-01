#include "BufferViewImpl.hpp"
#include "DeviceImpl.hpp"
#include "BufferImpl.hpp"
#include "../Errors.hpp"

namespace graphics
{
	BufferViewImpl::BufferViewImpl(DeviceImpl& device, const desc::BufferView& desc, const BufferImpl& buffer) :
		m_descriptor(desc)
	{
		switch (desc.descriptor().type)
		{
		case desc::ViewType::UAV:
			createUAV(device, desc, buffer);
			break;
		case desc::ViewType::RTV:
		case desc::ViewType::DSV:
			SP_ASSERT(false, "BufferView cannot be RTV or DSV. Use UAV to access the buffer.");
		case desc::ViewType::SRV:
		default:
			createSRV(device, desc, buffer);
			break;
		}
	}

	BufferViewImpl::~BufferViewImpl()
	{
		SAFE_RELEASE(m_view);
	}

	void BufferViewImpl::createSRV(DeviceImpl& device, const desc::BufferView& desc, const BufferImpl& buffer)
	{
		SP_ASSERT(desc.descriptor().raw && !buffer.m_descriptor.descriptor().raw,
			"Cannot create a raw view, if the buffer is not created with raw view enabled.");

		D3D11_SHADER_RESOURCE_VIEW_DESC dxdesc;
		if (desc.descriptor().raw)
		{
			dxdesc.ViewDimension			= D3D11_SRV_DIMENSION_BUFFEREX;
			dxdesc.BufferEx.FirstElement	= desc.descriptor().firstElement;
			dxdesc.BufferEx.NumElements		= desc.descriptor().numElements;
			dxdesc.BufferEx.Flags			= D3D11_BUFFEREX_SRV_FLAG_RAW;
		}
		else
		{
			dxdesc.ViewDimension			= D3D11_SRV_DIMENSION_BUFFER;
			dxdesc.Buffer.FirstElement		= desc.descriptor().firstElement;
			dxdesc.Buffer.NumElements		= desc.descriptor().numElements;
		}

		dxdesc.Format = dxgiFormat(buffer.m_descriptor.descriptor().format);

		HRESULT hr = device.m_device->CreateShaderResourceView(buffer.m_buffer, &dxdesc,
						(ID3D11ShaderResourceView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateShaderResourceView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}
	}

	void BufferViewImpl::createUAV(DeviceImpl& device, const desc::BufferView& desc, const BufferImpl& buffer)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC dxdesc;

		dxdesc.ViewDimension		= D3D11_UAV_DIMENSION_BUFFER;

		dxdesc.Buffer.FirstElement	= desc.descriptor().firstElement;
		dxdesc.Buffer.NumElements	= desc.descriptor().numElements;
		dxdesc.Format				= dxgiFormat(buffer.m_descriptor.descriptor().format);

		if (desc.descriptor().raw)
		{
			dxdesc.Buffer.Flags		= D3D11_BUFFER_UAV_FLAG_RAW;
			// Force correct format
			dxdesc.Format			= DXGI_FORMAT_R32_TYPELESS;
		}
		else if (desc.descriptor().append)
		{
			SP_ASSERT(buffer.m_descriptor.descriptor().structured, "Append buffers must be created with structured flag.");
			dxdesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		}
		else if (desc.descriptor().counter)
		{
			SP_ASSERT(buffer.m_descriptor.descriptor().structured, "Counters can be attached to structured buffer only.");
			dxdesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
		}

		HRESULT hr = device.m_device->CreateUnorderedAccessView(buffer.m_buffer, &dxdesc,
						(ID3D11UnorderedAccessView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateUnorderedAccessView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}
	}
}
