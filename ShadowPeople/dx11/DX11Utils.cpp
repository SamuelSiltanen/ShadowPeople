#include "DX11Utils.hpp"
#include "../Errors.hpp"

namespace graphics
{
	void setUsageFlags(const desc::Usage& descUsage,
					   D3D11_USAGE& usage, UINT& cpuAccess, UINT& bind)
	{
		switch (descUsage)
		{
		case desc::Usage::CpuToGpuFrequent:
			usage		= D3D11_USAGE_DYNAMIC;
			cpuAccess	= D3D11_CPU_ACCESS_WRITE;
			bind		= 0;
			break;
		case desc::Usage::GpuToCpuReadback:
			usage		= D3D11_USAGE_STAGING;
			cpuAccess	= D3D11_CPU_ACCESS_READ;
			bind		= 0;
			break;
		case desc::Usage::RenderTarget:
			usage		= D3D11_USAGE_DEFAULT;
			cpuAccess	= 0;
			bind		= D3D11_BIND_SHADER_RESOURCE |
						  D3D11_BIND_RENDER_TARGET;
			break;
		case desc::Usage::DepthBuffer:
			usage		= D3D11_USAGE_DEFAULT;
			cpuAccess	= 0;
			bind		= D3D11_BIND_SHADER_RESOURCE |
						  D3D11_BIND_DEPTH_STENCIL;
			break;
		case desc::Usage::GpuReadOnly:
			usage		= D3D11_USAGE_IMMUTABLE;
			cpuAccess	= 0;
			bind		= D3D11_BIND_SHADER_RESOURCE;
			break;
		case desc::Usage::GpuReadWrite:
		default:
			usage		= D3D11_USAGE_DEFAULT;
			cpuAccess	= 0;
			bind		= D3D11_BIND_SHADER_RESOURCE |
						  D3D11_BIND_UNORDERED_ACCESS;
			break;
		}
	}

	DXGI_FORMAT dxgiFormat(const desc::Format& format)
	{
		switch (format.descriptor().bytes)
		{
		case desc::FormatBytesPerChannel::B8:
			switch (format.descriptor().channels)
			{
			case desc::FormatChannels::R:
				switch (format.descriptor().type)
				{
				case desc::FormatType::Typeless:	return DXGI_FORMAT_R8_TYPELESS;
				case desc::FormatType::UNorm:		return DXGI_FORMAT_R8_UNORM;
				case desc::FormatType::UInt:		return DXGI_FORMAT_R8_UINT;
				case desc::FormatType::SNorm:		return DXGI_FORMAT_R8_SNORM;
				case desc::FormatType::SInt:		return DXGI_FORMAT_R8_SINT;
				default:
					break;
				}
				break;
			case desc::FormatChannels::RG:
				switch (format.descriptor().type)
				{
				case desc::FormatType::Typeless:	return DXGI_FORMAT_R8G8_TYPELESS;
				case desc::FormatType::UNorm:		return DXGI_FORMAT_R8G8_UNORM;
				case desc::FormatType::UInt:		return DXGI_FORMAT_R8G8_UINT;
				case desc::FormatType::SNorm:		return DXGI_FORMAT_R8G8_SNORM;
				case desc::FormatType::SInt:		return DXGI_FORMAT_R8G8_SINT;
				default:
					break;
				}
				break;
			case desc::FormatChannels::RGBA:
				switch (format.descriptor().type)
				{
				case desc::FormatType::Typeless:	return DXGI_FORMAT_R8G8B8A8_TYPELESS;
				case desc::FormatType::UNorm:		return DXGI_FORMAT_R8G8B8A8_UNORM;
				case desc::FormatType::UInt:		return DXGI_FORMAT_R8G8B8A8_UINT;
				case desc::FormatType::SNorm:		return DXGI_FORMAT_R8G8B8A8_SNORM;
				case desc::FormatType::SInt:		return DXGI_FORMAT_R8G8B8A8_SINT;
				default:
					break;
				}
				break;
			case desc::FormatChannels::Depth:
				return DXGI_FORMAT_D16_UNORM;
			case desc::FormatChannels::RGB:
			case desc::FormatChannels::DepthAndStencil:
			default:
				break;
			}
			break;
		case desc::FormatBytesPerChannel::B16:
			switch (format.descriptor().channels)
			{
			case desc::FormatChannels::R:
				switch (format.descriptor().type)
				{
				case desc::FormatType::Typeless:	return DXGI_FORMAT_R16_TYPELESS;
				case desc::FormatType::Float:		return DXGI_FORMAT_R16_FLOAT;
				case desc::FormatType::UNorm:		return DXGI_FORMAT_R16_UNORM;
				case desc::FormatType::UInt:		return DXGI_FORMAT_R16_UINT;
				case desc::FormatType::SNorm:		return DXGI_FORMAT_R16_SNORM;
				case desc::FormatType::SInt:		return DXGI_FORMAT_R16_SINT;
				default:
					break;
				}
				break;
			case desc::FormatChannels::RG:
				switch (format.descriptor().type)
				{
				case desc::FormatType::Typeless:	return DXGI_FORMAT_R16G16_TYPELESS;
				case desc::FormatType::Float:		return DXGI_FORMAT_R16G16_FLOAT;
				case desc::FormatType::UNorm:		return DXGI_FORMAT_R16G16_UNORM;
				case desc::FormatType::UInt:		return DXGI_FORMAT_R16G16_UINT;
				case desc::FormatType::SNorm:		return DXGI_FORMAT_R16G16_SNORM;
				case desc::FormatType::SInt:		return DXGI_FORMAT_R16G16_SINT;
				default:
					break;
				}
				break;
			case desc::FormatChannels::RGBA:
				switch (format.descriptor().type)
				{
				case desc::FormatType::Typeless:	return DXGI_FORMAT_R16G16B16A16_TYPELESS;
				case desc::FormatType::Float:		return DXGI_FORMAT_R16G16B16A16_FLOAT;
				case desc::FormatType::UNorm:		return DXGI_FORMAT_R16G16B16A16_UNORM;
				case desc::FormatType::UInt:		return DXGI_FORMAT_R16G16B16A16_UINT;
				case desc::FormatType::SNorm:		return DXGI_FORMAT_R16G16B16A16_SNORM;
				case desc::FormatType::SInt:		return DXGI_FORMAT_R16G16B16A16_SINT;
				default:
					break;
				}
				break;
			case desc::FormatChannels::Depth:
				return DXGI_FORMAT_D16_UNORM;
			case desc::FormatChannels::RGB:
			case desc::FormatChannels::DepthAndStencil:
			default:
				break;
			}
			break;
		case desc::FormatBytesPerChannel::B32:
			switch (format.descriptor().channels)
			{
			case desc::FormatChannels::R:
				switch (format.descriptor().type)
				{
				case desc::FormatType::Typeless:	return DXGI_FORMAT_R32_TYPELESS;
				case desc::FormatType::Float:		return DXGI_FORMAT_R32_FLOAT;
				case desc::FormatType::UInt:		return DXGI_FORMAT_R32_UINT;
				case desc::FormatType::SInt:		return DXGI_FORMAT_R32_SINT;
				default:
					break;
				}
				break;
			case desc::FormatChannels::RG:
				switch (format.descriptor().type)
				{
				case desc::FormatType::Typeless:	return DXGI_FORMAT_R32G32_TYPELESS;
				case desc::FormatType::Float:		return DXGI_FORMAT_R32G32_FLOAT;
				case desc::FormatType::UInt:		return DXGI_FORMAT_R32G32_UINT;
				case desc::FormatType::SInt:		return DXGI_FORMAT_R32G32_SINT;
				default:
					break;
				}
				break;
			case desc::FormatChannels::RGB:
				switch (format.descriptor().type)
				{
				case desc::FormatType::Typeless:	return DXGI_FORMAT_R32G32B32_TYPELESS;
				case desc::FormatType::Float:		return DXGI_FORMAT_R32G32B32_FLOAT;
				case desc::FormatType::UInt:		return DXGI_FORMAT_R32G32B32_UINT;
				case desc::FormatType::SInt:		return DXGI_FORMAT_R32G32B32_SINT;
				default:
					break;
				}
				break;
			case desc::FormatChannels::RGBA:
				switch (format.descriptor().type)
				{
				case desc::FormatType::Typeless:	return DXGI_FORMAT_R32G32B32A32_TYPELESS;
				case desc::FormatType::Float:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
				case desc::FormatType::UInt:		return DXGI_FORMAT_R32G32B32A32_UINT;
				case desc::FormatType::SInt:		return DXGI_FORMAT_R32G32B32A32_SINT;
				default:
					break;
				}
				break;
			case desc::FormatChannels::Depth:
				return DXGI_FORMAT_D32_FLOAT;
			case desc::FormatChannels::DepthAndStencil:
				return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			default:
				break;
			}
			break;
		case desc::FormatBytesPerChannel::B24_8:

			break;
		case desc::FormatBytesPerChannel::B10_10_10_2:
			break;
		default:
			break;
		}

		// Fall back to unknown
		return DXGI_FORMAT_UNKNOWN;
	}

	desc::Format descFormat(DXGI_FORMAT format)
	{
		desc::Format f;
		
		// TODO: Parse format
		

		return desc::Format::unknown();
	}

	DXGI_SAMPLE_DESC dxgiSampleDesc(desc::Multisampling multisampling)
	{
		switch (multisampling)
		{
		case desc::Multisampling::MSx2:
			return { 2, static_cast<UINT>(D3D11_STANDARD_MULTISAMPLE_PATTERN) };
		case desc::Multisampling::MSx4:
			return { 4, static_cast<UINT>(D3D11_STANDARD_MULTISAMPLE_PATTERN) };
		case desc::Multisampling::MSx8:
			return { 8, static_cast<UINT>(D3D11_STANDARD_MULTISAMPLE_PATTERN) };
		case desc::Multisampling::MSx16:
			return { 16, static_cast<UINT>(D3D11_STANDARD_MULTISAMPLE_PATTERN) };
		case desc::Multisampling::None:
		default:
			return { 1, 0 };	// No multi-sampling
		}
	}

	desc::Multisampling descMultisampling(DXGI_SAMPLE_DESC sampleDesc)
	{
		SP_ASSERT((sampleDesc.Quality == D3D11_STANDARD_MULTISAMPLE_PATTERN) ||
				  ((sampleDesc.Count == 1) && (sampleDesc.Quality == 0)),
				  "Only standard multisampling pattern supported on DX11");

		switch (sampleDesc.Count)
		{		
			return desc::Multisampling::None;
		case 2:
			return desc::Multisampling::MSx2;
		case 4:
			return desc::Multisampling::MSx8;
		case 8:
			return desc::Multisampling::MSx16;
		case 1:
		default:
			// If given an unknown sample count, fall back to no multisampling
			return desc::Multisampling::None;
		}
	}

	D3D11_TEXTURE_ADDRESS_MODE textureAddressMode(const desc::SamplerCoordinateMode mode)
	{
		switch (mode)
		{
		case desc::SamplerCoordinateMode::Mirror:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		case desc::SamplerCoordinateMode::MirrorOnce:
			return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		case desc::SamplerCoordinateMode::Clamp:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		case desc::SamplerCoordinateMode::Border:
			return D3D11_TEXTURE_ADDRESS_BORDER;
		case desc::SamplerCoordinateMode::Wrap:
		default:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		}
	}

	D3D11_COMPARISON_FUNC comparisonFunc(const desc::ComparisonMode mode)
	{
		switch (mode)
		{
		case desc::ComparisonMode::Less:
			return D3D11_COMPARISON_LESS;
		case desc::ComparisonMode::LessEqual:
			return D3D11_COMPARISON_LESS_EQUAL;
		case desc::ComparisonMode::Greater:
			return D3D11_COMPARISON_GREATER;
		case desc::ComparisonMode::GreaterEqual:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case desc::ComparisonMode::Equal:
			return D3D11_COMPARISON_EQUAL;
		case desc::ComparisonMode::NotEqual:
			return D3D11_COMPARISON_NOT_EQUAL;
		case desc::ComparisonMode::Never:
			return D3D11_COMPARISON_NEVER;
		case desc::ComparisonMode::Always:
		default:
			return D3D11_COMPARISON_ALWAYS;
		}
	}

	D3D11_STENCIL_OP stencilOp(const desc::StencilOp op)
	{
		switch(op)
		{
		case desc::StencilOp::Zero:
			return D3D11_STENCIL_OP_ZERO;
		case desc::StencilOp::Replace:
			return D3D11_STENCIL_OP_REPLACE;
		case desc::StencilOp::IncrSat:
			return D3D11_STENCIL_OP_INCR_SAT;
		case desc::StencilOp::DecrSat:
			return D3D11_STENCIL_OP_DECR_SAT;
		case desc::StencilOp::Invert:
			return D3D11_STENCIL_OP_INVERT;
		case desc::StencilOp::Incr:
			return D3D11_STENCIL_OP_INCR;
		case desc::StencilOp::Decr:
			return D3D11_STENCIL_OP_DECR;
		case desc::StencilOp::Keep:
		default:
			return D3D11_STENCIL_OP_KEEP;
		}
	}

	D3D11_BLEND blendMode(const desc::BlendMode mode)
	{
		switch (mode)
		{
		case desc::BlendMode::One:
			return D3D11_BLEND_ONE;
		case desc::BlendMode::SrcColor:
			return D3D11_BLEND_SRC_COLOR;
		case desc::BlendMode::InvSrcColor:
			return D3D11_BLEND_INV_SRC_COLOR;
		case desc::BlendMode::SrcAlpha:
			return D3D11_BLEND_SRC_ALPHA;
		case desc::BlendMode::InvSrcAlpha:
			return D3D11_BLEND_INV_SRC_ALPHA;
		case desc::BlendMode::DstAlpha:
			return D3D11_BLEND_DEST_ALPHA;
		case desc::BlendMode::InvDestAlpha:
			return D3D11_BLEND_INV_DEST_ALPHA;
		case desc::BlendMode::DstColor:
			return D3D11_BLEND_DEST_COLOR;
		case desc::BlendMode::InvDstColor:
			return D3D11_BLEND_INV_DEST_COLOR;
		case desc::BlendMode::SrcAlphaSat:
			return D3D11_BLEND_SRC_ALPHA_SAT;
		case desc::BlendMode::BlendFactor:
			return D3D11_BLEND_BLEND_FACTOR;
		case desc::BlendMode::InvBlendFactor:
			return D3D11_BLEND_INV_BLEND_FACTOR;
		case desc::BlendMode::Zero:
		default:
			return D3D11_BLEND_ZERO;
		}
	}

	D3D11_BLEND_OP blendOp(const desc::BlendOp op)
	{
		switch (op)
		{
		case desc::BlendOp::Subtract:
			return D3D11_BLEND_OP_SUBTRACT;
		case desc::BlendOp::RevSubtract:
			return D3D11_BLEND_OP_REV_SUBTRACT;
		case desc::BlendOp::Min:
			return D3D11_BLEND_OP_MIN;
		case desc::BlendOp::Max:
			return D3D11_BLEND_OP_MAX;
		case desc::BlendOp::Add:
		default:
			return D3D11_BLEND_OP_ADD;
		}
	}
}
