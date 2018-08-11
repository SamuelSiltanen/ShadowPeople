#include "BufferViewImpl.hpp"
#include "DeviceImpl.hpp"
#include "BufferImpl.hpp"
#include "../Errors.hpp"

namespace graphics
{
	BufferViewImpl::BufferViewImpl(DeviceImpl& device, const BufferImpl& buffer, const desc::BufferView::Descriptor& desc) :
		m_descriptor(desc)
	{
		switch (desc.type)
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

	void BufferViewImpl::createSRV(DeviceImpl& device, const desc::BufferView::Descriptor& desc, const BufferImpl& buffer)
	{
		SP_ASSERT(!(desc.raw && !buffer.descriptor().raw),
			"Cannot create a raw view, if the buffer is not created with raw view enabled.");

		D3D11_SHADER_RESOURCE_VIEW_DESC dxdesc;
		if (desc.raw)
		{
			dxdesc.ViewDimension			= D3D11_SRV_DIMENSION_BUFFEREX;
			dxdesc.BufferEx.FirstElement	= desc.firstElement;
			dxdesc.BufferEx.NumElements		= desc.numElements;
			dxdesc.BufferEx.Flags			= D3D11_BUFFEREX_SRV_FLAG_RAW;
		}
		else
		{
			dxdesc.ViewDimension			= D3D11_SRV_DIMENSION_BUFFER;
			dxdesc.Buffer.FirstElement		= desc.firstElement;
			dxdesc.Buffer.NumElements		= desc.numElements;
		}

		dxdesc.Format = dxgiFormat(buffer.descriptor().format);

		HRESULT hr = device.device()->CreateShaderResourceView(buffer.m_buffer, &dxdesc,
						(ID3D11ShaderResourceView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateShaderResourceView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}

        auto name = buffer.descriptor().name;
        name.append(" SRV");
        m_view->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
	}

	void BufferViewImpl::createUAV(DeviceImpl& device, const desc::BufferView::Descriptor& desc, const BufferImpl& buffer)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC dxdesc;

		dxdesc.ViewDimension		= D3D11_UAV_DIMENSION_BUFFER;

		dxdesc.Buffer.FirstElement	= desc.firstElement;
		dxdesc.Buffer.NumElements	= desc.numElements;
		dxdesc.Format				= dxgiFormat(buffer.descriptor().format);

		if (desc.raw)
		{
			dxdesc.Buffer.Flags		= D3D11_BUFFER_UAV_FLAG_RAW;
			// Force correct format
			dxdesc.Format			= DXGI_FORMAT_R32_TYPELESS;
		}
		else if (desc.append)
		{
			SP_ASSERT(buffer.descriptor().structured,
					  "Append buffers must be created with structured flag.");
			dxdesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		}
		else if (desc.counter)
		{
			SP_ASSERT(buffer.descriptor().structured,
					  "Counters can be attached to structured buffer only.");
			dxdesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
		}

		HRESULT hr = device.device()->CreateUnorderedAccessView(buffer.m_buffer, &dxdesc,
						(ID3D11UnorderedAccessView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateUnorderedAccessView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}

        auto name = buffer.descriptor().name;
        name.append(" UAV");
        m_view->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
	}
}
