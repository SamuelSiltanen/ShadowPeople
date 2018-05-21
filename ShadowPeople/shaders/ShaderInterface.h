#ifndef SP_SHADER_INTERFACE_H
#define SP_SHADER_INTERFACE_H

#ifdef __cplusplus	// C++

#include "../Graphics.hpp"
#include "../Descriptors.hpp"
#include "../Errors.hpp"

#include <vector>

// TYPES

using uint = unsigned;

// CONSTANT BUFFERS

#define CBuffer(x, body) struct body x

namespace graphics
{
	namespace detail
	{
		std::vector<const Buffer*>			cbs;
		std::vector<const ResourceView*>	srvs;
		std::vector<const ResourceView*>	uavs;
		std::vector<const Sampler*>			samplers;

		void resetBindings()
		{
			cbs.clear();
			srvs.clear();
			uavs.clear();
			samplers.clear();
		}
	}

	#define UAV_CLASS(X, Y) \
		template<typename T> \
		struct X \
		{ \
			uint32_t index; \
			X() \
			{ \
				index = static_cast<uint32_t>(detail::uavs.size()); \
				detail::uavs.emplace_back(nullptr); \
			} \
			X& operator=(const graphics::Y& view) \
			{ \
				SP_ASSERT(view.descriptor().descriptor().type == graphics::desc::ViewType::UAV, \
						  "Only UAVs can be bound to UAV slot."); \
				detail::uavs[index] = &view; \
				return *this; \
			} \
		};

	UAV_CLASS(RWTexture1D,		TextureView)
	UAV_CLASS(RWTexture1DArray, TextureView)
	UAV_CLASS(RWTexture2D,		TextureView)
	UAV_CLASS(RWTexture2DArray, TextureView)
	UAV_CLASS(RWTexture3D,		TextureView)

	UAV_CLASS(RWBuffer,				BufferView)
	UAV_CLASS(RWByteAddressBuffer,	BufferView)
	UAV_CLASS(RWStructuredBuffer,	BufferView)
}
	// INTERFACE STRUCT

	#define BEGIN_SHADER_INTERFACE(X)	\
		namespace graphics \
		{ \
			namespace shaders \
			{ \
				class I##X : public graphics::desc::ShaderInterface \
				{ \
				public: \
					const char* filename() const override { return __FILE__; } \
				}; \
				I##X i##X;\
				struct X : public graphics::desc::ShaderBinding {
		
	#define END_SHADER_INTERFACE(X)		\
					ComputePipeline* computePipeline() override { return m_computePipeline; } \
					GraphicsPipeline* graphicsPipeline() override { return m_graphicsPipeline; } \
					const std::vector<const graphics::Buffer*>& cbs() const override { return detail::cbs; } \
					const std::vector<const graphics::ResourceView*>& srvs() const override { return detail::srvs; } \
					const std::vector<const graphics::ResourceView*>& uavs() const override { return detail::uavs; } \
					const std::vector<const graphics::Sampler*>& samplers() const override { return detail::samplers; } \
					uint32_t threadGroupSizeX() const { return ThreadGroupSizeX; } \
					uint32_t threadGroupSizeY() const { return ThreadGroupSizeY; } \
					uint32_t threadGroupSizeZ() const { return ThreadGroupSizeZ; } \
				private: \
					X(ComputePipeline *computePipeline) : \
						m_computePipeline(computePipeline), \
						m_graphicsPipeline(nullptr) {} \
					X(GraphicsPipeline *graphicsPipeline) : \
						m_computePipeline(nullptr), \
						m_graphicsPipeline(graphicsPipeline) {} \
					friend class ComputePipeline; \
					friend class GraphicsPipeline; \
					ComputePipeline*	m_computePipeline; \
					GraphicsPipeline*	m_graphicsPipeline; \
				}; \
			} \
		}

	#define THREAD_GROUP_SIZE(X, Y, Z)	\
		const uint ThreadGroupSizeX = X;		\
		const uint ThreadGroupSizeY = Y;		\
		const uint ThreadGroupSizeZ = Z;
	
	// Possible bindings:
	// cbuffer TypeName
	// TextureXD<>
	// RWTextureXD<>
	// TextureXDArray<>
	// RWTextureXDArray<>
	// TextureCube<>
	// Buffer<>
	// RWBuffer<>
	// StructuredBuffer<>
	// RWStructuredBuffer<>
	// ByteAddressBuffer
	// RWByteAddressBuffer
	// sampler
#else				// HLSL

#define CBuffer(x, body) cbuffer x body

#define BEGIN_SHADER_INTERFACE(X) 

#define END_SHADER_INTERFACE(X)

#define THREAD_GROUP_SIZE(X, Y, Z) \
	static const uint ThreadGroupsX = X; \
	static const uint ThreadGroupsY = Y; \
	static const uint ThreadGroupsZ = Z;

#endif

#endif
