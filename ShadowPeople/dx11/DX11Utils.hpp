#pragma once

#include <d3d11.h>

#include "../graphics/Descriptors.hpp"

// We don't want to allow copying the implementation classes, 
// because that could mess up the reference counting by smart pointers
#define NO_COPY_CLASS(X)	X::X(const X&)			= delete; \
							X& operator=(const X&)	= delete;

#define NO_MOVE_CLASS(X)	X::X(X&&)			= delete; \
							X& operator=(X&&)	= delete;

#define SAFE_RELEASE(X) if (X) { X->Release(); X = nullptr; }

namespace graphics
{
	void setUsageFlags(const desc::Usage& descUsage,
					   D3D11_USAGE& usage, UINT& cpuAccess, UINT& bind);

	DXGI_FORMAT		dxgiFormat(const desc::Format& format);
	desc::Format	descFormat(DXGI_FORMAT format);

	DXGI_SAMPLE_DESC	dxgiSampleDesc(const desc::Multisampling multisampling);
	desc::Multisampling descMultisampling(DXGI_SAMPLE_DESC sampleDesc);

	D3D11_TEXTURE_ADDRESS_MODE textureAddressMode(const desc::SamplerCoordinateMode mode);

	D3D11_COMPARISON_FUNC comparisonFunc(const desc::ComparisonMode mode);
	D3D11_STENCIL_OP stencilOp(const desc::StencilOp op);

	D3D11_BLEND blendMode(const desc::BlendMode mode);
	D3D11_BLEND_OP blendOp(const desc::BlendOp op);
}
