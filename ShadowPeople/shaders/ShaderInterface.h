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
		std::vector<Buffer*>		cbs;
		std::vector<ResourceView*>	srvs;
		std::vector<ResourceView*>	uavs;
		std::vector<Sampler*>		samplers;
	}

	#define UAV_CLASS(X, Y) \
		template<typename T> \
		struct X \
		{ \
			graphics::Y uav; \
			X() { detail::uavs.emplace_back(&uav); } \
			X& operator=(const graphics::Y& view) \
			{ \
				SP_ASSERT(view.descriptor().descriptor().type == graphics::desc::ViewType::UAV, \
						  "Only UAVs can be bound to UAV slot."); \
				uav = view; \
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
					const ComputePipeline* computePipeline() const override { reset(); return m_computePipeline; } \
					const GraphicsPipeline* graphicsPipeline() const override { reset(); return m_graphicsPipeline; } \
					const std::vector<graphics::Buffer*>& cbs() const { return detail::cbs; } \
					const std::vector<graphics::ResourceView*>& srvs() const { return detail::srvs; } \
					const std::vector<graphics::ResourceView*>& uavs() const { return detail::uavs; } \
					const std::vector<graphics::Sampler*>& samplers() const { return detail::samplers; } \
				private: \
					X(ComputePipeline *computePipeline) : \
						m_computePipeline(computePipeline), \
						m_graphicsPipeline(nullptr) {} \
					X(GraphicsPipeline *graphicsPipeline) : \
						m_computePipeline(nullptr), \
						m_graphicsPipeline(graphicsPipeline) {} \
					void reset() const \
					{ \
						detail::cbs.clear(); \
						detail::srvs.clear(); \
						detail::uavs.clear(); \
						detail::samplers.clear(); \
					} \
					friend class ComputePipeline; \
					friend class GraphicsPipeline; \
					ComputePipeline*	m_computePipeline; \
					GraphicsPipeline*	m_graphicsPipeline; \
				}; \
			} \
		}

	#define THREAD_GROUP_SIZE(X, Y, Z)	\
		const uint ThreadGroupsX = X;		\
		const uint ThreadGroupsY = Y;		\
		const uint ThreadGroupsZ = Z;
	
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
