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
		m_inputLayout(nullptr),
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

		createDepthStencilState(desc.descriptor().depthStencilState);
		createBlendState(desc.descriptor().blendState);
		createRasterizerState(desc.descriptor().rasterizerState);
		if (desc.descriptor().inputLayout.size() > 0)
		{
			createInputLayout(desc.descriptor().inputLayout);
		}
	}

    GraphicsPipelineImpl::~GraphicsPipelineImpl()
    {
        SAFE_RELEASE(m_depthStencilState);
        SAFE_RELEASE(m_blendState);
        SAFE_RELEASE(m_rasterizerState);
        SAFE_RELEASE(m_inputLayout);
    }

	void GraphicsPipelineImpl::setScissorRect(Rect<int, 2> rect)
	{
		D3D11_RECT dxrect;
		dxrect.left		= rect.minCorner()[0];
		dxrect.top		= rect.minCorner()[1];
		dxrect.right	= rect.minCorner()[0] + rect.size()[0];
		dxrect.bottom	= rect.minCorner()[1] + rect.size()[1];
		m_device.context()->RSSetScissorRects(1, &dxrect);
	}

	void GraphicsPipelineImpl::createDepthStencilState(const desc::DepthStencilState& desc)
	{
		auto d = desc.descriptor();

		D3D11_DEPTH_STENCIL_DESC dxdesc;

		dxdesc.DepthEnable					= d.depthTestEnable;
		dxdesc.DepthWriteMask				= d.depthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL :
												D3D11_DEPTH_WRITE_MASK_ZERO;
		dxdesc.DepthFunc					= comparisonFunc(d.depthFunc);
		dxdesc.StencilEnable				= d.stencilEnable;
		dxdesc.StencilReadMask				= d.stencilReadMask;
		dxdesc.StencilWriteMask				= d.stencilWriteMask;
		dxdesc.FrontFace.StencilFailOp		= stencilOp(d.stencilFrontFace.failOp);
		dxdesc.FrontFace.StencilDepthFailOp = stencilOp(d.stencilFrontFace.depthFailOp);
		dxdesc.FrontFace.StencilPassOp		= stencilOp(d.stencilFrontFace.passOp);
		dxdesc.FrontFace.StencilFunc		= comparisonFunc(d.stencilFrontFace.func);
		dxdesc.BackFace.StencilFailOp		= stencilOp(d.stencilBackFace.failOp);
		dxdesc.BackFace.StencilDepthFailOp	= stencilOp(d.stencilBackFace.depthFailOp);
		dxdesc.BackFace.StencilPassOp		= stencilOp(d.stencilBackFace.passOp);
		dxdesc.BackFace.StencilFunc			= comparisonFunc(d.stencilBackFace.func);

		HRESULT hr = m_device.device()->CreateDepthStencilState(&dxdesc, &m_depthStencilState);
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
			dxdesc.RenderTarget[i].BlendEnable				= d.renderTargetBlend[i].enabled;
			dxdesc.RenderTarget[i].SrcBlend					= blendMode(d.renderTargetBlend[i].source);
			dxdesc.RenderTarget[i].DestBlend				= blendMode(d.renderTargetBlend[i].destination);
			dxdesc.RenderTarget[i].BlendOp					= blendOp(d.renderTargetBlend[i].blendOp);
			dxdesc.RenderTarget[i].SrcBlendAlpha			= blendMode(d.renderTargetBlend[i].sourceAlpha);
			dxdesc.RenderTarget[i].DestBlendAlpha			= blendMode(d.renderTargetBlend[i].destinationAlpha);
			dxdesc.RenderTarget[i].BlendOpAlpha				= blendOp(d.renderTargetBlend[i].blendOpAlpha);
			dxdesc.RenderTarget[i].RenderTargetWriteMask	= d.renderTargetBlend[i].writeMask;
		}

		HRESULT hr = m_device.device()->CreateBlendState(&dxdesc, &m_blendState);
		SP_ASSERT_HR(hr, ERROR_CODE_BLEND_STATE_NOT_CREATED);
	}

	void GraphicsPipelineImpl::createRasterizerState(const desc::RasterizerState& desc)
	{
		auto d = desc.descriptor();

		D3D11_RASTERIZER_DESC dxdesc;

		dxdesc.FillMode = (d.fillMode == desc::FillMode::Solid) ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
		dxdesc.CullMode = (d.cullMode == desc::CullMode::Back) ? D3D11_CULL_BACK :
			(d.cullMode == desc::CullMode::Front) ? D3D11_CULL_FRONT : D3D11_CULL_NONE;
		dxdesc.FrontCounterClockwise	= false;
		dxdesc.DepthBias				= d.depthBias;
		dxdesc.DepthBiasClamp			= d.depthBiasClamp;
		dxdesc.SlopeScaledDepthBias		= d.depthBiasSlopeScale;
		dxdesc.DepthClipEnable			= true;
		dxdesc.ScissorEnable			= d.enableScissors;
		dxdesc.MultisampleEnable		= false;
		dxdesc.AntialiasedLineEnable	= false;

		HRESULT hr = m_device.device()->CreateRasterizerState(&dxdesc, &m_rasterizerState);
		SP_ASSERT_HR(hr, ERROR_CODE_RASTERIZER_STATE_NOT_CREATED);
	}

	void GraphicsPipelineImpl::createInputLayout(const std::vector<desc::InputElement>& desc)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> dxdescs;

		uint32_t byteOffset = 0;
		for (const auto& element : desc)
		{
			D3D11_INPUT_ELEMENT_DESC dxdesc;
			dxdesc.SemanticName			= element.semanticName.c_str();
			dxdesc.SemanticIndex		= element.semanticIndex;
			dxdesc.Format				= dxgiFormat(element.format);
			dxdesc.InputSlot			= element.inputSlot;
			dxdesc.AlignedByteOffset	= byteOffset;
			dxdesc.InputSlotClass		= element.isInstanceData ? D3D11_INPUT_PER_INSTANCE_DATA :
																   D3D11_INPUT_PER_VERTEX_DATA;
			dxdesc.InstanceDataStepRate = element.isInstanceData ? element.instanceDataStepRate : 0;

			dxdescs.emplace_back(dxdesc);

			byteOffset += element.format.byteWidth();
		}

		ID3DBlob* byteCode = m_vertexShader.compiledSource();
		HRESULT hr = m_device.device()->CreateInputLayout(dxdescs.data(),
														  static_cast<uint32_t>(dxdescs.size()),
														  byteCode->GetBufferPointer(),
														  byteCode->GetBufferSize(), &m_inputLayout);
		SP_ASSERT_HR(hr, ERROR_CODE_INPUT_LAYOUT_NOT_CREATED);
	}
}
