#include "ComputePipelineImpl.hpp"
#include "DeviceImpl.hpp"
#include "../Errors.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>	// TODO: Move the compilation into ShaderManager

namespace graphics
{
	std::wstring s2ws(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	ComputePipelineImpl::ComputePipelineImpl(DeviceImpl& device, const desc::ComputePipeline& desc) :
		m_descriptor(desc),
		m_shader(nullptr)
	{
		std::string ifName(desc.shaderInterface().filename());
		size_t pos = ifName.find_last_of(".if.h");

		SP_ASSERT(pos != std::wstring::npos, "Shader interface filename does not end with .if.h");

		std::string nameBody	= ifName.substr(0, pos - 4);
		std::string shaderName	= nameBody.append(".cs.hlsl");

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
		}
		else
		{
			SP_ASSERT(device.m_device, "Device used but not properly initialized");
			HRESULT hr = device.m_device->CreateComputeShader(
				shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(),
				NULL, &m_shader);
			SP_ASSERT_HR(hr, ERROR_CODE_COMPUTE_SHADER_NOT_CREATED);
		}
	}

	ComputePipelineImpl::~ComputePipelineImpl()
	{
		SAFE_RELEASE(m_shader);
	}
}
