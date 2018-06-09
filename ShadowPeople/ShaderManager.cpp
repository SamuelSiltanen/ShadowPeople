#include "ShaderManager.hpp"
#include "Errors.hpp"
#include "Graphics.hpp"

#include "dx11/ShaderManagerImpl.hpp"

namespace graphics
{
	ShaderManager::ShaderManager(Device& device)
	{
		pImpl = std::make_shared<ShaderManagerImpl>(*device.pImpl);
	}

	bool ShaderManager::compile(Shader& shader)
	{		
		return pImpl->compile(*shader.pImpl);
	}
}