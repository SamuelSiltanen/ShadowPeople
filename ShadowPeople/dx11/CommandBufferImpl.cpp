#include "CommandBufferImpl.hpp"
#include "DeviceImpl.hpp"
#include "TextureImpl.hpp"
#include "TextureViewImpl.hpp"
#include "BufferImpl.hpp"
#include "ComputePipelineImpl.hpp"
#include "../Errors.hpp"

#include <vector>

namespace graphics
{
	CommandBufferImpl::CommandBufferImpl(DeviceImpl& device) :
		m_context(*device.m_context),
		m_device(device)
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
				SP_ASSERT((t == desc::FormatType::Float) || (t == desc::FormatType::UNorm) || (t == desc::FormatType::SNorm),
						  "View is not float-type. Use another clear command for clearing.");				
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
				SP_ASSERT((t != desc::FormatType::Float) && (t != desc::FormatType::UNorm) && (t != desc::FormatType::SNorm),
						  "View is float-type. Use another clear command for clearing.");
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

	bool CommandBufferImpl::isSimilarForCopy(const TextureImpl& dst, const TextureImpl& src)
	{
		// TODO

		return true;
	}

	void CommandBufferImpl::copy(TextureImpl& dst, const TextureImpl& src)
	{
		SP_ASSERT(dst.m_texture != src.m_texture, "Copy source and destination cannot be the same texture.");
		SP_ASSERT(dst.m_descriptor.descriptor().dimension == src.m_descriptor.descriptor().dimension,
				  "Textures in copy operation must have the same dimensions.");
		SP_ASSERT((dst.m_descriptor.descriptor().width == src.m_descriptor.descriptor().width) &&
				  (dst.m_descriptor.descriptor().height == src.m_descriptor.descriptor().height) &&
				  (dst.m_descriptor.descriptor().depth == src.m_descriptor.descriptor().depth),
				  "Textures in copy operation must have the same size or the subresource region must be defined.");
		/*
		// TODO: Some format are compatible, although not equal
		SP_ASSERT(dst.m_descriptor.descriptor().format == src.m_descriptor.descriptor().format, 
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
														dst.m_descriptor.descriptor().mipLevels);
		UINT srcSubresourceIndex = D3D11CalcSubresource(srcSubresource.mipLevel,
														srcSubresource.mipLevel,
														src.m_descriptor.descriptor().mipLevels);
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

	void CommandBufferImpl::dispatch(const ComputePipelineImpl& pipeline,
									 uint32_t threadsX, uint32_t threadsY, uint32_t threadsZ)
	{
		// Bind Shader
		m_context.CSSetShader(pipeline.shader(), NULL, 0);

		// TODO: Get the resources from the shader interface
		std::vector<ID3D11Buffer*>				CBs;
		std::vector<ID3D11ShaderResourceView*>	SRVs;
		std::vector<ID3D11UnorderedAccessView*>	UAVs;
		std::vector<uint32_t>					UAVICounts;
		std::vector<ID3D11SamplerState*>		samplers;

		auto& shaderInteface = pipeline.descriptor().shaderInterface();

		// Bind Resources
		m_context.CSSetConstantBuffers(0, static_cast<uint32_t>(CBs.size()), CBs.data());
		m_context.CSSetShaderResources(0, static_cast<uint32_t>(SRVs.size()), SRVs.data());
		m_context.CSSetUnorderedAccessViews(0, static_cast<uint32_t>(UAVs.size()), UAVs.data(), UAVICounts.data());
		m_context.CSSetSamplers(0, static_cast<uint32_t>(samplers.size()), samplers.data());

		m_context.Dispatch(threadsX, threadsY, threadsZ);

		// Unbind resources
		memset(samplers.data(), 0, samplers.size() * sizeof(ID3D11SamplerState*));
		m_context.CSSetSamplers(0, static_cast<uint32_t>(samplers.size()), samplers.data());

		memset(UAVs.data(), 0, UAVs.size() * sizeof(ID3D11UnorderedAccessView*));
		memset(UAVICounts.data(), 0, UAVICounts.size() * sizeof(uint32_t));
		m_context.CSSetUnorderedAccessViews(0, static_cast<uint32_t>(UAVs.size()), UAVs.data(), UAVICounts.data());

		memset(SRVs.data(), 0, SRVs.size() * sizeof(ID3D11ShaderResourceView*));
		m_context.CSSetShaderResources(0, static_cast<uint32_t>(SRVs.size()), SRVs.data());

		memset(CBs.data(), 0, CBs.size() * sizeof(ID3D11Buffer*));
		m_context.CSSetConstantBuffers(0, static_cast<uint32_t>(CBs.size()), CBs.data());

		// Unbind shader
		m_context.CSSetShader(NULL, NULL, 0);
	}
	
	void CommandBufferImpl::dispatchIndirect(const ComputePipelineImpl& pipeline,
											 const BufferImpl& argsBuffer, uint32_t argsOffset)
	{

	}
}
