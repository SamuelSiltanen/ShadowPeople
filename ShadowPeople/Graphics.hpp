#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>
#include <cstdint>

#include "Descriptors.hpp"
#include "Types.hpp"

namespace graphics
{
	class TextureImpl;
	class TextureViewImpl;
	class BufferImpl;
	class BufferViewImpl;
	class SamplerImpl;
	class CommandBufferImpl;
	class ShaderImpl;
	class GraphicsPipelineImpl;
	class ComputePipelineImpl;
	class DeviceImpl;

	class Device;

	class Texture
	{
	public:
		Texture() : pImpl(nullptr) {}		
		Texture(Device& device, const desc::Texture& desc);

		bool valid() { return (pImpl != nullptr); }
		const desc::Texture& descriptor();
	private:
		friend class TextureView;
		friend class CommandBuffer;
		friend class Device;

		Texture(std::shared_ptr<TextureImpl> impl) : pImpl(impl) {}

		std::shared_ptr<TextureImpl> pImpl;
	};

	class TextureView
	{
	public:
		TextureView() : pImpl(nullptr) {}
		TextureView(Device& device, const desc::TextureView& desc, const Texture& texture);

		bool valid() { return (pImpl != nullptr); }
		const desc::TextureView& descriptor();
	private:
		friend class CommandBuffer;

		std::shared_ptr<TextureViewImpl> pImpl;
	};

	class Buffer
	{
	public:
		Buffer() : pImpl(nullptr) {}
		Buffer(Device& buffer, const desc::Buffer& desc);

		bool valid() { return (pImpl != nullptr); }
		const desc::Buffer& descriptor();
	private:
		friend class BufferView;

		std::shared_ptr<BufferImpl> pImpl;
	};

	class BufferView
	{
	public:
		BufferView() : pImpl(nullptr) {}
		BufferView(Device& device, const desc::BufferView& desc, const Buffer& buffer);

		bool valid() { return (pImpl != nullptr); }
		const desc::BufferView& descriptor();
	private:
		friend class CommandBuffer;

		std::shared_ptr<BufferViewImpl> pImpl;
	};

	class Sampler
	{
	public:
		Sampler() : pImpl(nullptr) {}
		Sampler(Device& device, const desc::Sampler& desc);

		bool valid() { return (pImpl != nullptr); }
		const desc::Sampler& descriptor();
	private:
		std::shared_ptr<SamplerImpl> pImpl;
	};

	class CommandBuffer
	{
	public:
		CommandBuffer() : pImpl(nullptr) {}
		CommandBuffer(Device& device);

		bool valid() { return (pImpl != nullptr); }

		void clear(TextureView view, float r, float g, float b, float a);
		void clear(TextureView view, uint32_t r, uint32_t g, uint32_t b, uint32_t a);
		void clear(TextureView view, float depth);
		void clear(TextureView view, float depth, uint8_t stencil);
		void clear(TextureView view, uint8_t stencil);

		void copy(Texture dst, Texture src);
		void copy(Texture dst, Texture src,
				  int2 dstCorner, Rect<int, 2> srcRect,
				  Subresource dstSubresource, Subresource srcSubresource);
		void copy(Texture dst, Texture src,
				  int3 dstCorner, Rect<int, 3> srcRect,
				  Subresource dstSubresource, Subresource srcSubresource);

		void copyToBackBuffer(Texture src);

		void dispatch();
		void dispatchIndirect();

		void draw();
		void drawIndexed();
		void drawInstanced();
		void drawIndexedInstanced();
		void drawInstancedIndirect();
		void drawIndexedInstancedIndirect();
	private:
		friend class Device;

		std::shared_ptr<CommandBufferImpl> pImpl;
	};

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline() : pImpl(nullptr) {}
		GraphicsPipeline(Device& device, const desc::GraphicsPipeline& desc);

		bool valid() { return (pImpl != nullptr); }
		const desc::GraphicsPipeline& descriptor();
	private:
		std::shared_ptr<GraphicsPipelineImpl> pImpl;
	};

	class ComputePipeline
	{
	public:
		ComputePipeline() : pImpl(nullptr) {}
		ComputePipeline(Device& device, const desc::ComputePipeline& desc);

		bool valid() { return (pImpl != nullptr); }
		const desc::ComputePipeline& descriptor();
	private:
		std::shared_ptr<ComputePipelineImpl> pImpl;
	};

	class Device
	{
	public:
		Device(HWND hWnd, unsigned width, unsigned height);

		bool				valid() { return (pImpl != nullptr); }

		Texture				createTexture(const desc::Texture& desc);
		Buffer				createBuffer(const desc::Buffer& desc);

		TextureView			createTextureView(const desc::TextureView& desc, const Texture& texture);
		BufferView			createBufferView(const desc::BufferView& desc, const Buffer& buffer);

		Sampler				createSampler(const desc::Sampler& desc);

		GraphicsPipeline	createGraphicsPipeline(const desc::GraphicsPipeline& desc);
		ComputePipeline		createComputePipeline(const desc::ComputePipeline& desc);

		CommandBuffer		createCommandBuffer();
		void				submit(CommandBuffer& gfx);

		void				present(int syncInterval);
	private:
		friend class Texture;
		friend class TextureView;
		friend class Buffer;
		friend class BufferView;
		friend class Sampler;
		friend class CommandBuffer;
		friend class GraphicsPipeline;
		friend class ComputePipeline;

		std::shared_ptr<DeviceImpl> pImpl;
	};
}
