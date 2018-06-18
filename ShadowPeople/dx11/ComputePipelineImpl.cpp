#include "ComputePipelineImpl.hpp"
#include "DeviceImpl.hpp"
#include "BufferImpl.hpp"
#include "ShaderManagerImpl.hpp"
#include "../Errors.hpp"

#include <d3d11.h>

namespace graphics
{
	ComputePipelineImpl::ComputePipelineImpl(DeviceImpl&					device,
											 ShaderManagerImpl&				shaderManager,
											 const desc::ComputePipeline&	desc) :
		m_descriptor(desc),
		m_shader(desc.descriptor().binding->filename(), desc::ShaderType::Compute)
	{
		if (shaderManager.compile(m_shader))
		{
			shaderManager.createComputeShader(m_shader);
		}
		
		shaderManager.createConstantBuffers(m_resources, *desc.descriptor().binding);

		m_resources.dsv = nullptr;

		
	}

	
}
