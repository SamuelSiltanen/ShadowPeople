#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"

namespace graphics
{
	class DeviceImpl;

	class ComputePipelineImpl
	{
	public:
		ComputePipelineImpl(DeviceImpl& device, const desc::ComputePipeline& desc);
		~ComputePipelineImpl();

		NO_COPY_CLASS(ComputePipelineImpl);

		const desc::ComputePipeline& descriptor() const { return m_descriptor; }
	private:
		ID3D11ComputeShader*	m_shader;

		desc::ComputePipeline	m_descriptor;
	};
}
