#include "GraphicsPipelineImpl.hpp"
#include "DeviceImpl.hpp"
#include "../Errors.hpp"

namespace graphics
{
	GraphicsPipelineImpl::GraphicsPipelineImpl(DeviceImpl& device, const desc::GraphicsPipeline& desc) :
		m_descriptor(desc)
	{
		// TODO
	}

	GraphicsPipelineImpl::~GraphicsPipelineImpl()
	{
		SAFE_RELEASE(m_vertexShader);
		SAFE_RELEASE(m_pixelShader);
	}
}
