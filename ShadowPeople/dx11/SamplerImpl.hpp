#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"

namespace graphics
{
	class DeviceImpl;

	class SamplerImpl
	{
	public:
		SamplerImpl(DeviceImpl& device, const desc::Sampler& desc);

		NO_COPY_CLASS(SamplerImpl)

		const desc::Sampler& descriptor() const { return m_descriptor; }
	private:
		ID3D11SamplerState*	m_sampler;
		desc::Sampler		m_descriptor;
	};
}
