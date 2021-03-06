/*
    Copyright 2018 Samuel Siltanen
    Graphics.hpp
*/

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <memory>
#include <cstdint>

#include "Descriptors.hpp"
#include "Image.hpp"
#include "../Types.hpp"

namespace graphics
{
	// Forward declare implementation classes for pointer to implementation scheme
	class TextureImpl;
	class TextureViewImpl;
	class BufferImpl;
	class BufferViewImpl;
	class ResourceViewImpl;
	class MappingImpl;
	class SamplerImpl;
	class CommandBufferImpl;
	class ShaderImpl;
	struct ShaderResourcesImpl;
	class GraphicsPipelineImpl;
	class ComputePipelineImpl;
	class DeviceImpl;

	class Device;
	class ShaderManager;
    namespace shaders
    {
        class SlotAllocator;
    }

	class Texture
	{
	public:
		Texture() : pImpl(nullptr) {}		
		Texture(Device& device, const desc::Texture& desc);

		bool valid() const { return (pImpl != nullptr); }
		const desc::Texture::Descriptor& descriptor() const;
	private:
		friend class TextureView;
		friend class CommandBuffer;
		friend class Device;

		Texture(std::shared_ptr<TextureImpl> impl) : pImpl(impl) {}

		std::shared_ptr<TextureImpl> pImpl;
	};

	class ResourceView
	{
	public:
        ResourceView() : pImpl(nullptr) {}
        ResourceView(std::shared_ptr<ResourceViewImpl> pImpl) : pImpl(pImpl) {}

        virtual bool valid() const { return (pImpl != nullptr); }
	protected:
		friend class CommandBuffer;
        friend class shaders::SlotAllocator;

        virtual std::string debugName() const { return std::string(); }

        std::shared_ptr<ResourceViewImpl> pImpl;
	};

	class TextureView : public ResourceView
	{
	public:
		TextureView() : pImpl(nullptr) {}
		TextureView(Device& device, const Texture& texture, const desc::TextureView::Descriptor& desc);

		bool valid() const override { return (pImpl != nullptr); }

		const desc::TextureView::Descriptor& descriptor() const;

	protected:		
        std::string debugName() const override;

	private:
		friend class CommandBuffer;
		friend class GraphicsPipeline;

		std::shared_ptr<TextureViewImpl> pImpl;
	};

	class Buffer
	{
	public:
		Buffer() : pImpl(nullptr) {}
		Buffer(Device& buffer, const desc::Buffer& desc);

		bool valid() const { return (pImpl != nullptr); }
		const desc::Buffer::Descriptor& descriptor() const;
	private:
		friend class BufferView;
		friend class CommandBuffer;

		std::shared_ptr<BufferImpl> pImpl;
	};

	class BufferView : public ResourceView
	{
	public:
		BufferView() : pImpl(nullptr) {}
		BufferView(Device& device, const Buffer& buffer, const desc::BufferView::Descriptor& desc);

		bool valid() const override { return (pImpl != nullptr); }

		const desc::BufferView::Descriptor& descriptor() const;

	protected:
        std::string debugName() const override;

	private:
		friend class CommandBuffer;		

		std::shared_ptr<BufferViewImpl> pImpl;
	};

	class Mapping
	{
	public:
		Mapping(std::shared_ptr<MappingImpl> impl);

		void* data();
	private:
		std::shared_ptr<MappingImpl> pImpl;
	};

	class Sampler
	{
	public:
		Sampler() : pImpl(nullptr) {}
		Sampler(Device& device, const desc::Sampler& desc);

		bool valid() { return (pImpl != nullptr); }
		const desc::Sampler::Descriptor& descriptor();
	private:
		friend class CommandBuffer;

		std::shared_ptr<SamplerImpl> pImpl;
	};

	class CommandBuffer
	{
	public:
		CommandBuffer() : pImpl(nullptr) {}
		CommandBuffer(Device& device);

		bool valid() const { return (pImpl != nullptr); }

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

        void update(Texture dst, const Image src,
                    int2 dstCorner = { 0, 0 }, Rect<int, 2> srcRect = Rect<int, 2>(),
                    Subresource dstSubresource = Subresource());
        void update(Buffer dst, Range<const uint8_t> cpuData, uint32_t dstOffset = 0);

		void setRenderTargets();
		void setRenderTargets(TextureView rtv);
		void setRenderTargets(TextureView dsv, TextureView rtv);

		void setVertexBuffer();
		void setVertexBuffer(Buffer buffer, GraphicsPipeline pipeline);
		void setIndexBuffer();
		void setIndexBuffer(Buffer buffer);		

		// This is mainly for filling index and vertex buffers
		Mapping map(Buffer buf);

		void dispatch(desc::ShaderBinding& binding,
					  uint32_t threadsX, uint32_t threadsY, uint32_t threadsZ);
		void dispatchIndirect(desc::ShaderBinding& binding,
							  const Buffer& argsBuffer, uint32_t argsOffset);

		void draw(desc::ShaderBinding& binding, uint32_t vertexCount, uint32_t startVertexOffset);
		void drawIndexed(desc::ShaderBinding& binding, uint32_t indexCount, uint32_t startIndexOffset,
						 uint32_t vertexOffset);
		void drawInstanced(desc::ShaderBinding& binding, uint32_t vertexCountPerInstance, uint32_t instanceCount,
						   uint32_t startVextexOffset, uint32_t startInstanceOffset);
		void drawIndexedInstanced(desc::ShaderBinding& binding, uint32_t vertexCountPerInstance,
								  uint32_t instanceCount, uint32_t startVextexOffset, uint32_t vertexOffset,
								  uint32_t startInstanceOffset);
		void drawInstancedIndirect(desc::ShaderBinding& binding, const Buffer& argsBuffer, uint32_t argsOffset);
		void drawIndexedInstancedIndirect(desc::ShaderBinding& binding, const Buffer& argsBuffer, uint32_t argsOffset);
	private:
		friend class Device;

		ShaderResourcesImpl& getResources(desc::ShaderBinding& binding);
		void setupResourceBindings(desc::ShaderBinding& binding);
		void clearResourceBindings(desc::ShaderBinding& binding);

		std::shared_ptr<CommandBufferImpl> pImpl;
	};

	class Shader
	{
	public:
		Shader(const std::string& bindingName, desc::ShaderType type);
	private:
		friend class ShaderManager;

		std::shared_ptr<ShaderImpl> pImpl;
	};

    namespace shaders
    {
        class SlotAllocator
        {
        public:
            uint32_t nextCBVSlot();
            uint32_t nextSRVSlot();
            uint32_t nextUAVSlot();
            uint32_t nextSamplerSlot();

            void setCBV(uint32_t cbvSlot, Range<const uint8_t> constants);
            void setSRV(uint32_t srvSlot, const ResourceView& srv);
            void setUAV(uint32_t uavSlot, const ResourceView& uav);
            void setSampler(uint32_t samplerSlot, const Sampler* sampler);

            const std::vector<Range<const uint8_t>>& cbs()      { return m_cbs; }
            const std::vector<ResourceView>&         srvs()     { return m_srvs; }
            const std::vector<ResourceView>&         uavs()     { return m_uavs; }
            const std::vector<const Sampler*>&       samplers() { return m_samplers; }

            void resetBindings();
        private:
            std::vector<Range<const uint8_t>>	m_cbs;
			std::vector<ResourceView>	        m_srvs;
			std::vector<ResourceView>	        m_uavs;
			std::vector<const Sampler*>		    m_samplers;
        };

        extern SlotAllocator g_slotAllocator;
    }

	class GraphicsPipeline
	{
	public:
		GraphicsPipeline() : pImpl(nullptr) {}
		GraphicsPipeline(Device&						device, 
						 ShaderManager&					shaderManager,
						 const desc::GraphicsPipeline&	desc);

		bool valid() const { return (pImpl != nullptr); }
		const desc::GraphicsPipeline::Descriptor& descriptor() const;

		template<typename T>
		std::unique_ptr<T> bind(CommandBuffer& gfx)
		{
			shaders::g_slotAllocator.resetBindings();
			return std::make_unique<T>(this);
		}

		void setScissorRect(Rect<int, 2> rect);
	private:
		friend class CommandBuffer;

		std::shared_ptr<GraphicsPipelineImpl> pImpl;
	};

	class ComputePipeline
	{
	public:
		ComputePipeline() : pImpl(nullptr) {}
		ComputePipeline(Device&							device,
						ShaderManager&					shaderManager,
						const desc::ComputePipeline&	desc);

		bool valid() { return (pImpl != nullptr); }
		const desc::ComputePipeline::Descriptor& descriptor();

		template<typename T> 
		std::unique_ptr<T> bind(CommandBuffer& gfx)
		{
			shaders::g_slotAllocator.resetBindings();
			return std::make_unique<T>(this);
		}
	private:
		friend class CommandBuffer;

		std::shared_ptr<ComputePipelineImpl> pImpl;
	};

	class Device
	{
	public:
		Device(HWND hWnd, int2 screenSize);

		bool				valid() const { return (pImpl != nullptr); }

		Texture				createTexture(const desc::Texture& desc);
		Buffer				createBuffer(const desc::Buffer& desc);

		TextureView			createTextureView(const Texture& texture, const desc::TextureView& desc);
		BufferView			createBufferView(const Buffer& buffer, const desc::BufferView& desc);

		Sampler				createSampler(const desc::Sampler& desc);

		GraphicsPipeline	createGraphicsPipeline(const desc::GraphicsPipeline& desc);
		ComputePipeline		createComputePipeline(const desc::ComputePipeline& desc);

		CommandBuffer		createCommandBuffer();
		void				submit(CommandBuffer& gfx);

		void				present(int syncInterval);

		int2				swapChainSize();

		void				shaderHotReload();
	private:
		friend class Texture;
		friend class TextureView;
		friend class Buffer;
		friend class BufferView;
		friend class Sampler;
		friend class CommandBuffer;
		friend class GraphicsPipeline;
		friend class ComputePipeline;
		friend class ShaderManager;

		std::shared_ptr<DeviceImpl>		pImpl;
		std::shared_ptr<ShaderManager>	m_shaderManager;
	};
}
