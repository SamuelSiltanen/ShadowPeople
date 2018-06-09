#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"
#include "ShaderResourcesImpl.hpp"
#include "ShaderImpl.hpp"

namespace graphics
{
	class DeviceImpl;
	class ShaderManagerImpl;

	class GraphicsPipelineImpl
	{
	public:
		GraphicsPipelineImpl(DeviceImpl&					device,
							 ShaderManagerImpl&				shaderManager,
							 const desc::GraphicsPipeline&	desc);

		NO_COPY_CLASS(GraphicsPipelineImpl)

		const desc::GraphicsPipeline& descriptor() const { return m_descriptor; }
	private:
		ShaderImpl				m_vertexShader;
		ShaderImpl				m_pixelShader;
		ShaderResourcesImpl		m_resources;
		desc::GraphicsPipeline	m_descriptor;
	};
}
