#include "Graphics.hpp"
#include "DX11Graphics.hpp"
#include "Errors.hpp"

namespace graphics
{
	Texture::Texture(Device& device, const desc::Texture& desc)
	{
		pImpl = std::make_shared<TextureImpl>(*device.pImpl, desc);
	}

	const desc::Texture& Texture::descriptor()
	{
		SP_ASSERT(pImpl != nullptr, "Texture used, but not created with Device::createTexture().");
		return pImpl->descriptor();
	}

	TextureView::TextureView(Device& device, const desc::TextureView& desc, const Texture& texture)
	{
		pImpl = std::make_shared<TextureViewImpl>(*device.pImpl, desc, *texture.pImpl);
	}

	const desc::TextureView& TextureView::descriptor()
	{
		SP_ASSERT(pImpl != nullptr, "TextureView used, but not created with Device::createTextureView().");
		return pImpl->descriptor();
	}

	Buffer::Buffer(Device& device, const desc::Buffer& desc)
	{
		pImpl = std::make_shared<BufferImpl>(*device.pImpl, desc);
	}

	const desc::Buffer& Buffer::descriptor()
	{
		SP_ASSERT(pImpl != nullptr, "Buffer used, but not created with Device::createBuffer().");
		return pImpl->descriptor();
	}

	BufferView::BufferView(Device& device, const desc::BufferView& desc, const Buffer& buffer)
	{
		pImpl = std::make_shared<BufferViewImpl>(*device.pImpl, desc, *buffer.pImpl);
	}

	const desc::BufferView& BufferView::descriptor()
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

	void CommandBuffer::dispatch()
	{

	}

	void CommandBuffer::dispatchIndirect()
	{

	}

	void CommandBuffer::draw()
	{

	}

	void CommandBuffer::drawIndexed()
	{

	}

	void CommandBuffer::drawInstanced()
	{

	}

	void CommandBuffer::drawIndexedInstanced()
	{

	}

	void CommandBuffer::drawInstancedIndirect()
	{

	}

	void CommandBuffer::drawIndexedInstancedIndirect()
	{

	}

	GraphicsPipeline::GraphicsPipeline(Device& device, const desc::GraphicsPipeline& desc)
	{
		pImpl = std::make_shared<GraphicsPipelineImpl>(*device.pImpl, desc);
	}

	const desc::GraphicsPipeline& GraphicsPipeline::descriptor()
	{
		SP_ASSERT(pImpl != nullptr, "GraphicsPipelineused, but not created with Device::createGraphicsPipeline().");
		return pImpl->descriptor();
	}

	ComputePipeline::ComputePipeline(Device& device, const desc::ComputePipeline& desc)
	{
		pImpl = std::make_shared<ComputePipelineImpl>(*device.pImpl, desc);
	}

	const desc::ComputePipeline& ComputePipeline::descriptor()
	{
		SP_ASSERT(pImpl != nullptr, "ComputePipeline used, but not created with Device::createComputePipeline().");
		return pImpl->descriptor();
	}

	Device::Device(HWND hWnd, unsigned width, unsigned height)
	{
		pImpl = std::make_shared<DeviceImpl>(hWnd, width, height);
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

	GraphicsPipeline Device::createGraphicsPipeline(const desc::GraphicsPipeline& desc)
	{
		return GraphicsPipeline(*this, desc);
	}

	ComputePipeline Device::createComputePipeline(const desc::ComputePipeline& desc)
	{
		return ComputePipeline(*this, desc);
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
}
