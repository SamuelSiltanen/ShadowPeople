#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"
#include "ShaderResourcesImpl.hpp"
#include "ShaderImpl.hpp"

namespace graphics
{
	class DeviceImpl;
	class ShaderManagerImpl;
	class TextureViewImpl;

	class GraphicsPipelineImpl
	{
	public:
		GraphicsPipelineImpl(DeviceImpl&					device,
							 ShaderManagerImpl&				shaderManager,
							 const desc::GraphicsPipeline&	desc);

		NO_COPY_CLASS(GraphicsPipelineImpl)

		void setRenderTargets();
		void setRenderTargets(TextureViewImpl& rtv);
		void setRenderTargets(TextureViewImpl& dsv, TextureViewImpl& rtv);

		const desc::GraphicsPipeline& descriptor() const { return m_descriptor; }
		const ShaderImpl& vertexShader() const			 { return m_vertexShader; }
		const ShaderImpl& pixelShader() const			 { return m_pixelShader; }
		ShaderResourcesImpl& resources()				 { return m_resources; }

		ID3D11DepthStencilState* depthStencilState() { return m_depthStencilState; }
		ID3D11BlendState* blendState() { return m_blendState; }
		ID3D11RasterizerState* rasterizerState() { return m_rasterizerState; }
	private:
		void createDepthStencilState(const desc::DepthStencilState& desc);
		void createBlendState(const desc::BlendState& desc);
		void createRasterizerState(const desc::RasterizerState& desc);
		void setViewport(uint32_t width, uint32_t height);

		ShaderImpl				m_vertexShader;
		ShaderImpl				m_pixelShader;
		ShaderResourcesImpl		m_resources;
		desc::GraphicsPipeline	m_descriptor;

		ID3D11DepthStencilState* m_depthStencilState;
		ID3D11BlendState*		m_blendState;
		ID3D11RasterizerState*	m_rasterizerState;

		DeviceImpl&				m_device;
	};
}
