#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"
#include "ShaderResourcesImpl.hpp"

namespace graphics
{
	class DeviceImpl;
	class BufferImpl;

	class ComputePipelineImpl
	{
	public:
		ComputePipelineImpl(DeviceImpl& device, const desc::ComputePipeline& desc);
		~ComputePipelineImpl();

		NO_COPY_CLASS(ComputePipelineImpl);

		const desc::ComputePipeline& descriptor() const { return m_descriptor; }
		ID3D11ComputeShader* shader() const { return m_shader; }
		ShaderResourcesImpl& resources() { return m_resources; }
	private:
		std::string getComputeShaderName(const desc::ComputePipeline& desc);
		ID3DBlob* compileShader(const std::string& shaderName);
		void createComputeShader(DeviceImpl& device, ID3DBlob* shaderBlob);
		void createConstantBuffers(DeviceImpl& device, const desc::ComputePipeline& desc);

		ID3D11ComputeShader*	m_shader;
		ShaderResourcesImpl		m_resources;
		desc::ComputePipeline	m_descriptor;
	};
}
