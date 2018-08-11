#include "TextureImpl.hpp"
#include "DeviceImpl.hpp"
#include "../Errors.hpp"

#include <d3dcommon.h>

namespace graphics
{
	TextureImpl::TextureImpl(DeviceImpl& device, const desc::Texture& desc) :
		m_descriptor(desc)
	{
		HRESULT hr = S_OK;
		switch (desc.descriptor().dimension)
		{
		case desc::Dimension::Texture1D:
			createTexture1D(device, desc);
			break;
		case desc::Dimension::Texture3D:
			createTexture3D(device, desc);
			break;
		case desc::Dimension::Texture2D:
		case desc::Dimension::TextureCube:
		default:
			createTexture2D(device, desc);
			break;
		}

        auto name = desc.descriptor().name;
        m_texture->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());
	}

	TextureImpl::TextureImpl(ID3D11Texture2D* pTexture)
	{
		D3D11_TEXTURE2D_DESC dxdesc;
		pTexture->GetDesc(&dxdesc);

		// TODO: The ownership of the texture must be thought carefully
		//		 What happens, when the back buffer is discarded?
		m_texture = pTexture;

		m_descriptor = desc::Texture()
			.dimension(desc::Dimension::Texture2D)
			.width(dxdesc.Width)
			.height(dxdesc.Height)
			.depth(1)			
			.format(descFormat(dxdesc.Format))
			.arraySize(dxdesc.ArraySize)
			.mipLevels(dxdesc.MipLevels)
			.multisampling(descMultisampling(dxdesc.SampleDesc))
			.usage(desc::Usage::GpuReadWrite);	// DX11 allows writing the backbuffer in with a UAV
	}

	TextureImpl::~TextureImpl()
	{
		SAFE_RELEASE(m_texture);
	}

	void TextureImpl::createTexture1D(DeviceImpl& device, const desc::Texture& desc)
	{
		D3D11_TEXTURE1D_DESC dxdesc;

		dxdesc.Width		= desc.descriptor().width;
		dxdesc.MipLevels	= desc.descriptor().mipLevels;
		dxdesc.ArraySize	= desc.descriptor().arraySize;
		dxdesc.Format		= dxgiFormat(desc.descriptor().format);

		setUsageFlags(desc.descriptor().usage,
					  dxdesc.Usage, dxdesc.CPUAccessFlags, dxdesc.BindFlags);

		dxdesc.MiscFlags	= 0;

		D3D11_SUBRESOURCE_DATA dxInit;
		auto& init = desc.descriptor().initialData;
		if (init.dataPtr)
		{
			dxInit.pSysMem			= init.dataPtr;
			dxInit.SysMemPitch		= init.pitch;
			dxInit.SysMemSlicePitch	= init.pitchDepth;
		}

		HRESULT hr = device.device()->CreateTexture1D(&dxdesc, init.dataPtr ? &dxInit : NULL,
													  reinterpret_cast<ID3D11Texture1D**>(&m_texture));
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateTexture1D() failed!"), _T("Error"), NULL);
			m_texture = nullptr;
		}
	}

	void TextureImpl::createTexture2D(DeviceImpl& device, const desc::Texture& desc)
	{
		D3D11_TEXTURE2D_DESC dxdesc;

		dxdesc.Width		= desc.descriptor().width;
		dxdesc.Height		= desc.descriptor().height;
		dxdesc.MipLevels	= desc.descriptor().mipLevels;
		dxdesc.ArraySize	= desc.descriptor().arraySize;

		desc::Format format = desc.descriptor().format;
		if (format.descriptor().channels == desc::FormatChannels::Depth)
		{
			// Because the depth buffer can be used as a shader resource,
			// we create it as a typeless format
            format = desc::Format(desc::FormatChannels::R, format.descriptor().bytes, desc::FormatType::Typeless);
		}

		dxdesc.Format		= dxgiFormat(format);
		dxdesc.SampleDesc	= dxgiSampleDesc(desc.descriptor().multisampling);

		setUsageFlags(desc.descriptor().usage,
					  dxdesc.Usage, dxdesc.CPUAccessFlags, dxdesc.BindFlags);

		dxdesc.MiscFlags = 0;
		if (desc.descriptor().dimension == desc::Dimension::TextureCube)
		{
			dxdesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			dxdesc.ArraySize *= 6;
		}

		D3D11_SUBRESOURCE_DATA dxInit;
		auto& init = desc.descriptor().initialData;
		if (init.dataPtr)
		{
			dxInit.pSysMem			= init.dataPtr;
			dxInit.SysMemPitch		= init.pitch;
			dxInit.SysMemSlicePitch	= init.pitchDepth;
		}

		HRESULT hr = device.device()->CreateTexture2D(&dxdesc, init.dataPtr ? &dxInit : NULL,
													  reinterpret_cast<ID3D11Texture2D**>(&m_texture));
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateTexture2D() failed!"), _T("Error"), NULL);
			m_texture = nullptr;
		}
	}

	void TextureImpl::createTexture3D(DeviceImpl& device, const desc::Texture& desc)
	{
		D3D11_TEXTURE3D_DESC dxdesc;

		dxdesc.Width		= desc.descriptor().width;
		dxdesc.Height		= desc.descriptor().height;
		dxdesc.Depth		= desc.descriptor().depth;
		dxdesc.MipLevels	= desc.descriptor().mipLevels;
		dxdesc.Format		= dxgiFormat(desc.descriptor().format);

		setUsageFlags(desc.descriptor().usage,
					  dxdesc.Usage, dxdesc.CPUAccessFlags, dxdesc.BindFlags);

		dxdesc.MiscFlags	= 0;

		D3D11_SUBRESOURCE_DATA dxInit;
		auto& init = desc.descriptor().initialData;
		if (init.dataPtr)
		{
			dxInit.pSysMem			= init.dataPtr;
			dxInit.SysMemPitch		= init.pitch;
			dxInit.SysMemSlicePitch	= init.pitchDepth;
		}
		
		HRESULT hr = device.device()->CreateTexture3D(&dxdesc, init.dataPtr ? &dxInit : NULL,
													  reinterpret_cast<ID3D11Texture3D**>(&m_texture));
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateTexture3D() failed!"), _T("Error"), NULL);
			m_texture = nullptr;
		}
	}
}
