/*
    Copyright 2018 Samuel Siltanen
    ShaderManagerImpl.hpp
*/

#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"

#include <unordered_map>

namespace graphics
{
	class DeviceImpl;
	class ShaderImpl;
	struct ShaderResourcesImpl;

	class ShaderManagerImpl
	{
	public:
		ShaderManagerImpl(DeviceImpl& device);

		NO_COPY_CLASS(ShaderManagerImpl);

		bool compile(ShaderImpl& shader);
		void hotReload();

		void createComputeShader(ShaderImpl& shader);
		void createVertexShader(ShaderImpl& shader);
		void createPixelShader(ShaderImpl& shader);
		void createConstantBuffers(ShaderResourcesImpl& resources, const desc::ShaderBinding& binding);
	private:
		std::string getShaderName(const std::string& bindingName, desc::ShaderType type);
		std::string getShaderTarget(desc::ShaderType type);
        void setShaderDebugName(ShaderImpl& shader);

		DeviceImpl& m_device;

		struct ShaderReference
		{
			ShaderImpl*	shader;
			uint64_t	compilationTime;
		};

		std::unordered_map<std::string, ShaderReference>	m_shaderNameToShader;
	};
}
