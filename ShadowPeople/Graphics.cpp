#include "Graphics.hpp"
#include "DX11Graphics.hpp"
#include "ShaderManager.hpp"
#include "Errors.hpp"
#include "Math.hpp"

namespace graphics
{
	Texture::Texture(Device& device, const desc::Texture& desc)
	{
		pImpl = std::make_shared<TextureImpl>(*device.pImpl, desc);
	}

	const desc::Texture& Texture::descriptor() const
	{
		SP_ASSERT(pImpl != nullptr, "Texture used, but not created with Device::createTexture().");
		return pImpl->descriptor();
	}

	TextureView::TextureView(Device& device, const desc::TextureView& desc, const Texture& texture)
	{
		pImpl = std::make_shared<TextureViewImpl>(*device.pImpl, desc, *texture.pImpl);
	}

	const desc::TextureView& TextureView::descriptor() const
	{
		SP_ASSERT(pImpl != nullptr, "TextureView used, but not created with Device::createTextureView().");
		return pImpl->descriptor();
	}

	Buffer::Buffer(Device& device, const desc::Buffer& desc)
	{
		pImpl = std::make_shared<BufferImpl>(*device.pImpl, desc);
	}

	const desc::Buffer& Buffer::descriptor() const
	{
		SP_ASSERT(pImpl != nullptr, "Buffer used, but not created with Device::createBuffer().");
		return pImpl->descriptor();
	}

	BufferView::BufferView(Device& device, const desc::BufferView& desc, const Buffer& buffer)
	{
		pImpl = std::make_shared<BufferViewImpl>(*device.pImpl, desc, *buffer.pImpl);
	}

	const desc::BufferView& BufferView::descriptor() const
	{
		SP_ASSERT(pImpl != nullptr, "BufferView used, but not created with Device::createBufferView().");
		return pImpl->descriptor();
	}

	Sampler::Sampler(Device& device, const desc::Sampler& desc)
	{
		pImpl = std::make_shared<SamplerImpl>(*device.pImpl, desc);
	}

	const desc::Sampler& Sampler::descriptor()
	{
		SP_ASSERT(pImpl != nullptr, "Sampler used, but not created with Device::createSampler().");
		return pImpl->descriptor();
	}

	CommandBuffer::CommandBuffer(Device& device)
	{
		pImpl = std::make_shared<CommandBufferImpl>(*device.pImpl);
	}

	void CommandBuffer::clear(TextureView view, float r, float g, float b, float a)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		pImpl->clear(*view.pImpl, r, g, b, a);
	}

	void CommandBuffer::clear(TextureView view, uint32_t r, uint32_t g, uint32_t b, uint32_t a)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		pImpl->clear(*view.pImpl, r, g, b, a);
	}

	void CommandBuffer::clear(TextureView view, float depth)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		pImpl->clear(*view.pImpl, depth);
	}

	void CommandBuffer::clear(TextureView view, float depth, uint8_t stencil)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		pImpl->clear(*view.pImpl, depth, stencil);
	}

	void CommandBuffer::clear(TextureView view, uint8_t stencil)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		pImpl->clear(*view.pImpl, stencil);
	}

	void CommandBuffer::copy(Texture dst, Texture src)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		pImpl->copy(*dst.pImpl, *src.pImpl);
	}

	void CommandBuffer::copy(Texture dst, Texture src,
							 int2 dstCorner, Rect<int, 2> srcRect,
							 Subresource dstSubresource, Subresource srcSubresource)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		int3 dstCorner3D = { dstCorner[0], dstCorner[1], 0 };
		Rect<int, 3> srcRect3D = Rect<int, 3>({ srcRect.minCorner()[0], srcRect.minCorner()[1], 0 },
											  { srcRect.size()[0], srcRect.size()[1], 1 });
		pImpl->copy(*dst.pImpl, *src.pImpl, dstCorner3D, srcRect3D, dstSubresource, srcSubresource);
	}

	void CommandBuffer::copy(Texture dst, Texture src,
							 int3 dstCorner, Rect<int, 3> srcRect,
							 Subresource dstSubresource, Subresource srcSubresource)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		pImpl->copy(*dst.pImpl, *src.pImpl, dstCorner, srcRect, dstSubresource, srcSubresource);
	}

	void CommandBuffer::copyToBackBuffer(Texture src)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		pImpl->copyToBackBuffer(*src.pImpl);
	}

	ShaderResourcesImpl& CommandBuffer::getResources(desc::ShaderBinding& binding)
	{
		if (binding.computePipeline())
			return binding.computePipeline()->pImpl->resources();
		else if (binding.graphicsPipeline())
			return binding.graphicsPipeline()->pImpl->resources();
		SP_ASSERT(false, "Shader bound to neither compute or graphics pipeline");
		return ShaderResourcesImpl();
	}

	void CommandBuffer::setupResourceBindings(desc::ShaderBinding& binding)
	{
		auto& resources = getResources(binding);
		
		// Special handling for constant buffers:
		// - copy the contents of the constant buffer from CPU to GPU
		// - the implementation should already know which GPU buffers to bind then
		size_t numBindingCBs = binding.cbs().size();
		size_t numPipelineCBs = resources.cbs.size();
		SP_ASSERT(numBindingCBs == numPipelineCBs, "Mismatch in the number of constant buffers.");
		for (uint32_t i = 0; i < binding.cbs().size(); i++)
		{
			auto cbData = binding.cbs()[i];
			pImpl->copyToConstantBuffer(*resources.cbs[i], cbData);
		}
		
		for (auto srv: binding.srvs())
		{
			resources.srvs.emplace_back(srv->impl());
		}
		
		for (auto uav: binding.uavs())
		{
			resources.uavs.emplace_back(uav->impl());
		}

		for (auto sampler : binding.samplers())
		{
			resources.samplers.emplace_back(sampler->pImpl.get());
		}
	}

	void CommandBuffer::clearResourceBindings(desc::ShaderBinding& binding)
	{
		auto& resources = getResources(binding);

		// Note: Clear resources, because they contain shared pointers to resources,
		// and we don't need them anymore.
		// This does NOT apply to constant buffers, because the pipeline re-uses them every frame
		resources.srvs.clear();
		resources.uavs.clear();
		resources.samplers.clear();
	}

	void CommandBuffer::dispatch(desc::ShaderBinding& binding,
								 uint32_t threadsX, uint32_t threadsY, uint32_t threadsZ)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		SP_ASSERT(binding.computePipeline() != nullptr, "ComputePipeline must be bound defore calling dispatch().");

		setupResourceBindings(binding);

		uint32_t threadGroupsX = math::divRoundUp(threadsX, binding.threadGroupSizeX());
		uint32_t threadGroupsY = math::divRoundUp(threadsX, binding.threadGroupSizeY());
		uint32_t threadGroupsZ = math::divRoundUp(threadsX, binding.threadGroupSizeZ());

		pImpl->dispatch(*binding.computePipeline()->pImpl, threadGroupsX, threadGroupsY, threadGroupsZ);

		clearResourceBindings(binding);
	}

	void CommandBuffer::dispatchIndirect(desc::ShaderBinding& binding,
										 const Buffer& argsBuffer, uint32_t argsOffset)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		SP_ASSERT(binding.computePipeline() != nullptr, "ComputePipeline must be bound defore calling dispatchIndirect().");

		setupResourceBindings(binding);

		pImpl->dispatchIndirect(*binding.computePipeline()->pImpl, *argsBuffer.pImpl, argsOffset);

		clearResourceBindings(binding);
	}

	void CommandBuffer::draw(desc::ShaderBinding& binding, uint32_t vertexCount, uint32_t startVertexOffset)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		SP_ASSERT(binding.graphicsPipeline() != nullptr, "GraphicsPipeline must be bound defore calling draw().");

		setupResourceBindings(binding);

		pImpl->draw(*binding.graphicsPipeline()->pImpl, vertexCount, startVertexOffset);

		clearResourceBindings(binding);
	}

	void CommandBuffer::drawIndexed(desc::ShaderBinding& binding, uint32_t indexCount,
									uint32_t startIndexOffset, uint32_t vertexOffset)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		SP_ASSERT(binding.graphicsPipeline() != nullptr, "GraphicsPipeline must be bound defore calling draw().");

		setupResourceBindings(binding);

		pImpl->drawIndexed(*binding.graphicsPipeline()->pImpl, indexCount, startIndexOffset, vertexOffset);

		clearResourceBindings(binding);
	}

	void CommandBuffer::drawInstanced(desc::ShaderBinding& binding, uint32_t vertexCountPerInstance,
									  uint32_t instanceCount, uint32_t startVextexOffset,
									  uint32_t startInstanceOffset)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		SP_ASSERT(binding.graphicsPipeline() != nullptr, "GraphicsPipeline must be bound defore calling draw().");

		setupResourceBindings(binding);

		pImpl->drawInstanced(*binding.graphicsPipeline()->pImpl, vertexCountPerInstance, instanceCount,
							 startVextexOffset, startInstanceOffset);

		clearResourceBindings(binding);
	}

	void CommandBuffer::drawIndexedInstanced(desc::ShaderBinding& binding, uint32_t vertexCountPerInstance,
											 uint32_t instanceCount, uint32_t startVextexOffset,
											 uint32_t vertexOffset, uint32_t startInstanceOffset)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		SP_ASSERT(binding.graphicsPipeline() != nullptr, "GraphicsPipeline must be bound defore calling draw().");

		setupResourceBindings(binding);

		pImpl->drawIndexedInstanced(*binding.graphicsPipeline()->pImpl, vertexCountPerInstance, instanceCount,
									startVextexOffset, vertexOffset, startInstanceOffset);

		clearResourceBindings(binding);
	}

	void CommandBuffer::drawInstancedIndirect(desc::ShaderBinding& binding, const Buffer& argsBuffer,
											  uint32_t argsOffset)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		SP_ASSERT(binding.graphicsPipeline() != nullptr, "GraphicsPipeline must be bound defore calling draw().");

		setupResourceBindings(binding);

		pImpl->drawInstancedIndirect(*binding.graphicsPipeline()->pImpl, *argsBuffer.pImpl, argsOffset);

		clearResourceBindings(binding);
	}

	void CommandBuffer::drawIndexedInstancedIndirect(desc::ShaderBinding& binding, const Buffer& argsBuffer,
													 uint32_t argsOffset)
	{
		SP_ASSERT(pImpl != nullptr, "CommandBuffer used, but created with Device::createCommandBuffer().");
		SP_ASSERT(binding.graphicsPipeline() != nullptr, "GraphicsPipeline must be bound defore calling draw().");

		setupResourceBindings(binding);

		pImpl->drawIndexedInstancedIndirect(*binding.graphicsPipeline()->pImpl, *argsBuffer.pImpl, argsOffset);

		clearResourceBindings(binding);
	}

	GraphicsPipeline::GraphicsPipeline(Device&							device,
									   ShaderManager&					shaderManager,
									   const desc::GraphicsPipeline&	desc)
	{
		pImpl = std::make_shared<GraphicsPipelineImpl>(*device.pImpl, *shaderManager.pImpl, desc);
	}

	const desc::GraphicsPipeline& GraphicsPipeline::descriptor() const
	{
		SP_ASSERT(pImpl != nullptr, "GraphicsPipelineused, but not created with Device::createGraphicsPipeline().");
		return pImpl->descriptor();
	}

	void GraphicsPipeline::setRenderTargets()
	{
		SP_ASSERT(pImpl != nullptr, "GraphicsPipelineused, but not created with Device::createGraphicsPipeline().");
		pImpl->setRenderTargets();
	}

	void GraphicsPipeline::setRenderTargets(TextureView rtv)
	{
		SP_ASSERT(pImpl != nullptr, "GraphicsPipelineused, but not created with Device::createGraphicsPipeline().");
		pImpl->setRenderTargets(*rtv.pImpl);
	}

	void GraphicsPipeline::setRenderTargets(TextureView dsv, TextureView rtv)
	{
		SP_ASSERT(pImpl != nullptr, "GraphicsPipelineused, but not created with Device::createGraphicsPipeline().");
		pImpl->setRenderTargets(*dsv.pImpl, *rtv.pImpl);
	}

	ComputePipeline::ComputePipeline(Device&						device, 
									 ShaderManager&					shaderManager,
									 const desc::ComputePipeline&	desc)
	{
		pImpl = std::make_shared<ComputePipelineImpl>(*device.pImpl, *shaderManager.pImpl, desc);
	}

	const desc::ComputePipeline& ComputePipeline::descriptor()
	{
		SP_ASSERT(pImpl != nullptr, "ComputePipeline used, but not created with Device::createComputePipeline().");
		return pImpl->descriptor();
	}

	Device::Device(HWND hWnd, int2 screenSize)
	{
		pImpl			= std::make_shared<DeviceImpl>(hWnd, screenSize);
		m_shaderManager	= std::make_shared<ShaderManager>(*this);
	}

	Texture Device::createTexture(const desc::Texture& desc)
	{
		return Texture(*this, desc);
	}

	Buffer Device::createBuffer(const desc::Buffer& desc)
	{
		return Buffer(*this, desc);
	}

	TextureView Device::createTextureView(const desc::TextureView& desc, const Texture& texture)
	{
		return TextureView(*this, desc, texture);
	}

	BufferView Device::createBufferView(const desc::BufferView& desc, const Buffer& buffer)
	{
		return BufferView(*this, desc, buffer);
	}

	Sampler Device::createSampler(const desc::Sampler& desc)
	{
		return Sampler(*this, desc);
	}

	Shader::Shader(const std::string& bindingName, desc::ShaderType type)
	{
		pImpl = std::make_shared<ShaderImpl>(bindingName, type);
	}

	GraphicsPipeline Device::createGraphicsPipeline(const desc::GraphicsPipeline& desc)
	{
		return GraphicsPipeline(*this, *m_shaderManager, desc);
	}

	ComputePipeline Device::createComputePipeline(const desc::ComputePipeline& desc)
	{
		return ComputePipeline(*this, *m_shaderManager, desc);
	}

	CommandBuffer Device::createCommandBuffer()
	{
		return CommandBuffer(*this);
	}
		
	void Device::submit(CommandBuffer& gfx)
	{
		SP_ASSERT(pImpl != nullptr, "Device used after a failed Device creation.");
		pImpl->submit(*gfx.pImpl);
	}

	void Device::present(int syncInterval)
	{
		SP_ASSERT(pImpl != nullptr, "Device used after a failed Device creation.");
		pImpl->present(syncInterval);
	}

	int2 Device::swapChainSize()
	{
		SP_ASSERT(pImpl != nullptr, "Device used after a failed Device creation.");
		return pImpl->swapChainSize();
	}
}
