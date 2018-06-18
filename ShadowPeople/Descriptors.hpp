#pragma once

#include "Types.hpp"

#include <vector>
#include <memory>

namespace graphics
{
	struct Subresource
	{
		int mipLevel;
		int arraySlice;
	};

	class Buffer;
	class ResourceView;
	class Sampler;
	class GraphicsPipeline;
	class ComputePipeline;

	namespace desc
	{
		enum class Dimension
		{
			Texture1D,
			Texture2D,
			Texture3D,
			TextureCube
		};

		enum class Usage
		{
			GpuReadOnly,
			GpuReadWrite,
			RenderTarget,
			DepthBuffer,
			GpuToCpuReadback,
			CpuToGpuFrequent
		};

		enum class Multisampling
		{
			None,
			MSx2,
			MSx4,
			MSx8,
			MSx16
		};

		enum class BufferType
		{
			Regular,
			Vertex,
			Index,
			Constant
		};

		enum class ViewType
		{
			SRV,
			UAV,
			RTV,
			DSV
		};

		enum class SamplerType
		{
			Point,
			Bilinear,
			Trilinear,
			Anisotropic,
			PointComparison,
			BilinearComparison,
			TrilinearComparison,
			AnisotropicComparison,
		};

		enum class SamplerCoordinateMode
		{
			Wrap,
			Mirror,
			Clamp,
			Border,
			MirrorOnce
		};

		enum class ShaderType
		{
			Vertex,
			Pixel,
			Compute
		};

		enum class ComparisonMode
		{
			Never,
			Always,
			Equal,
			NotEqual,
			Less,
			LessEqual,
			Greater,
			GreaterEqual
		};

		enum class FormatChannels
		{
			R,
			RG,
			RGB,
			RGBA,
			Depth,
			DepthAndStencil
		};

		enum class FormatBytesPerChannel
		{
			B8,
			B16,
			B32,
			B24_8,
			B10_10_10_2
		};

		enum class FormatType
		{
			Unknown,
			Typeless,
			Float,
			UNorm,
			SNorm,
			UInt,
			SInt,
			BlockCompressed
		};

		struct Format
		{			
			FormatChannels			channels	= FormatChannels::RGBA;
			FormatBytesPerChannel	bytes		= FormatBytesPerChannel::B8;
			FormatType				type		= FormatType::UNorm;

			bool operator==(const Format& f) const
			{
				if (channels != f.channels) return false;
				if (bytes != f.bytes) return false;
				if (type != f.type) return false;
				return true;
			}

			uint32_t byteWidth() const
			{
				uint32_t channelFactor = 
					(channels == FormatChannels::RG) ? 2 :
					(channels == FormatChannels::RGB) ? 3 :
					(channels == FormatChannels::RGBA) ? 4 :
					1;
				uint32_t byteFactor = 
					(bytes == FormatBytesPerChannel::B8) ? 1 :
					(bytes == FormatBytesPerChannel::B16) ? 2 :
					4;
				return channelFactor * byteFactor;
			}

			static Format unknown()
			{
				Format f;
				f.channels	= FormatChannels::RGBA;
				f.bytes		= FormatBytesPerChannel::B32;
				f.type		= FormatType::Unknown;
				return f;
			}			
		};

		enum class StencilOp
		{
			Keep,
			Zero,
			Replace,
			IncrSat,
			DecrSat,
			Invert,
			Incr,
			Decr
		};

		struct StencilOpDesc
		{
			StencilOp		failOp		= StencilOp::Keep;
			StencilOp		depthFailOp	= StencilOp::Keep;
			StencilOp		passOp		= StencilOp::Replace;
			ComparisonMode	func		= ComparisonMode::Equal;
		};

		enum class FaceWinding
		{
			Front,
			Back
		};

		enum class PrimitiveTopology
		{
			Undefined,
			PointList,
			LineList,
			LineStrip,
			TriangleList,
			TriangleStrip
		};

		struct DepthStencilState
		{
		public:
			struct Descriptor
			{
				bool			depthTestEnable;
				bool			depthWriteEnable;
				ComparisonMode	depthFunc;
				bool			stencilEnable;
				uint8_t			stencilReadMask;
				uint8_t			stencilWriteMask;
				StencilOpDesc	stencilFrontFace;
				StencilOpDesc	stencilBackFace;
				uint32_t		stencilRef;
			};

			DepthStencilState()
			{
				desc.depthTestEnable	= true;
				desc.depthWriteEnable	= true;
				desc.depthFunc			= ComparisonMode::Greater;
				desc.stencilEnable		= false;
				desc.stencilReadMask	= 0xff;
				desc.stencilWriteMask	= 0xff;
				desc.stencilRef			= 0;
			}

			const Descriptor& descriptor() const	{ return desc; }

			DepthStencilState& depthTestingEnable(bool e) { desc.depthTestEnable = e; return *this; }
			DepthStencilState& depthWriteEnable(bool e) { desc.depthWriteEnable = e; return *this; }
			DepthStencilState& depthFunc(ComparisonMode m) { desc.depthFunc = m; return *this; }
			DepthStencilState& stencilEnable(bool e) { desc.stencilEnable = e; return *this; }
			DepthStencilState& stencilReadMask(uint8_t m) { desc.stencilReadMask = m; return *this; }
			DepthStencilState& stencilWriteMask(uint8_t m) { desc.stencilWriteMask = m; return *this; }
			DepthStencilState& stencilFailOp(StencilOp o, FaceWinding f)
			{
				if (f == FaceWinding::Front) desc.stencilFrontFace.failOp = o;
				else desc.stencilBackFace.failOp = o;
				return *this; 
			}
			DepthStencilState& stencilDepthFailOp(StencilOp o, FaceWinding f)
			{
				if (f == FaceWinding::Front) desc.stencilFrontFace.depthFailOp = o;
				else desc.stencilBackFace.depthFailOp = o;
				return *this;
			}
			DepthStencilState& stencilPassOp(StencilOp o, FaceWinding f)
			{
				if (f == FaceWinding::Front) desc.stencilFrontFace.passOp = o;
				else desc.stencilBackFace.passOp = o;
				return *this;
			}
			DepthStencilState& stencilFunc(ComparisonMode m, FaceWinding f)
			{
				if (f == FaceWinding::Front) desc.stencilFrontFace.func = m;
				else desc.stencilBackFace.func = m;
				return *this;
			}
			DepthStencilState& stencilRef(uint32_t r) { desc.stencilRef = r; return *this; }
		private:
			Descriptor desc;
		};

		enum class BlendMode
		{
			Zero,
			One,
			SrcColor,
			InvSrcColor,
			SrcAlpha,
			InvSrcAlpha,
			DstAlpha,
			InvDestAlpha,
			DstColor,
			InvDstColor,
			SrcAlphaSat,
			BlendFactor,
			InvBlendFactor
		};

		enum class BlendOp
		{
			Add,
			Subtract,
			RevSubtract,
			Min,
			Max
		};

		constexpr uint32_t MaxNumRenderTargets = 8;

		class BlendState
		{
		public:
			struct RenderTargetBlend
			{
				bool		enabled				= false;
				BlendMode	source				= BlendMode::One;
				BlendMode	destination			= BlendMode::Zero;
				BlendOp		blendOp				= BlendOp::Add;
				BlendMode	sourceAlpha			= BlendMode::One;
				BlendMode	destinationAlpha	= BlendMode::Zero;
				BlendOp		blendOpAlpha		= BlendOp::Add;
				uint8_t		writeMask			= 0x0f;
			};

			struct Descriptor
			{
				RenderTargetBlend	renderTargetBlend[MaxNumRenderTargets];
				Color4				blendFactor;
			};

			BlendState()
			{
				desc.blendFactor = { 1.f, 1.f, 1.f, 1.f };
			}

			const Descriptor& descriptor() const	{ return desc; }

			BlendState& enabled(bool e, uint32_t n = 0) { desc.renderTargetBlend[n].enabled = e; return *this; }
			BlendState& source(BlendMode m, uint32_t n = 0) { desc.renderTargetBlend[n].source = m; return *this; }
			BlendState& destination(BlendMode m, uint32_t n = 0) { desc.renderTargetBlend[n].destination = m; return *this; }
			BlendState& blendOp(BlendOp o, uint32_t n = 0) { desc.renderTargetBlend[n].blendOp = o; return *this; }
			BlendState& sourceAlpha(BlendMode m, uint32_t n = 0) { desc.renderTargetBlend[n].sourceAlpha = m; return *this; }
			BlendState& destinationAlpha(BlendMode m, uint32_t n = 0) { desc.renderTargetBlend[n].destinationAlpha = m; return *this; }
			BlendState& blendOpAlpha(BlendOp o, uint32_t n = 0) { desc.renderTargetBlend[n].blendOpAlpha = o; return *this; }
			BlendState& writeMask(uint8_t m, uint32_t n = 0) { desc.renderTargetBlend[n].writeMask = m; return *this; }
			BlendState& blendFactor(Color4 f) { desc.blendFactor = f; return *this; }
		private:
			Descriptor desc;
		};

		enum class FillMode
		{
			Solid,
			WireFrame
		};

		enum class CullMode
		{
			None,
			Front,
			Back
		};

		class RasterizerState
		{
		public:
			struct Descriptor
			{
				FillMode	fillMode;
				CullMode	cullMode;
				int			depthBias;
				float		depthBiasClamp;
				float		depthBiasSlopeScale;
			};

			RasterizerState()
			{
				desc.fillMode = FillMode::Solid;
				desc.cullMode = CullMode::Back;
				desc.depthBias = 0;
				desc.depthBiasClamp = 0;
				desc.depthBiasSlopeScale = 0;
			}

			const Descriptor& descriptor() const { return desc; }

			RasterizerState& fillMode(FillMode m) { desc.fillMode = m; return *this; }
			RasterizerState& cullMode(CullMode m) { desc.cullMode = m; return *this; }
			RasterizerState& depthBias(int b) { desc.depthBias = b; return *this; }
			RasterizerState& depthBiasClamp(float c) { desc.depthBiasClamp = c; return *this; }
			RasterizerState& depthBiasSlopeScale(float s) { desc.depthBiasSlopeScale = s; return *this; }
		private:
			Descriptor desc;
		};

		class Texture
		{
		public:
			struct Descriptor
			{
				Dimension		dimension;
				int				width;
				int				height;
				int				depth;
				int				mipLevels;
				int				arraySize;
				Format			format;
				Multisampling	multisampling;
				Usage			usage;
			};

			Texture()
			{
				desc.dimension		= Dimension::Texture2D;
				desc.width			= 256;
				desc.height			= 256;
				desc.depth			= 1;
				desc.mipLevels		= 1;
				desc.arraySize		= 1;
				desc.format			= Format();
				desc.multisampling	= Multisampling::None;
				desc.usage			= Usage::GpuReadWrite;
			}

			const Descriptor& descriptor() const	{ return desc; }

			Texture& dimension(Dimension d)			{ desc.dimension = d; return *this; }
			Texture& width(int w)					{ desc.width = w; return *this; }
			Texture& height(int h)					{ desc.height = h; return *this; }
			Texture& depth(int d)					{ desc.depth = d; return *this; }
			Texture& mipLevels(int m)				{ desc.mipLevels = m; return *this; }
			Texture& arraySize(int a)				{ desc.arraySize = a; return *this; }
			Texture& format(Format f)				{ desc.format = f; return *this; }
			Texture& multisampling(Multisampling m) { desc.multisampling = m; return *this; }
			Texture& usage(Usage u)					{ desc.usage = u; return *this; }
		private:
			Descriptor desc;
		};

		class TextureView
		{
		public:
			struct Descriptor
			{
				ViewType	type;
				Format		format;
				Dimension	dimension;
				int			firstMip;
				int			numMips;
				int			firstSlice;
				int			numSlices;
			};

			TextureView(const Texture& texture)
			{
				desc.type		= ViewType::SRV;
				desc.format		= texture.descriptor().format;
				desc.dimension	= texture.descriptor().dimension;
				desc.firstMip	= 0;
				desc.numMips	= texture.descriptor().mipLevels;
				desc.firstSlice = 0;
				desc.numSlices	= texture.descriptor().arraySize;
			}

			const Descriptor& descriptor() const { return desc; }

			TextureView& type(ViewType t)		{ desc.type = t; return *this; }
			TextureView& format(Format f)		{ desc.format = f; return *this; }
			TextureView& dimension(Dimension d)	{ desc.dimension = d; return *this; }
			TextureView& firstMip(int f)		{ desc.firstMip = f; return *this; }
			TextureView& numMips(int n)			{ desc.numMips = n; return *this; }
			TextureView& firstSlice(int f)		{ desc.firstSlice = f; return *this; }
			TextureView& numSlices(int n)		{ desc.numSlices = n; return *this; }
		private:
			Descriptor desc;
		};

		class Buffer
		{
		public:
			struct Descriptor
			{
				BufferType	type;
				Format		format;
				int			elements;
				Usage		usage;
				int			stride;
				bool		indirectArgs;
				bool		raw;
				bool		structured;
			};

			Buffer()
			{
				desc.type			= BufferType::Regular;
				desc.format			= Format::unknown();
				desc.elements		= 65536;
				desc.usage			= Usage::GpuReadWrite;
				desc.stride			= Format::unknown().byteWidth();
				desc.indirectArgs	= false;
				desc.raw			= false;
				desc.structured		= false;
			}

			const Descriptor& descriptor() const { return desc; }

			Buffer& type(BufferType t)		{ desc.type = t; return *this; }
			Buffer& elements(int e)			{ desc.elements = e; return *this; }
			Buffer& usage(Usage u)			{ desc.usage = u; return *this; }
			Buffer& format(Format f)		{ desc.format = f; desc.stride = 0; return *this; }
			template<typename T>
			Buffer& format(int s)			{ desc.format = Format::unknown(); desc.stride = sizeof(T); return *this; }
			Buffer& indirectArgs(bool i)	{ desc.indirectArgs = i; return *this; }
			Buffer& raw(bool r)				{ desc.raw = r; return *this; }
			Buffer& structured(bool s)		{ desc.structured = s; return *this; }
		private:
			Descriptor desc;
		};

		class BufferView
		{
		public:
			struct Descriptor
			{
				ViewType	type;
				int			firstElement;
				int			numElements;
				bool		raw;
				bool		append;
				bool		counter;
			};

			BufferView(const Buffer& buffer)
			{
				desc.type			= ViewType::SRV;
				desc.firstElement	= 0;
				desc.numElements	= buffer.descriptor().elements;
				desc.raw			= false;
				desc.append			= false;
				desc.counter		= false;
			}

			const Descriptor& descriptor() const { return desc; }

			BufferView& type(ViewType v)	{ desc.type = v; return *this; }
			BufferView& firstElement(int f)	{ desc.firstElement = f; return *this; }
			BufferView& numElements(int n)	{ desc.numElements = n; return *this; }
			BufferView& raw()				{ desc.raw = true; return *this; }
			BufferView& append()			{ desc.append = true; return *this; }
			BufferView& counter()			{ desc.counter = true; return *this; }
		private:
			Descriptor desc;
		};

		class Sampler
		{
		public:
			struct Descriptor
			{
				SamplerType				type;
				SamplerCoordinateMode	uMode;
				SamplerCoordinateMode	vMode;
				SamplerCoordinateMode	wMode;
				float					mipBias;
				float					mipMin;
				float					mipMax;
				int						anisotropy;
				ComparisonMode			compFunc;
				Color4					borderColor;
			};

			Sampler()
			{
				desc.type			= SamplerType::Bilinear;
				desc.uMode			= SamplerCoordinateMode::Wrap;
				desc.vMode			= SamplerCoordinateMode::Wrap;
				desc.wMode			= SamplerCoordinateMode::Wrap;
				desc.mipBias		= 0.f;
				desc.mipMin			= 0.f;
				desc.mipMax			= FLT_MAX;
				desc.anisotropy		= 1;
				desc.compFunc		= ComparisonMode::GreaterEqual;
				desc.borderColor	= { 0.f, 0.f, 0.f, 0.f };
			}

			const Descriptor& descriptor() const { return desc; }

			Sampler& type(SamplerType t)			{ desc.type = t; return *this; }
			Sampler& uMode(SamplerCoordinateMode m) { desc.uMode = m; return *this; }
			Sampler& vMode(SamplerCoordinateMode m) { desc.vMode = m; return *this; }
			Sampler& wMode(SamplerCoordinateMode m) { desc.wMode = m; return *this; }
			Sampler& mipBias(float b)				{ desc.mipBias = b; return *this; }
			Sampler& mipMin(float m)				{ desc.mipMin = m; return *this; }
			Sampler& mipMax(float m)				{ desc.mipMax = m; return *this; }
			Sampler& anisotropy(int a)				{ desc.anisotropy = a; return *this; }
			Sampler& compFunc(ComparisonMode c)		{ desc.compFunc = c; return *this; }
			Sampler& borderColor(Color4 c)			{ desc.borderColor = c; return *this; }
		private:
			Descriptor desc;
		};

		struct ShaderBinding
		{
			virtual const char* filename() const = 0;
			virtual graphics::ComputePipeline* computePipeline() = 0;
			virtual graphics::GraphicsPipeline* graphicsPipeline() = 0;
			virtual const std::vector<Range<const uint8_t>> cbs() const = 0;
			virtual const std::vector<const graphics::ResourceView*>& srvs() const = 0;
			virtual const std::vector<const graphics::ResourceView*>& uavs() const = 0;
			virtual const std::vector<const graphics::Sampler*>& samplers() const = 0;
			virtual uint32_t threadGroupSizeX() const = 0;
			virtual uint32_t threadGroupSizeY() const = 0;
			virtual uint32_t threadGroupSizeZ() const = 0;
		};

		class GraphicsPipeline
		{
		public:
			struct Descriptor
			{
				std::shared_ptr<ShaderBinding>	binding;
				DepthStencilState				depthStencilState;
				BlendState						blendState;
				RasterizerState					rasterizerState;
				PrimitiveTopology				primitiveTopology;
				uint32_t						numRenderTargets;
			};

			GraphicsPipeline()
			{
				desc.binding = nullptr;
				desc.numRenderTargets = 1;
			}

			const Descriptor& descriptor() const { return desc; }

			template<typename T>
			GraphicsPipeline& binding()
			{
				shaders::detail::resetBindings();
				desc.binding = std::make_shared<T>();
				return *this;
			}

			GraphicsPipeline& depthStencilState(const DepthStencilState& s) { desc.depthStencilState = s; return *this; }
			GraphicsPipeline& blendState(const BlendState& s) { desc.blendState = s; return *this; }
			GraphicsPipeline& rasterizerState(const RasterizerState& s) { desc.rasterizerState = s; return *this; }
			GraphicsPipeline& setPrimitiveTopology(const PrimitiveTopology t) { desc.primitiveTopology = t; return *this; }
			GraphicsPipeline& numRenderTargets(uint32_t n) { desc.numRenderTargets = n; return *this; }
		private:
			Descriptor desc;
		};
		
		class ComputePipeline
		{
		public:
			struct Descriptor
			{
				std::shared_ptr<ShaderBinding> binding;
			};

			ComputePipeline()
			{
				desc.binding = nullptr;
			}

			const Descriptor& descriptor() const { return desc; }

			template<typename T>
			ComputePipeline& binding()
			{
				shaders::detail::resetBindings();
				desc.binding = std::make_shared<T>();
				return *this;
			}
		private:
			Descriptor desc;
		};
	}
}
