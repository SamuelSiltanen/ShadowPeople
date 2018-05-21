#pragma once

#include <vector>
#include <memory>

namespace graphics
{
	class BufferImpl;
	class ResourceViewImpl;
	class SamplerImpl;

	struct ShaderResources
	{
		std::vector<const BufferImpl*>			cbs;
		std::vector<const ResourceViewImpl*>	srvs;
		std::vector<const ResourceViewImpl*>	uavs;
		std::vector<const SamplerImpl*>			samplers;
	};
}
