#include "GraphicsPipelineImpl.hpp"
#include "DeviceImpl.hpp"
#include "ShaderManagerImpl.hpp"
#include "../Errors.hpp"

namespace graphics
{
	GraphicsPipelineImpl::GraphicsPipelineImpl(DeviceImpl& device,
											   ShaderManagerImpl& shaderManager,
											   const desc::GraphicsPipeline& desc) :
		m_descriptor(desc),
		m_vertexShader(desc.descriptor().binding->filename(), desc::ShaderType::Vertex),
		m_pixelShader(desc.descriptor().binding->filename(), desc::ShaderType::Pixel)
	{
		if (shaderManager.compile(m_vertexShader))
		{
			shaderManager.createVertexShader(m_vertexShader);
		}
		if (shaderManager.compile(m_pixelShader))
		{
			shaderManager.createPixelShader(m_pixelShader);
		}
	}
}
