#include "ComputePipelineImpl.hpp"
#include "DeviceImpl.hpp"
#include "BufferImpl.hpp"
#include "../Errors.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>	// TODO: Move the compilation into ShaderManager

namespace graphics
{
	ComputePipelineImpl::ComputePipelineImpl(DeviceImpl& device, const desc::ComputePipeline& desc) :
		m_descriptor(desc),
		m_shader(nullptr)
	{
		std::string shaderName	= getComputeShaderName(desc);

		if (auto blob = compileShader(shaderName))
		{
			createComputeShader(device, blob);
		}

		createConstantBuffers(device, desc);
	}

	ComputePipelineImpl::~ComputePipelineImpl()
	{
		SAFE_RELEASE(m_shader);
	}

	std::wstring s2ws(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	std::string ComputePipelineImpl::getComputeShaderName(const desc::ComputePipeline& desc)
	{
		std::string ifName(desc.descriptor().binding->filename());
		size_t pos = ifName.find_last_of(".if.h");

		SP_ASSERT(pos != std::wstring::npos, "Shader interface filename does not end with .if.h");

		std::string nameBody	= ifName.substr(0, pos - 4);
		std::string shaderName	= nameBody.append(".cs.hlsl");

		return shaderName;
	}

	void ComputePipelineImpl::createComputeShader(DeviceImpl& device, ID3DBlob* shaderBlob)
	{
		SP_ASSERT(device.m_device, "Device used but not properly initialized");
		HRESULT hr = device.m_device->CreateComputeShader(
				shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(),
				NULL, &m_shader);
		SP_ASSERT_HR(hr, ERROR_CODE_COMPUTE_SHADER_NOT_CREATED);
	}

	ID3DBlob* ComputePipelineImpl::compileShader(const std::string& shaderName)
	{
		ID3DBlob* shaderBlob	= nullptr;
		ID3DBlob* errorBlob		= nullptr;

		HRESULT hr = D3DCompileFromFile(s2ws(shaderName).c_str(), NULL,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main", "cs_5_0", 0, 0, &shaderBlob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugString((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			SAFE_RELEASE(shaderBlob);

			return nullptr;
		}

		return shaderBlob;
	}

	void ComputePipelineImpl::createConstantBuffers(DeviceImpl& device, const desc::ComputePipeline& desc)
	{
		for (auto cb : desc.descriptor().binding->cbs())
		{
			uint32_t byteSize = static_cast<uint32_t>(cb.byteSize());
			SP_ASSERT(byteSize % 16 == 0, "Size of a constant buffer must be multipler of 16 bytes.");
			uint32_t numElems = byteSize / 16;
			auto desc = desc::Buffer().type(desc::BufferType::Constant)
				.elements(numElems).usage(desc::Usage::CpuToGpuFrequent);
			std::shared_ptr<BufferImpl> constantBuf = std::make_shared<BufferImpl>(device, desc);
			m_resources.cbs.emplace_back(constantBuf);
		}
	}
}
