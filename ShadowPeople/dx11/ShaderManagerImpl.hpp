#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"
#include "DeviceImpl.hpp"
#include "ShaderImpl.hpp"

namespace graphics
{
	class ShaderManagerImpl
	{
	public:
		ShaderManagerImpl(DeviceImpl& device);

		NO_COPY_CLASS(ShaderManagerImpl);

		bool compile(ShaderImpl& shader);
		void createComputeShader(ShaderImpl& shader);
		void createVertexShader(ShaderImpl& shader);
		void createPixelShader(ShaderImpl& shader);
	private:
		std::string getShaderName(const std::string& bindingName, desc::ShaderType type);
		std::string getShaderTarget(desc::ShaderType type);

		DeviceImpl& m_device;

		// TODO: Mapping of shaders
	};
}
