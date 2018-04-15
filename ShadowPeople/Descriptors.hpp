#pragma once

namespace graphics
{
	struct Subresource
	{
		int mipLevel;
		int arraySlice;
	};

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

			static Format unknown()
			{
				Format f;
				f.channels	= FormatChannels::RGBA;
				f.bytes		= FormatBytesPerChannel::B32;
				f.type		= FormatType::Unknown;
				return f;
			}
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
				desc.mipLevels		= 0;
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
				desc.stride			= 4;
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
	}
}
