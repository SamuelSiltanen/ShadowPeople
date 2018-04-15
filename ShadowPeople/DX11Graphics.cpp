#include "DX11Graphics.hpp"
#include "DX11Utils.hpp"
#include "Errors.hpp"

#define SAFE_RELEASE(X) if (X) { X->Release(); X = nullptr; }

namespace graphics
{
	DeviceImpl::DeviceImpl(HWND hWnd, unsigned width, unsigned height)
	{
		IDXGIAdapter* pAdapter				= NULL;
		D3D_DRIVER_TYPE driverType			= D3D_DRIVER_TYPE_HARDWARE;
		HMODULE hSoftware					= NULL;
		unsigned flags						= D3D11_CREATE_DEVICE_SINGLETHREADED;
		D3D_FEATURE_LEVEL* pRequestFeatLvls = NULL;
		unsigned featureLevels				= 0;
		D3D_FEATURE_LEVEL* pGivenFeatLvl	= NULL;

		DXGI_MODE_DESC backBufferMode;
		backBufferMode.Width				= width;
		backBufferMode.Height				= height;
		backBufferMode.RefreshRate			= { 1, 60 };
		backBufferMode.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
		backBufferMode.ScanlineOrdering		= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		backBufferMode.Scaling				= DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc			= backBufferMode;
		swapChainDesc.SampleDesc			= { 1, 0 };
		swapChainDesc.BufferUsage			= DXGI_USAGE_BACK_BUFFER |
											  DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount			= 2;
		swapChainDesc.OutputWindow			= hWnd;
		swapChainDesc.Windowed				= TRUE;
		swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags					= 0;

		HRESULT hr = D3D11CreateDeviceAndSwapChain(pAdapter, driverType, hSoftware,
			flags, pRequestFeatLvls, featureLevels, D3D11_SDK_VERSION, &swapChainDesc,
			&m_swapChain, &m_device, pGivenFeatLvl, &m_context);

		if (hr != S_OK)
		{
			MessageBox(NULL, _T("D3D11CreateDeviceAndSwapChain() failed!"), _T("Error"), NULL);
			m_device	= nullptr;
			m_context	= nullptr;
			m_swapChain = nullptr;
		}
	}

	DeviceImpl::~DeviceImpl()
	{
		SAFE_RELEASE(m_swapChain);
		SAFE_RELEASE(m_context);
		SAFE_RELEASE(m_device);
	}

	void DeviceImpl::submit(CommandBufferImpl& gfx)
	{
		SP_ASSERT(m_context, "Immediate context used, but not properly initialized.");
		m_context->Flush();
	}

	std::shared_ptr<TextureImpl> DeviceImpl::getBackBuffer()
	{
		SP_ASSERT(m_swapChain, "Swap chain used, but not properly initialized.");
		ID3D11Texture2D* backBuffer = nullptr;
		HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backBuffer);
		// TODO: Check errors
		return std::make_shared<TextureImpl>(backBuffer);
	}

	void DeviceImpl::present(int syncInterval)
	{
		SP_ASSERT(m_swapChain, "Swap chain used, but not properly initialized.");
		m_swapChain->Present(syncInterval, 0);
	}

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
	}

	TextureImpl::TextureImpl(ID3D11Texture2D* pTexture)
	{
		// TODO
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

		HRESULT hr = device.m_device->CreateTexture1D(&dxdesc, NULL,
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
		dxdesc.Format		= dxgiFormat(desc.descriptor().format);

		dxdesc.SampleDesc	= setSampleDesc(desc);

		setUsageFlags(desc.descriptor().usage,
					  dxdesc.Usage, dxdesc.CPUAccessFlags, dxdesc.BindFlags);

		dxdesc.MiscFlags = 0;
		if (desc.descriptor().dimension == desc::Dimension::TextureCube)
		{
			dxdesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			dxdesc.ArraySize *= 6;
		}

		HRESULT hr = device.m_device->CreateTexture2D(&dxdesc, NULL,
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
		
		HRESULT hr = device.m_device->CreateTexture3D(&dxdesc, NULL,
						reinterpret_cast<ID3D11Texture3D**>(&m_texture));
		if (hr != S_OK)
		{
			MessageBox(NULL, _T("CreateTexture3D() failed!"), _T("Error"), NULL);
			m_texture = nullptr;
		}
	}

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

	DXGI_SAMPLE_DESC TextureImpl::setSampleDesc(const desc::Texture& desc)
	{
		switch (desc.descriptor().multisampling)
		{
		case desc::Multisampling::MSx2:
			return { 2, static_cast<UINT>(D3D11_STANDARD_MULTISAMPLE_PATTERN) };
			break;
		case desc::Multisampling::MSx4:
			return { 4, static_cast<UINT>(D3D11_STANDARD_MULTISAMPLE_PATTERN) };
			break;
		case desc::Multisampling::MSx8:
			return { 8, static_cast<UINT>(D3D11_STANDARD_MULTISAMPLE_PATTERN) };
			break;
		case desc::Multisampling::MSx16:
			return { 16, static_cast<UINT>(D3D11_STANDARD_MULTISAMPLE_PATTERN) };
			break;
		case desc::Multisampling::None:
		default:
			return { 1, 0 };	// No multi-sampling
			break;
		}
	}

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

	CommandBufferImpl::CommandBufferImpl(DeviceImpl& device) :
		m_context(*device.m_context)
	{}

	void CommandBufferImpl::clear(TextureViewImpl& view, float r, float g, float b, float a)
	{
		float colors[4];
		colors[0] = r;
		colors[1] = g;
		colors[2] = b;
		colors[3] = a;
		
		switch (view.m_descriptor.descriptor().type)
		{
		case desc::ViewType::UAV:
			{
				auto t = view.m_descriptor.descriptor().format.type;
				SP_ASSERT((t != desc::FormatType::Float) && (t != desc::FormatType::UNorm) && (t != desc::FormatType::SNorm),
						  "View is float-type. Use another clear command for clearing.");
				m_context.ClearUnorderedAccessViewFloat((ID3D11UnorderedAccessView *)view.m_view, colors);
			}
			break;
		case desc::ViewType::RTV:
			m_context.ClearRenderTargetView((ID3D11RenderTargetView *)view.m_view, colors);
			break;
		case desc::ViewType::DSV:
			SP_ASSERT(view.m_descriptor.descriptor().type != desc::ViewType::DSV,
					  "Cannot clear DSV with colors. Use depth clearing commands for clearing DSVs.");
			break;
		case desc::ViewType::SRV:
		default:
			SP_ASSERT(view.m_descriptor.descriptor().type != desc::ViewType::SRV,
					  "Cannot clear SRVs. Use UAVs, RTVs, or DSVs for clearing.");
			break;
		}
	}

	void CommandBufferImpl::clear(TextureViewImpl& view, uint32_t r, uint32_t g, uint32_t b, uint32_t a)
	{
		UINT values[4];
		values[0] = r;
		values[1] = g;
		values[2] = b;
		values[3] = a;

		switch (view.m_descriptor.descriptor().type)
		{
		case desc::ViewType::UAV:
			{
				auto t = view.m_descriptor.descriptor().format.type;
				SP_ASSERT((t == desc::FormatType::Float) || (t == desc::FormatType::UNorm) || (t == desc::FormatType::SNorm),
						  "View is not float-type. Use another clear command for clearing.");
				m_context.ClearUnorderedAccessViewUint((ID3D11UnorderedAccessView *)view.m_view, values);
			}
			break;
		case desc::ViewType::RTV:
			SP_ASSERT(view.m_descriptor.descriptor().type != desc::ViewType::RTV,
					  "Cannot clear RTV with non-float values. Use another command for clearing.");
			break;
		case desc::ViewType::DSV:
			SP_ASSERT(view.m_descriptor.descriptor().type != desc::ViewType::DSV,
					  "Cannot clear DSV with colors. Use depth clearing commands for clearing DSVs.");
			break;
		case desc::ViewType::SRV:
		default:
			SP_ASSERT(view.m_descriptor.descriptor().type != desc::ViewType::SRV,
					  "Cannot clear SRVs. Use UAVs, RTVs, or DSVs for clearing.");
			break;
		}
	}

	void CommandBufferImpl::clear(TextureViewImpl& view, float depth)
	{
		SP_ASSERT(view.m_descriptor.descriptor().type == desc::ViewType::DSV,
				  "Only DSVs can be cleared with depth clear command.");
		m_context.ClearDepthStencilView((ID3D11DepthStencilView *)view.m_view, D3D11_CLEAR_DEPTH, depth, 0);
	}

	void CommandBufferImpl::clear(TextureViewImpl& view, float depth, uint8_t stencil)
	{
		SP_ASSERT(view.m_descriptor.descriptor().type == desc::ViewType::DSV,
				  "Only DSVs can be cleared with depth clear command.");
		m_context.ClearDepthStencilView((ID3D11DepthStencilView *)view.m_view,
										D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
	}

	void CommandBufferImpl::clear(TextureViewImpl& view, uint8_t stencil)
	{
		SP_ASSERT(view.m_descriptor.descriptor().type == desc::ViewType::DSV,
				  "Only DSVs can be cleared with depth clear command.");
		m_context.ClearDepthStencilView((ID3D11DepthStencilView *)view.m_view, D3D11_CLEAR_STENCIL, 0, stencil);
	}

	void CommandBufferImpl::copy(TextureImpl& dst, TextureImpl& src)
	{
		SP_ASSERT(dst.m_texture != dst.m_texture, "Copy source and destination cannot be the same texture.");
		SP_ASSERT(dst.m_descriptor.descriptor().dimension == src.m_descriptor.descriptor().dimension,
				  "Textures in copy operation must have the same dimensions.");
		SP_ASSERT((dst.m_descriptor.descriptor().width == src.m_descriptor.descriptor().width) &&
				  (dst.m_descriptor.descriptor().height == src.m_descriptor.descriptor().height) &&
				  (dst.m_descriptor.descriptor().depth == src.m_descriptor.descriptor().depth),
				  "Textures in copy operation must have the same size or the subresource region must be defined.");
		m_context.CopyResource(dst.m_texture, src.m_texture);
	}

	void CommandBufferImpl::copy(TextureImpl& dst, TextureImpl& src,
								 int3 dstCorner, Rect<int, 3> srcRect,
								 Subresource dstSubresource, Subresource srcSubresource)
	{
		// TODO: Asserts
		UINT dstSubresourceIndex = D3D11CalcSubresource(dstSubresource.mipLevel,
														dstSubresource.mipLevel,
														dst.m_descriptor.descriptor().mipLevels);
		UINT srcSubresourceIndex = D3D11CalcSubresource(srcSubresource.mipLevel,
														srcSubresource.mipLevel,
														src.m_descriptor.descriptor().mipLevels);
		D3D11_BOX srcBox;
		
		m_context.CopySubresourceRegion(dst.m_texture, dstSubresourceIndex,
										dstCorner[0], dstCorner[1], dstCorner[2],
										src.m_texture, srcSubresourceIndex, &srcBox);
	}
}
