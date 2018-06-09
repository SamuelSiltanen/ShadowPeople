#include "ComputePipelineImpl.hpp"
#include "DeviceImpl.hpp"
#include "BufferImpl.hpp"
#include "ShaderManagerImpl.hpp"
#include "../Errors.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>	// TODO: Move the compilation into ShaderManager

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
		
		createConstantBuffers(device, desc);
	}

	void ComputePipelineImpl::createConstantBuffers(DeviceImpl& device, const desc::ComputePipeline& desc)
	{
		for (auto cb : desc.descriptor().binding->cbs())
		{
			uint32_t byteSize = static_cast<uint32_t>(cb.byteSize());
			SP_ASSERT(byteSize % 16 == 0, "Size of a constant buffer must be multipler of 16 bytes.");
			uint32_t numElems = byteSize / 16;
			auto desc = desc::Buffer().type(desc::BufferType::Constant)
				.elements(numElems).usage(desc::Usage::CpuToGpuFrequent);
			std::shared_ptr<BufferImpl> constantBuf = std::make_shared<BufferImpl>(device, desc);
			m_resources.cbs.emplace_back(constantBuf);
		}
	}
}
