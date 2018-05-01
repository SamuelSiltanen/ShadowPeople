#include "DeviceImpl.hpp"
#include "TextureImpl.hpp"
#include "../Errors.hpp"

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

		SP_EXPECT_NOT_NULL_RET(!hr, ERROR_CODE_GET_BACK_BUFFER_FAILED, nullptr);

		return std::make_shared<TextureImpl>(backBuffer);
	}

	void DeviceImpl::present(int syncInterval)
	{
		SP_ASSERT(m_swapChain, "Swap chain used, but not properly initialized.");
		m_swapChain->Present(syncInterval, 0);
	}
}
