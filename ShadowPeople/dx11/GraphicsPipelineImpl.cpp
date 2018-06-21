#include "GraphicsPipelineImpl.hpp"
#include "DeviceImpl.hpp"
#include "ShaderManagerImpl.hpp"
#include "TextureViewImpl.hpp"
#include "TextureImpl.hpp"
#include "../Errors.hpp"

namespace graphics
{
	GraphicsPipelineImpl::GraphicsPipelineImpl(DeviceImpl& device,
											   ShaderManagerImpl& shaderManager,
											   const desc::GraphicsPipeline& desc) :
		m_descriptor(desc),
		m_vertexShader(desc.descriptor().binding->filename(), desc::ShaderType::Vertex),
		m_pixelShader(desc.descriptor().binding->filename(), desc::ShaderType::Pixel),
		m_depthStencilState(nullptr),
		m_blendState(nullptr),
		m_rasterizerState(nullptr),
		m_device(device)
	{
		if (shaderManager.compile(m_vertexShader))
		{
			shaderManager.createVertexShader(m_vertexShader);
		}
		if (shaderManager.compile(m_pixelShader))
		{
			shaderManager.createPixelShader(m_pixelShader);
		}

		shaderManager.createConstantBuffers(m_resources, *desc.descriptor().binding);

		m_resources.rtvs.resize(desc.descriptor().numRenderTargets);
		for (uint32_t i = 0; i < desc.descriptor().numRenderTargets; i++)
		{
			m_resources.rtvs[i] = nullptr;
		}
		m_resources.dsv = nullptr;

		createDepthStencilState(desc.descriptor().depthStencilState);
		createBlendState(desc.descriptor().blendState);
		createRasterizerState(desc.descriptor().rasterizerState);
	}

	void GraphicsPipelineImpl::setRenderTargets()
	{
		m_device.m_context->OMSetRenderTargets(0, nullptr, nullptr);
	}

	void GraphicsPipelineImpl::setRenderTargets(TextureViewImpl& rtv)
	{
		// TODO: This should only store the RTVs in resources, not call the DX setter

		SP_ASSERT(rtv.descriptor().descriptor().type == desc::ViewType::RTV, "Only RTV can be bound as render target");
		const ID3D11View* RTVs = rtv.view();
		m_device.m_context->OMSetRenderTargets(1, (ID3D11RenderTargetView**)&RTVs, nullptr);
		setViewport(rtv.texture().descriptor().descriptor().width, rtv.texture().descriptor().descriptor().height);
	}
	
	void GraphicsPipelineImpl::setRenderTargets(TextureViewImpl& dsv, TextureViewImpl& rtv)
	{
		SP_ASSERT(dsv.descriptor().descriptor().type == desc::ViewType::DSV, "Only DSV can be bound as depth stencil");
		SP_ASSERT(rtv.descriptor().descriptor().type == desc::ViewType::RTV, "Only RTV can be bound as render target");
		const ID3D11View* DSVs = dsv.view();
		const ID3D11View* RTVs = rtv.view();
		m_device.m_context->OMSetRenderTargets(1, (ID3D11RenderTargetView**)&RTVs, (ID3D11DepthStencilView*)DSVs);
		setViewport(rtv.texture().descriptor().descriptor().width, rtv.texture().descriptor().descriptor().height);
	}

	void GraphicsPipelineImpl::setViewport(uint32_t width, uint32_t height)
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX	= 0;
		viewport.TopLeftY	= 0;
		viewport.Width		= static_cast<float>(width);
		viewport.Height		= static_cast<float>(height);
		viewport.MinDepth	= 0.f;
		viewport.MaxDepth	= 1.f;
		m_device.m_context->RSSetViewports(1, &viewport);
	}

	void GraphicsPipelineImpl::createDepthStencilState(const desc::DepthStencilState& desc)
	{
		auto d = desc.descriptor();

		D3D11_DEPTH_STENCIL_DESC dxdesc;

		dxdesc.DepthEnable = d.depthTestEnable;
		dxdesc.DepthWriteMask = d.depthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		dxdesc.DepthFunc = comparisonFunc(d.depthFunc);
		dxdesc.StencilEnable = d.stencilEnable;
		dxdesc.StencilReadMask = d.stencilReadMask;
		dxdesc.StencilWriteMask = d.stencilWriteMask;
		dxdesc.FrontFace.StencilFailOp = stencilOp(d.stencilFrontFace.failOp);
		dxdesc.FrontFace.StencilDepthFailOp = stencilOp(d.stencilFrontFace.depthFailOp);
		dxdesc.FrontFace.StencilPassOp = stencilOp(d.stencilFrontFace.passOp);
		dxdesc.FrontFace.StencilFunc = comparisonFunc(d.stencilFrontFace.func);
		dxdesc.BackFace.StencilFailOp = stencilOp(d.stencilBackFace.failOp);
		dxdesc.BackFace.StencilDepthFailOp = stencilOp(d.stencilBackFace.depthFailOp);
		dxdesc.BackFace.StencilPassOp = stencilOp(d.stencilBackFace.passOp);
		dxdesc.BackFace.StencilFunc = comparisonFunc(d.stencilBackFace.func);

		HRESULT hr = m_device.m_device->CreateDepthStencilState(&dxdesc, &m_depthStencilState);
		SP_ASSERT_HR(hr, ERROR_CODE_DEPTH_STENCIL_STATE_NOT_CREATED);
	}

	void GraphicsPipelineImpl::createBlendState(const desc::BlendState& desc)
	{
		auto d = desc.descriptor();

		D3D11_BLEND_DESC dxdesc;

		dxdesc.AlphaToCoverageEnable = false;
		dxdesc.IndependentBlendEnable = true;
		for (uint32_t i = 0; i < 8; i++)
		{
			dxdesc.RenderTarget[i].BlendEnable = d.renderTargetBlend[i].enabled;
			dxdesc.RenderTarget[i].SrcBlend = blendMode(d.renderTargetBlend[i].source);
			dxdesc.RenderTarget[i].DestBlend = blendMode(d.renderTargetBlend[i].destination);
			dxdesc.RenderTarget[i].BlendOp = blendOp(d.renderTargetBlend[i].blendOp);
			dxdesc.RenderTarget[i].SrcBlendAlpha = blendMode(d.renderTargetBlend[i].sourceAlpha);
			dxdesc.RenderTarget[i].DestBlendAlpha = blendMode(d.renderTargetBlend[i].destinationAlpha);
			dxdesc.RenderTarget[i].BlendOpAlpha = blendOp(d.renderTargetBlend[i].blendOpAlpha);
			dxdesc.RenderTarget[i].RenderTargetWriteMask = d.renderTargetBlend[i].writeMask;
		}

		HRESULT hr = m_device.m_device->CreateBlendState(&dxdesc, &m_blendState);
		SP_ASSERT_HR(hr, ERROR_CODE_BLEND_STATE_NOT_CREATED);
	}

	void GraphicsPipelineImpl::createRasterizerState(const desc::RasterizerState& desc)
	{
		auto d = desc.descriptor();

		D3D11_RASTERIZER_DESC dxdesc;

		dxdesc.FillMode = (d.fillMode == desc::FillMode::Solid) ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
		dxdesc.CullMode = (d.cullMode == desc::CullMode::Back) ? D3D11_CULL_BACK :
			(d.cullMode == desc::CullMode::Front) ? D3D11_CULL_FRONT : D3D11_CULL_NONE;
		dxdesc.FrontCounterClockwise = false;
		dxdesc.DepthBias = d.depthBias;
		dxdesc.DepthBiasClamp = d.depthBiasClamp;
		dxdesc.SlopeScaledDepthBias = d.depthBiasSlopeScale;
		dxdesc.DepthClipEnable = true;
		dxdesc.ScissorEnable = false;
		dxdesc.MultisampleEnable = false;
		dxdesc.AntialiasedLineEnable = false;

		HRESULT hr = m_device.m_device->CreateRasterizerState(&dxdesc, &m_rasterizerState);
		SP_ASSERT_HR(hr, ERROR_CODE_RASTERIZER_STATE_NOT_CREATED);
	}
}
