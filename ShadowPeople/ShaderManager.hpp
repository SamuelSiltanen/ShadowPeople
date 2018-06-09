#pragma once

#include <memory>

#include "Descriptors.hpp"

namespace graphics
{
	class Device;
	class ShaderManagerImpl;
	class Shader;

	class ShaderManager
	{
	public:
		ShaderManager(Device& device);

		//void		registerShader();
		//const void* compiledByteCode();

		bool compile(Shader& shader);

	private:
		friend class ComputePipeline;
		friend class GraphicsPipeline;

		std::shared_ptr<ShaderManagerImpl> pImpl;
	};
}
