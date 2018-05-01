#include "ComputePipelineImpl.hpp"
#include "DeviceImpl.hpp"
#include "../Errors.hpp"

namespace graphics
{
	ComputePipelineImpl::ComputePipelineImpl(DeviceImpl& device, const desc::ComputePipeline& desc) :
		m_descriptor(desc)
	{
		// TODO
	}

	ComputePipelineImpl::~ComputePipelineImpl()
	{
		SAFE_RELEASE(m_shader);
	}
}
