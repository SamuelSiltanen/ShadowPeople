#include "CommandBufferImpl.hpp"
#include "DeviceImpl.hpp"
#include "TextureImpl.hpp"
#include "TextureViewImpl.hpp"
#include "BufferImpl.hpp"
#include "MappingImpl.hpp"
#include "SamplerImpl.hpp"
#include "GraphicsPipelineImpl.hpp"
#include "ComputePipelineImpl.hpp"
#include "ShaderResourcesImpl.hpp"
#include "../Errors.hpp"

#include <vector>

namespace graphics
{
	CommandBufferImpl::CommandBufferImpl(DeviceImpl& device) :
		m_context(*device.context()),
		m_device(device)
	{
		m_currentRenderTargets.rtvs.resize(1); // Note: Reserve slot for one render target
	}

	void CommandBufferImpl::clear(TextureViewImpl& view, float r, float g, float b, float a)
	{
		float colors[4];
		colors[0] = r;
		colors[1] = g;
		colors[2] = b;
		colors[3] = a;
		
		switch (view.descriptor().type)
		{
		case desc::ViewType::UAV:
			{
				auto t = view.descriptor().format.type;
				SP_ASSERT((t == desc::FormatType::Float) || (t == desc::FormatType::UNorm) || (t == desc::FormatType::SNorm),
						  "View is not float-type. Use another clear command for clearing.");				
				m_context.ClearUnorderedAccessViewFloat((ID3D11UnorderedAccessView *)view.m_view, colors);
			}
			break;
		case desc::ViewType::RTV:
			m_context.ClearRenderTargetView((ID3D11RenderTargetView *)view.m_view, colors);
			break;
		case desc::ViewType::DSV:
			SP_ASSERT(view.descriptor().type != desc::ViewType::DSV,
					  "Cannot clear DSV with colors. Use depth clearing commands for clearing DSVs.");
			break;
		case desc::ViewType::SRV:
		default:
			SP_ASSERT(view.descriptor().type != desc::ViewType::SRV,
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

		switch (view.descriptor().type)
		{
		case desc::ViewType::UAV:
			{
				auto t = view.descriptor().format.type;
				SP_ASSERT((t != desc::FormatType::Float) && (t != desc::FormatType::UNorm) && (t != desc::FormatType::SNorm),
						  "View is float-type. Use another clear command for clearing.");
				m_context.ClearUnorderedAccessViewUint((ID3D11UnorderedAccessView *)view.m_view, values);
			}
			break;
		case desc::ViewType::RTV:
			SP_ASSERT(view.descriptor().type != desc::ViewType::RTV,
					  "Cannot clear RTV with non-float values. Use another command for clearing.");
			break;
		case desc::ViewType::DSV:
			SP_ASSERT(view.descriptor().type != desc::ViewType::DSV,
					  "Cannot clear DSV with colors. Use depth clearing commands for clearing DSVs.");
			break;
		case desc::ViewType::SRV:
		default:
			SP_ASSERT(view.descriptor().type != desc::ViewType::SRV,
					  "Cannot clear SRVs. Use UAVs, RTVs, or DSVs for clearing.");
			break;
		}
	}

	void CommandBufferImpl::clear(TextureViewImpl& view, float depth)
	{
		SP_ASSERT(view.descriptor().type == desc::ViewType::DSV,
				  "Only DSVs can be cleared with depth clear command.");
		m_context.ClearDepthStencilView((ID3D11DepthStencilView *)view.m_view, D3D11_CLEAR_DEPTH, depth, 0);
	}

	void CommandBufferImpl::clear(TextureViewImpl& view, float depth, uint8_t stencil)
	{
		SP_ASSERT(view.descriptor().type == desc::ViewType::DSV,
				  "Only DSVs can be cleared with depth clear command.");
		m_context.ClearDepthStencilView((ID3D11DepthStencilView *)view.m_view,
										D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
	}

	void CommandBufferImpl::clear(TextureViewImpl& view, uint8_t stencil)
	{
		SP_ASSERT(view.descriptor().type == desc::ViewType::DSV,
				  "Only DSVs can be cleared with depth clear command.");
		m_context.ClearDepthStencilView((ID3D11DepthStencilView *)view.m_view, D3D11_CLEAR_STENCIL, 0, stencil);
	}

	bool CommandBufferImpl::isSimilarForCopy(const TextureImpl& dst, const TextureImpl& src)
	{
		// TODO

		return true;
	}

	void CommandBufferImpl::copy(TextureImpl& dst, const TextureImpl& src)
	{
		SP_ASSERT(dst.m_texture != src.m_texture, "Copy source and destination cannot be the same texture.");
		SP_ASSERT(dst.descriptor().dimension == src.descriptor().dimension,
				  "Textures in copy operation must have the same dimensions.");
		SP_ASSERT((dst.descriptor().width == src.descriptor().width) &&
				  (dst.descriptor().height == src.descriptor().height) &&
				  (dst.descriptor().depth == src.descriptor().depth),
				  "Textures in copy operation must have the same size or the subresource region must be defined.");
		/*
		// TODO: Some format are compatible, although not equal
		SP_ASSERT(dst.descriptor().format == src.descriptor().format, 
				  "Texture copy operations must have compatible formats.");
				  */
		m_context.CopyResource(dst.m_texture, src.m_texture);
	}

	void CommandBufferImpl::copy(TextureImpl& dst, const TextureImpl& src,
								 int3 dstCorner, Rect<int, 3> srcRect,
								 Subresource dstSubresource, Subresource srcSubresource)
	{
		// TODO: Asserts
		UINT dstSubresourceIndex = D3D11CalcSubresource(dstSubresource.mipLevel,
														dstSubresource.mipLevel,
														dst.descriptor().mipLevels);
		UINT srcSubresourceIndex = D3D11CalcSubresource(srcSubresource.mipLevel,
														srcSubresource.mipLevel,
														src.descriptor().mipLevels);
		D3D11_BOX srcBox;
		
		m_context.CopySubresourceRegion(dst.m_texture, dstSubresourceIndex,
										dstCorner[0], dstCorner[1], dstCorner[2],
										src.m_texture, srcSubresourceIndex, &srcBox);
	}

	void CommandBufferImpl::copyToBackBuffer(const TextureImpl& src)
	{
		auto backBuffer = m_device.getBackBuffer();
		if (isSimilarForCopy(*backBuffer, src))
		{
			copy(*backBuffer, src);
		}
		else
		{
			// TODO: Must do a rectangle copy
		}
	}

	void CommandBufferImpl::setRenderTargets()
	{
		for (auto& rtv : m_currentRenderTargets.rtvs)
		{
			rtv = nullptr;
		}
		m_currentRenderTargets.dsv = nullptr;
	}

	void CommandBufferImpl::setRenderTargets(TextureViewImpl& rtv)
	{
		SP_ASSERT(rtv.descriptor().type == desc::ViewType::RTV, "Only RTV can be bound as render target");		
		m_currentRenderTargets.rtvs[0]			= &rtv; // Note: This assumes only one render target
		m_currentRenderTargets.dsv				= nullptr;
		m_currentRenderTargets.renderTargetSize =
		{
			rtv.texture().descriptor().width,
			rtv.texture().descriptor().height
		};
	}
	
	void CommandBufferImpl::setRenderTargets(TextureViewImpl& dsv, TextureViewImpl& rtv)
	{
		SP_ASSERT(dsv.descriptor().type == desc::ViewType::DSV, "Only DSV can be bound as depth stencil");
		SP_ASSERT(rtv.descriptor().type == desc::ViewType::RTV, "Only RTV can be bound as render target");
		m_currentRenderTargets.rtvs[0]			= &rtv; // Note: This assumes only one render target
		m_currentRenderTargets.dsv				= &dsv;
		m_currentRenderTargets.renderTargetSize =
		{
			rtv.texture().descriptor().width,
			rtv.texture().descriptor().height
		};
	}

	void CommandBufferImpl::setVertexBuffer()
	{
		m_context.IAGetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		m_context.IASetInputLayout(nullptr);
	}

	void CommandBufferImpl::setVertexBuffer(BufferImpl& buffer, GraphicsPipelineImpl& pipeline)
	{
		uint32_t stride = 0;
		for (auto& element : pipeline.descriptor().inputLayout)
		{
			stride += element.format.byteWidth();
		}
		uint32_t offset = 0;
		m_context.IASetVertexBuffers(0, 1, &buffer.m_buffer, &stride, &offset);
		m_context.IASetInputLayout(pipeline.inputLayout());
	}

	void CommandBufferImpl::setIndexBuffer()
	{
		m_context.IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	}

	void CommandBufferImpl::setIndexBuffer(BufferImpl& buffer)
	{
		uint32_t stride = buffer.descriptor().stride;
		SP_ASSERT((stride == 4) || (stride == 2), "Only 32- and 16-bit index buffers are supported");
		DXGI_FORMAT format = (stride == 4) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		m_context.IASetIndexBuffer(buffer.m_buffer, format, 0);
	}

	std::shared_ptr<MappingImpl> CommandBufferImpl::map(BufferImpl& buf)
	{
		return std::make_shared<MappingImpl>(m_device, buf);
	}

	void CommandBufferImpl::setupResources(ComputePipelineImpl& pipeline)
	{
		// Bind shader
		m_context.CSSetShader((ID3D11ComputeShader*)pipeline.shader().m_shader, NULL, 0);

		std::vector<ID3D11Buffer*>				CBs;
		std::vector<ID3D11ShaderResourceView*>	SRVs;
		std::vector<ID3D11UnorderedAccessView*>	UAVs;
		std::vector<uint32_t>					UAVICounts;
		std::vector<ID3D11SamplerState*>		samplers;

		auto resources = pipeline.resources();

		for (auto cb : resources.cbs)
		{
			CBs.emplace_back((ID3D11Buffer*)cb->m_buffer);
		}

		for (auto srv : resources.srvs)
		{
			SRVs.emplace_back((ID3D11ShaderResourceView*)srv->view());
		}

		for (auto uav : resources.uavs)
		{
			UAVs.emplace_back((ID3D11UnorderedAccessView*)uav->view());
		}

		for (auto sampler : resources.samplers)
		{
			samplers.emplace_back((ID3D11SamplerState*)sampler->m_sampler);
		}

		// Bind Resources
		m_context.CSSetConstantBuffers(0, static_cast<uint32_t>(CBs.size()), CBs.data());
		m_context.CSSetShaderResources(0, static_cast<uint32_t>(SRVs.size()), SRVs.data());
		m_context.CSSetUnorderedAccessViews(0, static_cast<uint32_t>(UAVs.size()), UAVs.data(), UAVICounts.data());
		m_context.CSSetSamplers(0, static_cast<uint32_t>(samplers.size()), samplers.data());
	}

	void CommandBufferImpl::setupResources(GraphicsPipelineImpl& pipeline)
	{
		// Bind shaders
		m_context.VSSetShader((ID3D11VertexShader*)pipeline.vertexShader().m_shader, NULL, 0);
		m_context.PSSetShader((ID3D11PixelShader*)pipeline.pixelShader().m_shader, NULL, 0);

		std::vector<ID3D11Buffer*>				CBs;
		std::vector<ID3D11ShaderResourceView*>	SRVs;
		std::vector<ID3D11UnorderedAccessView*>	UAVs;
		std::vector<uint32_t>					UAVICounts;
		std::vector<ID3D11RenderTargetView*>	RTVs;
		ID3D11DepthStencilView*					DSV;
		std::vector<ID3D11SamplerState*>		samplers;

		auto resources = pipeline.resources();

		for (auto cb : resources.cbs)
		{
			CBs.emplace_back((ID3D11Buffer*)cb->m_buffer);
		}

		for (auto srv : resources.srvs)
		{
			SRVs.emplace_back((ID3D11ShaderResourceView*)srv->view());
		}
		
		for (auto rtv : m_currentRenderTargets.rtvs)
		{
			RTVs.emplace_back((ID3D11RenderTargetView*)rtv->view());
		}
		
		DSV = (ID3D11DepthStencilView*)m_currentRenderTargets.dsv->view();

		for (auto sampler : resources.samplers)
		{
			samplers.emplace_back((ID3D11SamplerState*)sampler->m_sampler);
		}

		// Bind Resources
		m_context.VSSetConstantBuffers(0, static_cast<uint32_t>(CBs.size()), CBs.data());
		m_context.PSSetConstantBuffers(0, static_cast<uint32_t>(CBs.size()), CBs.data());
		m_context.VSSetShaderResources(0, static_cast<uint32_t>(SRVs.size()), SRVs.data());
		m_context.PSSetShaderResources(0, static_cast<uint32_t>(SRVs.size()), SRVs.data());
		m_context.VSSetSamplers(0, static_cast<uint32_t>(samplers.size()), samplers.data());
		m_context.PSSetSamplers(0, static_cast<uint32_t>(samplers.size()), samplers.data());
		
		if (resources.uavs.size() == 0)
		{
			m_context.OMSetRenderTargets(static_cast<uint32_t>(RTVs.size()), RTVs.data(), DSV);
		}
		else
		{
			for (auto uav : resources.uavs)
			{
				UAVs.emplace_back((ID3D11UnorderedAccessView*)uav->view());
			}
			m_context.OMGetRenderTargetsAndUnorderedAccessViews(
				static_cast<uint32_t>(RTVs.size()), RTVs.data(), &DSV, 
				static_cast<uint32_t>(RTVs.size()), static_cast<uint32_t>(UAVs.size()), UAVs.data());
		}
		
		setViewport();
	}

	void CommandBufferImpl::setViewport()
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX	= 0;
		viewport.TopLeftY	= 0;
		viewport.Width		= static_cast<float>(m_currentRenderTargets.renderTargetSize[0]);
		viewport.Height		= static_cast<float>(m_currentRenderTargets.renderTargetSize[1]);
		viewport.MinDepth	= 0.f;
		viewport.MaxDepth	= 1.f;
		m_device.context()->RSSetViewports(1, &viewport);
	}

	void CommandBufferImpl::clearResources(ComputePipelineImpl& pipeline)
	{
		auto resources = pipeline.resources();

		// Unbind resources		
		std::vector<ID3D11SamplerState*> samplerZeros(resources.samplers.size(), 0);
		m_context.CSSetSamplers(0, static_cast<uint32_t>(samplerZeros.size()), samplerZeros.data());

		std::vector<ID3D11UnorderedAccessView*> UAVZeros(resources.uavs.size(), 0);
		std::vector<uint32_t> UAVICountZeros(resources.uavs.size(), 0);
		m_context.CSSetUnorderedAccessViews(0, static_cast<uint32_t>(UAVZeros.size()), UAVZeros.data(), UAVICountZeros.data());

		std::vector<ID3D11ShaderResourceView*> SRVZeros(resources.srvs.size(), 0);
		m_context.CSSetShaderResources(0, static_cast<uint32_t>(SRVZeros.size()), SRVZeros.data());

		std::vector<ID3D11Buffer*> CBZeros(resources.cbs.size(), 0);
		m_context.CSSetConstantBuffers(0, static_cast<uint32_t>(CBZeros.size()), CBZeros.data());

		// Unbind shader
		m_context.CSSetShader(NULL, NULL, 0);
	}

	void CommandBufferImpl::clearResources(GraphicsPipelineImpl& pipeline)
	{
		auto resources = pipeline.resources();

		// Unbind resources		
		std::vector<ID3D11SamplerState*> samplerZeros(resources.samplers.size(), 0);
		m_context.VSSetSamplers(0, static_cast<uint32_t>(samplerZeros.size()), samplerZeros.data());
		m_context.PSSetSamplers(0, static_cast<uint32_t>(samplerZeros.size()), samplerZeros.data());

		if (resources.uavs.size() == 0)
		{
			m_context.OMSetRenderTargets(0, nullptr, nullptr);
		}
		else
		{
			std::vector<ID3D11UnorderedAccessView*> UAVZeros(resources.uavs.size(), 0);
			m_context.OMGetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr,
				static_cast<uint32_t>(m_currentRenderTargets.rtvs.size()), static_cast<uint32_t>(UAVZeros.size()),
				UAVZeros.data()); 
		}

		std::vector<ID3D11ShaderResourceView*> SRVZeros(resources.srvs.size(), 0);
		m_context.VSSetShaderResources(0, static_cast<uint32_t>(SRVZeros.size()), SRVZeros.data());
		m_context.PSSetShaderResources(0, static_cast<uint32_t>(SRVZeros.size()), SRVZeros.data());

		std::vector<ID3D11Buffer*> CBZeros(resources.cbs.size(), 0);
		m_context.VSSetConstantBuffers(0, static_cast<uint32_t>(CBZeros.size()), CBZeros.data());
		m_context.PSSetConstantBuffers(0, static_cast<uint32_t>(CBZeros.size()), CBZeros.data());

		// Unbind shaders
		m_context.VSSetShader(NULL, NULL, 0);
		m_context.PSSetShader(NULL, NULL, 0);
	}

	void CommandBufferImpl::setDepthStencilState(GraphicsPipelineImpl& pipeline)
	{
		auto desc = pipeline.descriptor().depthStencilState;
		m_context.OMSetDepthStencilState(pipeline.depthStencilState(), desc.descriptor().stencilRef);
	}

	void CommandBufferImpl::setBlendState(GraphicsPipelineImpl& pipeline)
	{
		auto desc = pipeline.descriptor().blendState;
		m_context.OMSetBlendState(pipeline.blendState(), &desc.descriptor().blendFactor[0], 0xffffffff);
	}

	void CommandBufferImpl::setPrimitiveTopology(GraphicsPipelineImpl& pipeline)
	{
		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

		auto desc = pipeline.descriptor().primitiveTopology;
		switch(desc)
		{
		case desc::PrimitiveTopology::PointList:
			topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
			break;
		case desc::PrimitiveTopology::LineList:
			topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			break;
		case desc::PrimitiveTopology::LineStrip:
			topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
			break;
		case desc::PrimitiveTopology::TriangleList:
			topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case desc::PrimitiveTopology::TriangleStrip:
			topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;
		case desc::PrimitiveTopology::Undefined:
		default:
			SP_ASSERT(false, "Primitive topology undefined - cannot draw");
			break;
		}

		m_context.IASetPrimitiveTopology(topology);
	}

	void CommandBufferImpl::setRasterizerState(GraphicsPipelineImpl& pipeline)
	{
		m_context.RSSetState(pipeline.rasterizerState());		
	}

	void CommandBufferImpl::dispatch(ComputePipelineImpl& pipeline,
									 uint32_t threadGroupsX, uint32_t threadGroupsY, uint32_t threadGroupsZ)
	{
		setupResources(pipeline);
		m_context.Dispatch(threadGroupsX, threadGroupsY, threadGroupsZ);
		clearResources(pipeline);
	}
	
	void CommandBufferImpl::dispatchIndirect(ComputePipelineImpl& pipeline,
											 const BufferImpl& argsBuffer, uint32_t argsOffset)
	{
		setupResources(pipeline);
		m_context.DispatchIndirect(argsBuffer.m_buffer, argsOffset);
		clearResources(pipeline);
	}

	void CommandBufferImpl::draw(GraphicsPipelineImpl& pipeline, uint32_t vertexCount, uint32_t startVertexOffset)
	{
		setupResources(pipeline);
		setDepthStencilState(pipeline);
		setBlendState(pipeline);
		setPrimitiveTopology(pipeline);
		setRasterizerState(pipeline);
		m_context.Draw(vertexCount, startVertexOffset);
		clearResources(pipeline);
	}
		
	void CommandBufferImpl::drawIndexed(GraphicsPipelineImpl& pipeline, uint32_t indexCount,
										uint32_t startIndexOffset, uint32_t vertexOffset)
	{
		setupResources(pipeline);
		setDepthStencilState(pipeline);
		setBlendState(pipeline);
		setPrimitiveTopology(pipeline);
		setRasterizerState(pipeline);
		m_context.DrawIndexed(indexCount, startIndexOffset, vertexOffset);
		clearResources(pipeline);
	}

	void CommandBufferImpl::drawInstanced(GraphicsPipelineImpl& pipeline, uint32_t vertexCountPerInstance,
										  uint32_t instanceCount, uint32_t startVextexOffset,
										  uint32_t startInstanceOffset)
	{
		setupResources(pipeline);
		setDepthStencilState(pipeline);
		setBlendState(pipeline);
		setPrimitiveTopology(pipeline);
		setRasterizerState(pipeline);
		m_context.DrawInstanced(vertexCountPerInstance, instanceCount, startVextexOffset, startInstanceOffset);
		clearResources(pipeline);
	}

	void CommandBufferImpl::drawIndexedInstanced(GraphicsPipelineImpl& pipeline, uint32_t vertexCountPerInstance,
												 uint32_t instanceCount, uint32_t startVextexOffset,
												 uint32_t vertexOffset, uint32_t startInstanceOffset)
	{
		setupResources(pipeline);
		setDepthStencilState(pipeline);
		setBlendState(pipeline);
		setPrimitiveTopology(pipeline);
		setRasterizerState(pipeline);
		m_context.DrawIndexedInstanced(vertexCountPerInstance, instanceCount, startVextexOffset,
									   vertexOffset, startInstanceOffset);
		clearResources(pipeline);
	}
		
	void CommandBufferImpl::drawInstancedIndirect(GraphicsPipelineImpl& pipeline, const BufferImpl& argsBuffer,
												  uint32_t argsOffset)
	{
		setupResources(pipeline);
		setDepthStencilState(pipeline);
		setBlendState(pipeline);
		setPrimitiveTopology(pipeline);
		setRasterizerState(pipeline);
		m_context.DrawInstancedIndirect(argsBuffer.m_buffer, argsOffset);
		clearResources(pipeline);
	}
	
	void CommandBufferImpl::drawIndexedInstancedIndirect(GraphicsPipelineImpl& pipeline,
														 const BufferImpl& argsBuffer, uint32_t argsOffset)
	{
		setupResources(pipeline);
		setDepthStencilState(pipeline);
		setBlendState(pipeline);
		setPrimitiveTopology(pipeline);
		setRasterizerState(pipeline);
		m_context.DrawIndexedInstancedIndirect(argsBuffer.m_buffer, argsOffset);
		clearResources(pipeline);
	}
}
