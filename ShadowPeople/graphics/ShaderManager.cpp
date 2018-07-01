#include "ShaderManager.hpp"
#include "Graphics.hpp"

#include "../Errors.hpp"

#include "../dx11/ShaderManagerImpl.hpp"

namespace graphics
{
	ShaderManager::ShaderManager(Device& device)
	{
		pImpl = std::make_shared<ShaderManagerImpl>(*device.pImpl);
	}

	bool ShaderManager::compile(Shader& shader)
	{		
		SP_EXPECT_NOT_NULL(pImpl, ERROR_CODE_SHADER_MANAGER_NULL);
		return pImpl->compile(*shader.pImpl);
	}

	void ShaderManager::hotReload()
	{
		SP_EXPECT_NOT_NULL(pImpl, ERROR_CODE_SHADER_MANAGER_NULL);
		pImpl->hotReload();
	}
}