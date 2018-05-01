#include "TextureViewImpl.hpp"
#include "DeviceImpl.hpp"
#include "TextureImpl.hpp"
#include "../Errors.hpp"

namespace graphics
{
	TextureViewImpl::TextureViewImpl(DeviceImpl& device,
									 const desc::TextureView& desc,
									 const TextureImpl& texture) :
		m_descriptor(desc)
	{
		switch (desc.descriptor().type)
		{
		case desc::ViewType::UAV:
			createUAV(device, desc, texture);
			break;
		case desc::ViewType::RTV:
			createRTV(device, desc, texture);
			break;
		case desc::ViewType::DSV:
			createDSV(device, desc, texture);
			break;
		case desc::ViewType::SRV:
		default:
			createSRV(device, desc, texture);
			break;
		}
	}

	TextureViewImpl::~TextureViewImpl()
	{
		SAFE_RELEASE(m_view);
	}

	void TextureViewImpl::createSRV(DeviceImpl& device,
									const desc::TextureView& desc,
									const TextureImpl& texture)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC dxdesc;

		dxdesc.Format			= dxgiFormat(desc.descriptor().format);
		switch (desc.descriptor().dimension)
		{
		case desc::Dimension::Texture1D:
			if (desc.descriptor().numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE1D;
				dxdesc.Texture1D.MostDetailedMip		= desc.descriptor().firstMip;
				dxdesc.Texture1D.MipLevels				= desc.descriptor().numMips;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
				dxdesc.Texture1DArray.MostDetailedMip	= desc.descriptor().firstMip;
				dxdesc.Texture1DArray.MipLevels			= desc.descriptor().numMips;
				dxdesc.Texture1DArray.FirstArraySlice	= desc.descriptor().firstSlice;
				dxdesc.Texture1DArray.ArraySize			= desc.descriptor().numSlices;
			}
			break;
		case desc::Dimension::Texture3D:
			dxdesc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURE3D;
			dxdesc.Texture3D.MostDetailedMip			= desc.descriptor().firstMip;
			dxdesc.Texture3D.MipLevels					= desc.descriptor().numMips;
			break;
		case desc::Dimension::TextureCube:
			if (desc.descriptor().numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURECUBE;
				dxdesc.TextureCube.MostDetailedMip		= desc.descriptor().firstMip;
				dxdesc.TextureCube.MipLevels			= desc.descriptor().numMips;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
				dxdesc.TextureCubeArray.MostDetailedMip = desc.descriptor().firstMip;
				dxdesc.TextureCubeArray.MipLevels		= desc.descriptor().numMips;
				dxdesc.TextureCubeArray.First2DArrayFace = desc.descriptor().firstSlice * 6;
				dxdesc.TextureCubeArray.NumCubes		= desc.descriptor().numSlices;
			}
			break;
		case desc::Dimension::Texture2D:
		default:
			if (texture.m_descriptor.descriptor().multisampling != desc::Multisampling::None)
			{
				if (desc.descriptor().numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
					dxdesc.Texture2DMSArray.FirstArraySlice = desc.descriptor().firstSlice;
					dxdesc.Texture2DMSArray.ArraySize		= desc.descriptor().numSlices;
				}
			}
			else
			{
				if (desc.descriptor().numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2D;
					dxdesc.Texture2D.MostDetailedMip		= desc.descriptor().firstMip;
					dxdesc.Texture2D.MipLevels				= desc.descriptor().numMips;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					dxdesc.Texture2DArray.MostDetailedMip	= desc.descriptor().firstMip;
					dxdesc.Texture2DArray.MipLevels			= desc.descriptor().numMips;
					dxdesc.Texture2DArray.FirstArraySlice	= desc.descriptor().firstSlice;
					dxdesc.Texture2DArray.ArraySize			= desc.descriptor().numSlices;
				}
			}
			break;
		}

		HRESULT hr = device.m_device->CreateShaderResourceView(texture.m_texture, &dxdesc,
						(ID3D11ShaderResourceView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateShaderResourceView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}
	}

	void TextureViewImpl::createUAV(DeviceImpl& device,
									const desc::TextureView& desc,
									const TextureImpl& texture)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC dxdesc;

		SP_ASSERT(texture.m_descriptor.descriptor().usage == desc::Usage::GpuReadWrite,
			"Cannot create UAV for a texture that was not created with Usage::GpuReadWrite.");

		SP_ASSERT(texture.m_descriptor.descriptor().multisampling == desc::Multisampling::None,
			"Cannot create UAV for a multisampled texture. It can be written only in a pixel shader.");

		dxdesc.Format			= dxgiFormat(desc.descriptor().format);
		switch (desc.descriptor().dimension)
		{
		case desc::Dimension::Texture1D:
			if (desc.descriptor().numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_UAV_DIMENSION_TEXTURE1D;
				dxdesc.Texture1D.MipSlice				= desc.descriptor().firstMip;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
				dxdesc.Texture1DArray.MipSlice			= desc.descriptor().firstMip;
				dxdesc.Texture1DArray.FirstArraySlice	= desc.descriptor().firstSlice;
				dxdesc.Texture1DArray.ArraySize			= desc.descriptor().numSlices;
			}
			break;
		case desc::Dimension::Texture3D:
			dxdesc.ViewDimension						= D3D11_UAV_DIMENSION_TEXTURE3D;
			dxdesc.Texture3D.MipSlice					= desc.descriptor().firstMip;
			dxdesc.Texture3D.FirstWSlice				= desc.descriptor().firstSlice;
			dxdesc.Texture3D.WSize						= desc.descriptor().numSlices;
			break;
		case desc::Dimension::TextureCube:
			SP_ASSERT(false, "Cannot create UAV for a TextureCube. Use a Texture2DArray to access the cube.");
			break;
		case desc::Dimension::Texture2D:
		default:
			if (desc.descriptor().numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_UAV_DIMENSION_TEXTURE2D;
				dxdesc.Texture2D.MipSlice				= desc.descriptor().firstMip;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
				dxdesc.Texture2DArray.MipSlice			= desc.descriptor().firstMip;
				dxdesc.Texture2DArray.FirstArraySlice	= desc.descriptor().firstSlice;
				dxdesc.Texture2DArray.ArraySize			= desc.descriptor().numSlices;
			}
			break;
		}

		HRESULT hr = device.m_device->CreateUnorderedAccessView(texture.m_texture, &dxdesc,
						(ID3D11UnorderedAccessView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateUnorderedAccessView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}
	}

	void TextureViewImpl::createRTV(DeviceImpl& device,
									const desc::TextureView& desc,
									const TextureImpl& texture)
	{
		D3D11_RENDER_TARGET_VIEW_DESC dxdesc;

		SP_ASSERT(texture.m_descriptor.descriptor().usage == desc::Usage::RenderTarget,
			"Cannot create RTV for a texture that was not created with Usage::RenderTarget.");

		dxdesc.Format			= dxgiFormat(desc.descriptor().format);
		switch (desc.descriptor().dimension)
		{
		case desc::Dimension::Texture1D:
			if (desc.descriptor().numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE1D;
				dxdesc.Texture1D.MipSlice				= desc.descriptor().firstMip;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
				dxdesc.Texture1DArray.MipSlice			= desc.descriptor().firstMip;
				dxdesc.Texture1DArray.FirstArraySlice	= desc.descriptor().firstSlice;
				dxdesc.Texture1DArray.ArraySize			= desc.descriptor().numSlices;
			}
			break;
		case desc::Dimension::Texture3D:
			dxdesc.ViewDimension						= D3D11_RTV_DIMENSION_TEXTURE3D;
			dxdesc.Texture3D.MipSlice					= desc.descriptor().firstMip;
			dxdesc.Texture3D.FirstWSlice				= desc.descriptor().firstSlice;
			dxdesc.Texture3D.WSize						= desc.descriptor().numSlices;
			break;
		case desc::Dimension::TextureCube:
			SP_ASSERT(false, "Cannot create UAV for a TextureCube. Use a Texture2DArray to access the cube.");
			break;
		case desc::Dimension::Texture2D:
		default:
			if (texture.m_descriptor.descriptor().multisampling != desc::Multisampling::None)
			{
				if (desc.descriptor().numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
					dxdesc.Texture2DMSArray.FirstArraySlice = desc.descriptor().firstSlice;
					dxdesc.Texture2DMSArray.ArraySize		= desc.descriptor().numSlices;
				}
			}
			else
			{
				if (desc.descriptor().numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2D;
					dxdesc.Texture2D.MipSlice				= desc.descriptor().firstMip;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
					dxdesc.Texture2DArray.MipSlice			= desc.descriptor().firstMip;
					dxdesc.Texture2DArray.FirstArraySlice	= desc.descriptor().firstSlice;
					dxdesc.Texture2DArray.ArraySize			= desc.descriptor().numSlices;
				}
			}
			break;
		}

		HRESULT hr = device.m_device->CreateRenderTargetView(texture.m_texture, &dxdesc,
						(ID3D11RenderTargetView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateRenderTargetView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}
	}

	void TextureViewImpl::createDSV(DeviceImpl& device,
									const desc::TextureView& desc,
									const TextureImpl& texture)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dxdesc;

		SP_ASSERT(texture.m_descriptor.descriptor().usage == desc::Usage::DepthBuffer,
			"Cannot create DSV for a texture that was not created with Usage::DepthStencil.");

		dxdesc.Format			= dxgiFormat(desc.descriptor().format);
		switch (desc.descriptor().dimension)
		{
		case desc::Dimension::Texture1D:
			if (desc.descriptor().numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE1D;
				dxdesc.Texture1D.MipSlice				= desc.descriptor().firstMip;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
				dxdesc.Texture1DArray.MipSlice			= desc.descriptor().firstMip;
				dxdesc.Texture1DArray.FirstArraySlice	= desc.descriptor().firstSlice;
				dxdesc.Texture1DArray.ArraySize			= desc.descriptor().numSlices;
			}
			break;
		case desc::Dimension::Texture3D:
			SP_ASSERT(false, "Cannot create DSV for a Texture3D.");
			break;
		case desc::Dimension::TextureCube:
			SP_ASSERT(false, "Cannot create DSV for a TextureCube. Use a Texture2DArray to access the cube.");
			break;
		case desc::Dimension::Texture2D:
		default:
			if (texture.m_descriptor.descriptor().multisampling != desc::Multisampling::None)
			{
				if (desc.descriptor().numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
					dxdesc.Texture2DMSArray.FirstArraySlice = desc.descriptor().firstSlice;
					dxdesc.Texture2DMSArray.ArraySize		= desc.descriptor().numSlices;
				}
			}
			else
			{
				if (desc.descriptor().numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE2D;
					dxdesc.Texture2D.MipSlice				= desc.descriptor().firstMip;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
					dxdesc.Texture2DArray.MipSlice			= desc.descriptor().firstMip;
					dxdesc.Texture2DArray.FirstArraySlice	= desc.descriptor().firstSlice;
					dxdesc.Texture2DArray.ArraySize			= desc.descriptor().numSlices;
				}
			}
			break;
		}
		dxdesc.Flags = 0;

		HRESULT hr = device.m_device->CreateDepthStencilView(texture.m_texture, &dxdesc,
						(ID3D11DepthStencilView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateDepthStencilView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}
	}
}
