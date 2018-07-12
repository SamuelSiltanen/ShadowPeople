#include "TextureViewImpl.hpp"
#include "DeviceImpl.hpp"
#include "TextureImpl.hpp"
#include "../Errors.hpp"

namespace graphics
{
	TextureViewImpl::TextureViewImpl(DeviceImpl& device,
									 const TextureImpl& texture,
									 const desc::TextureView::Descriptor& desc) :
		m_descriptor(desc),
		m_texture(texture)
	{
		switch (desc.type)
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
									const desc::TextureView::Descriptor& desc,
									const TextureImpl& texture)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC dxdesc;

		desc::Format format		= desc.format;
		if (format.descriptor().channels == desc::FormatChannels::Depth)
		{
			// Because the depth buffer can be used as a shader resource,
			// we replace the depth format with an equivalent regular format
			format = desc::Format(desc::FormatChannels::R, format.descriptor().bytes, format.descriptor().type);                
		}

		dxdesc.Format			= dxgiFormat(format);
		switch (desc.dimension)
		{
		case desc::Dimension::Texture1D:
			if (desc.numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE1D;
				dxdesc.Texture1D.MostDetailedMip		= desc.firstMip;
				dxdesc.Texture1D.MipLevels				= desc.numMips;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
				dxdesc.Texture1DArray.MostDetailedMip	= desc.firstMip;
				dxdesc.Texture1DArray.MipLevels			= desc.numMips;
				dxdesc.Texture1DArray.FirstArraySlice	= desc.firstSlice;
				dxdesc.Texture1DArray.ArraySize			= desc.numSlices;
			}
			break;
		case desc::Dimension::Texture3D:
			dxdesc.ViewDimension						= D3D11_SRV_DIMENSION_TEXTURE3D;
			dxdesc.Texture3D.MostDetailedMip			= desc.firstMip;
			dxdesc.Texture3D.MipLevels					= desc.numMips;
			break;
		case desc::Dimension::TextureCube:
			if (desc.numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURECUBE;
				dxdesc.TextureCube.MostDetailedMip		= desc.firstMip;
				dxdesc.TextureCube.MipLevels			= desc.numMips;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
				dxdesc.TextureCubeArray.MostDetailedMip = desc.firstMip;
				dxdesc.TextureCubeArray.MipLevels		= desc.numMips;
				dxdesc.TextureCubeArray.First2DArrayFace = desc.firstSlice * 6;
				dxdesc.TextureCubeArray.NumCubes		= desc.numSlices;
			}
			break;
		case desc::Dimension::Texture2D:
		default:
			if (texture.descriptor().multisampling != desc::Multisampling::None)
			{
				if (desc.numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
					dxdesc.Texture2DMSArray.FirstArraySlice = desc.firstSlice;
					dxdesc.Texture2DMSArray.ArraySize		= desc.numSlices;
				}
			}
			else
			{
				if (desc.numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2D;
					dxdesc.Texture2D.MostDetailedMip		= desc.firstMip;
					dxdesc.Texture2D.MipLevels				= desc.numMips;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					dxdesc.Texture2DArray.MostDetailedMip	= desc.firstMip;
					dxdesc.Texture2DArray.MipLevels			= desc.numMips;
					dxdesc.Texture2DArray.FirstArraySlice	= desc.firstSlice;
					dxdesc.Texture2DArray.ArraySize			= desc.numSlices;
				}
			}
			break;
		}

		HRESULT hr = device.device()->CreateShaderResourceView(texture.m_texture, &dxdesc,
						(ID3D11ShaderResourceView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateShaderResourceView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}
	}

	void TextureViewImpl::createUAV(DeviceImpl& device,
									const desc::TextureView::Descriptor& desc,
									const TextureImpl& texture)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC dxdesc;

		SP_ASSERT(texture.descriptor().usage == desc::Usage::GpuReadWrite,
			"Cannot create UAV for a texture that was not created with Usage::GpuReadWrite.");

		SP_ASSERT(texture.descriptor().multisampling == desc::Multisampling::None,
			"Cannot create UAV for a multisampled texture. It can be written only in a pixel shader.");

		dxdesc.Format			= dxgiFormat(desc.format);
		switch (desc.dimension)
		{
		case desc::Dimension::Texture1D:
			if (desc.numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_UAV_DIMENSION_TEXTURE1D;
				dxdesc.Texture1D.MipSlice				= desc.firstMip;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
				dxdesc.Texture1DArray.MipSlice			= desc.firstMip;
				dxdesc.Texture1DArray.FirstArraySlice	= desc.firstSlice;
				dxdesc.Texture1DArray.ArraySize			= desc.numSlices;
			}
			break;
		case desc::Dimension::Texture3D:
			dxdesc.ViewDimension						= D3D11_UAV_DIMENSION_TEXTURE3D;
			dxdesc.Texture3D.MipSlice					= desc.firstMip;
			dxdesc.Texture3D.FirstWSlice				= desc.firstSlice;
			dxdesc.Texture3D.WSize						= desc.numSlices;
			break;
		case desc::Dimension::TextureCube:
			SP_ASSERT(false, "Cannot create UAV for a TextureCube. Use a Texture2DArray to access the cube.");
			break;
		case desc::Dimension::Texture2D:
		default:
			if (desc.numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_UAV_DIMENSION_TEXTURE2D;
				dxdesc.Texture2D.MipSlice				= desc.firstMip;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
				dxdesc.Texture2DArray.MipSlice			= desc.firstMip;
				dxdesc.Texture2DArray.FirstArraySlice	= desc.firstSlice;
				dxdesc.Texture2DArray.ArraySize			= desc.numSlices;
			}
			break;
		}

		HRESULT hr = device.device()->CreateUnorderedAccessView(texture.m_texture, &dxdesc,
						(ID3D11UnorderedAccessView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateUnorderedAccessView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}
	}

	void TextureViewImpl::createRTV(DeviceImpl& device,
									const desc::TextureView::Descriptor& desc,
									const TextureImpl& texture)
	{
		D3D11_RENDER_TARGET_VIEW_DESC dxdesc;

		SP_ASSERT(texture.descriptor().usage == desc::Usage::RenderTarget,
			"Cannot create RTV for a texture that was not created with Usage::RenderTarget.");

		dxdesc.Format			= dxgiFormat(desc.format);
		switch (desc.dimension)
		{
		case desc::Dimension::Texture1D:
			if (desc.numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE1D;
				dxdesc.Texture1D.MipSlice				= desc.firstMip;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
				dxdesc.Texture1DArray.MipSlice			= desc.firstMip;
				dxdesc.Texture1DArray.FirstArraySlice	= desc.firstSlice;
				dxdesc.Texture1DArray.ArraySize			= desc.numSlices;
			}
			break;
		case desc::Dimension::Texture3D:
			dxdesc.ViewDimension						= D3D11_RTV_DIMENSION_TEXTURE3D;
			dxdesc.Texture3D.MipSlice					= desc.firstMip;
			dxdesc.Texture3D.FirstWSlice				= desc.firstSlice;
			dxdesc.Texture3D.WSize						= desc.numSlices;
			break;
		case desc::Dimension::TextureCube:
			SP_ASSERT(false, "Cannot create UAV for a TextureCube. Use a Texture2DArray to access the cube.");
			break;
		case desc::Dimension::Texture2D:
		default:
			if (texture.descriptor().multisampling != desc::Multisampling::None)
			{
				if (desc.numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
					dxdesc.Texture2DMSArray.FirstArraySlice = desc.firstSlice;
					dxdesc.Texture2DMSArray.ArraySize		= desc.numSlices;
				}
			}
			else
			{
				if (desc.numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2D;
					dxdesc.Texture2D.MipSlice				= desc.firstMip;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
					dxdesc.Texture2DArray.MipSlice			= desc.firstMip;
					dxdesc.Texture2DArray.FirstArraySlice	= desc.firstSlice;
					dxdesc.Texture2DArray.ArraySize			= desc.numSlices;
				}
			}
			break;
		}

		HRESULT hr = device.device()->CreateRenderTargetView(texture.m_texture, &dxdesc,
						(ID3D11RenderTargetView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateRenderTargetView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}
	}

	void TextureViewImpl::createDSV(DeviceImpl& device,
									const desc::TextureView::Descriptor& desc,
									const TextureImpl& texture)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dxdesc;

		SP_ASSERT(texture.descriptor().usage == desc::Usage::DepthBuffer,
			"Cannot create DSV for a texture that was not created with Usage::DepthStencil.");

		dxdesc.Format			= dxgiFormat(desc.format);
		switch (desc.dimension)
		{
		case desc::Dimension::Texture1D:
			if (desc.numSlices <= 1)
			{
				dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE1D;
				dxdesc.Texture1D.MipSlice				= desc.firstMip;
			}
			else
			{
				dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
				dxdesc.Texture1DArray.MipSlice			= desc.firstMip;
				dxdesc.Texture1DArray.FirstArraySlice	= desc.firstSlice;
				dxdesc.Texture1DArray.ArraySize			= desc.numSlices;
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
			if (texture.descriptor().multisampling != desc::Multisampling::None)
			{
				if (desc.numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
					dxdesc.Texture2DMSArray.FirstArraySlice = desc.firstSlice;
					dxdesc.Texture2DMSArray.ArraySize		= desc.numSlices;
				}
			}
			else
			{
				if (desc.numSlices <= 1)
				{
					dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE2D;
					dxdesc.Texture2D.MipSlice				= desc.firstMip;
				}
				else
				{
					dxdesc.ViewDimension					= D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
					dxdesc.Texture2DArray.MipSlice			= desc.firstMip;
					dxdesc.Texture2DArray.FirstArraySlice	= desc.firstSlice;
					dxdesc.Texture2DArray.ArraySize			= desc.numSlices;
				}
			}
			break;
		}
		dxdesc.Flags = 0;

		HRESULT hr = device.device()->CreateDepthStencilView(texture.m_texture, &dxdesc,
						(ID3D11DepthStencilView **)&m_view);
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateDepthStencilView() failed!"), _T("Error"), NULL);
			m_view = nullptr;
		}
	}
}
