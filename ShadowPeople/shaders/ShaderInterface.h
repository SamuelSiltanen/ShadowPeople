#ifndef SP_SHADER_INTERFACE_H
#define SP_SHADER_INTERFACE_H

#ifdef __cplusplus	// C++

#include "../Graphics.hpp"
#include "../Descriptors.hpp"
#include "../Errors.hpp"
#include "../Types.hpp"

#include <vector>

using uint		= unsigned;
using float4x4	= Matrix4x4;

namespace graphics
{
	namespace shaders
	{
		namespace detail
		{
			extern std::vector<Range<const uint8_t>>			cbs;
			extern std::vector<const graphics::ResourceView*>	srvs;
			extern std::vector<const graphics::ResourceView*>	uavs;
			extern std::vector<const graphics::Sampler*>		samplers;

			void resetBindings();
		}

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

		#define CBuffer(x, body) \
			struct Body##x body; \
			static_assert(sizeof(Body##x) % 16 == 0, "Size of a constant buffer must be multipler of 16 bytes."); \
			struct CB##x : public Body##x \
			{ \
				CB##x() \
				{ \
					Range<const uint8_t> data(reinterpret_cast<const uint8_t*>(this), sizeof(Body##x)); \
					index = static_cast<uint32_t>(detail::cbs.size()); \
					detail::cbs.emplace_back(data); \
				} \
			private: \
				uint32_t index; \
			}; \
			CB##x x

		#define SRV_CLASS(X, Y) \
			template<typename T> \
			struct X \
			{ \
				uint32_t index; \
				X() \
				{ \
					index = static_cast<uint32_t>(detail::srvs.size()); \
					detail::srvs.emplace_back(nullptr); \
				} \
				X& operator=(const graphics::Y& view) \
				{ \
					SP_ASSERT(view.descriptor().type == graphics::desc::ViewType::SRV, \
							  "Only SRVs can be bound to SRV slot."); \
					detail::srvs[index] = &view; \
					return *this; \
				} \
			};

		SRV_CLASS(Texture1D,		TextureView)
		SRV_CLASS(Texture1DArray,	TextureView)
		SRV_CLASS(Texture2D,		TextureView)
		SRV_CLASS(Texture2DArray,	TextureView)
		SRV_CLASS(Texture3D,		TextureView)
		SRV_CLASS(TextureCube,		TextureView)
		SRV_CLASS(TextureCubeArray, TextureView)

		SRV_CLASS(Buffer,				BufferView)
		SRV_CLASS(ByteAddressBuffer,	BufferView)
		SRV_CLASS(StructuredBuffer,		BufferView)

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
					SP_ASSERT(view.descriptor().type == graphics::desc::ViewType::UAV, \
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

		struct sampler
		{
			uint32_t index;
			sampler()
			{
				index = static_cast<uint32_t>(detail::samplers.size());
				detail::samplers.emplace_back(nullptr);
			}
			sampler& operator=(const graphics::Sampler& s)
			{
				detail::samplers[index] = &s;
				return *this;
			}
		};
	}
}
	// INTERFACE STRUCT

	#define BEGIN_SHADER_INTERFACE(X)	\
		namespace graphics \
		{ \
			namespace shaders \
			{ \
				struct X : public graphics::desc::ShaderBinding { \
					X(const X&) = delete; \
					X& operator=(const X&) = delete;
		
	#define END_SHADER_INTERFACE(X)		\
					const char* filename() const override { return __FILE__; } \
					ComputePipeline* computePipeline() override { return m_computePipeline; } \
					GraphicsPipeline* graphicsPipeline() override { return m_graphicsPipeline; } \
					const std::vector<Range<const uint8_t>> cbs() const override { return detail::cbs; } \
					const std::vector<const graphics::ResourceView*>& srvs() const override { return detail::srvs; } \
					const std::vector<const graphics::ResourceView*>& uavs() const override { return detail::uavs; } \
					const std::vector<const graphics::Sampler*>& samplers() const override { return detail::samplers; } \
					uint32_t threadGroupSizeX() const { return ThreadGroupSizeX; } \
					uint32_t threadGroupSizeY() const { return ThreadGroupSizeY; } \
					uint32_t threadGroupSizeZ() const { return ThreadGroupSizeZ; } \
					X() : \
						m_computePipeline(nullptr), \
						m_graphicsPipeline(nullptr) {} \
					X(ComputePipeline *computePipeline) : \
						m_computePipeline(computePipeline), \
						m_graphicsPipeline(nullptr) {} \
					X(GraphicsPipeline *graphicsPipeline) : \
						m_computePipeline(nullptr), \
						m_graphicsPipeline(graphicsPipeline) {} \
				private: \
					ComputePipeline*	m_computePipeline; \
					GraphicsPipeline*	m_graphicsPipeline; \
				}; \
			} \
		}

	#define THREAD_GROUP_SIZE(X, Y, Z)	\
		const uint ThreadGroupSizeX = X;		\
		const uint ThreadGroupSizeY = Y;		\
		const uint ThreadGroupSizeZ = Z;

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
