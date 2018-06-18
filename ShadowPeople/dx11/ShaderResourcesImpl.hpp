#pragma once

#include <vector>
#include <memory>

namespace graphics
{
	class BufferImpl;
	class ResourceViewImpl;
	class SamplerImpl;

	struct ShaderResourcesImpl
	{
		std::vector<std::shared_ptr<BufferImpl>>	cbs;
		std::vector<const ResourceViewImpl*>		srvs;
		std::vector<const ResourceViewImpl*>		uavs;
		std::vector<const ResourceViewImpl*>		rtvs;
		const ResourceViewImpl*						dsv;
		std::vector<const SamplerImpl*>				samplers;
	};
}
