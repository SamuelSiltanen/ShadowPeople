/*
    Copyright 2018 Samuel Siltanen
    ShaderResourcesImpl.hpp
*/

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
		std::vector<const ResourceViewImpl*>	    srvs;
		std::vector<const ResourceViewImpl*>		uavs;
		std::vector<const SamplerImpl*>				samplers;
	};
}
