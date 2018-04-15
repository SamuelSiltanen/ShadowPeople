#include "DX11Utils.hpp"

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
			bind		= D3D11_BIND_SHADER_RESOURCE;
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
		switch (format.bytes)
		{
		case desc::FormatBytesPerChannel::B8:
			switch (format.channels)
			{
			case desc::FormatChannels::R:
				switch (format.type)
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
				switch (format.type)
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
				switch (format.type)
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
			switch (format.channels)
			{
			case desc::FormatChannels::R:
				switch (format.type)
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
				switch (format.type)
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
				switch (format.type)
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
			switch (format.channels)
			{
			case desc::FormatChannels::R:
				switch (format.type)
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
				switch (format.type)
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
				switch (format.type)
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
				switch (format.type)
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
}
