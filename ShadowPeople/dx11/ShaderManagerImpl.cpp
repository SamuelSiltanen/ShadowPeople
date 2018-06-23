#include "ShaderManagerImpl.hpp"
#include "DeviceImpl.hpp"
#include "ShaderImpl.hpp"
#include "ShaderResourcesImpl.hpp"
#include "BufferImpl.hpp"
#include "../Errors.hpp"

#include <d3d11.h>
#include <d3dcompiler.h>

namespace graphics
{
	ShaderManagerImpl::ShaderManagerImpl(DeviceImpl& device) :
		m_device(device)
	{}

	std::wstring s2ws(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	bool ShaderManagerImpl::compile(ShaderImpl& shader)
	{
		std::string shaderName	= getShaderName(shader.m_bindingName, shader.m_type);
		std::string target		= getShaderTarget(shader.m_type);

		uint32_t flags1 = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
		uint32_t flags2 = 0;

		shader.m_compiledSource = nullptr;
		ID3DBlob* errorBlob		= nullptr;

		HRESULT hr = D3DCompileFromFile(s2ws(shaderName).c_str(), NULL,
										D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", target.c_str(),
										flags1, flags2, &shader.m_compiledSource, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugString((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}
			else
			{
				std::string errorMsg("Shader source not found: ");
				errorMsg.append(shaderName);
				errorMsg.append("\n");
				OutputDebugString(errorMsg.c_str());
			}

			SAFE_RELEASE(shader.m_compiledSource);
			shader.m_compiledSource = nullptr;

			return false;
		}

		return true;
	}

	void ShaderManagerImpl::createComputeShader(ShaderImpl& shader)
	{
		SP_ASSERT(shader.m_type == desc::ShaderType::Compute, "Shader type mismatch - expecting compute shader");
		auto shaderBlob = shader.m_compiledSource;
		SP_ASSERT(shaderBlob != nullptr, "Shader source not compiled - call compile() first");
		HRESULT hr = m_device.device()->CreateComputeShader(shaderBlob->GetBufferPointer(),
															shaderBlob->GetBufferSize(),
															NULL, (ID3D11ComputeShader**)&shader.m_shader);
		SP_ASSERT_HR(hr, ERROR_CODE_COMPUTE_SHADER_NOT_CREATED);
	}
		
	void ShaderManagerImpl::createVertexShader(ShaderImpl& shader)
	{
		SP_ASSERT(shader.m_type == desc::ShaderType::Vertex, "Shader type mismatch - expecting vertex shader");
		auto shaderBlob = shader.m_compiledSource;
		SP_ASSERT(shaderBlob != nullptr, "Shader source not compiled - call compile() first");
		HRESULT hr = m_device.device()->CreateVertexShader(shaderBlob->GetBufferPointer(),
														   shaderBlob->GetBufferSize(),
														   NULL, (ID3D11VertexShader**)&shader.m_shader);
		SP_ASSERT_HR(hr, ERROR_CODE_VERTEX_SHADER_NOT_CREATED);
	}
	
	void ShaderManagerImpl::createPixelShader(ShaderImpl& shader)
	{
		SP_ASSERT(shader.m_type == desc::ShaderType::Pixel, "Shader type mismatch - expecting pixel shader");
		auto shaderBlob = shader.m_compiledSource;
		SP_ASSERT(shaderBlob != nullptr, "Shader source not compiled - call compile() first");
		HRESULT hr = m_device.device()->CreatePixelShader(shaderBlob->GetBufferPointer(),
														  shaderBlob->GetBufferSize(),
														  NULL, (ID3D11PixelShader**)&shader.m_shader);
		SP_ASSERT_HR(hr, ERROR_CODE_PIXEL_SHADER_NOT_CREATED);
	}

	void ShaderManagerImpl::createConstantBuffers(ShaderResourcesImpl& resources, const desc::ShaderBinding& binding)
	{
		for (auto cb : binding.cbs())
		{
			uint32_t byteSize = static_cast<uint32_t>(cb.byteSize());
			SP_ASSERT(byteSize % 16 == 0, "Size of a constant buffer must be multipler of 16 bytes.");
			uint32_t numElems = byteSize / 16;
			auto desc = desc::Buffer().type(desc::BufferType::Constant)
				.elements(numElems).usage(desc::Usage::CpuToGpuFrequent);
			std::shared_ptr<BufferImpl> constantBuf = std::make_shared<BufferImpl>(m_device, desc);
			resources.cbs.emplace_back(constantBuf);
		}
	}

	std::string ShaderManagerImpl::getShaderName(const std::string& bindingName, desc::ShaderType type)
	{
		std::string ifName(bindingName);
		size_t pos = ifName.find_last_of(".if.h");

		SP_ASSERT(pos != std::wstring::npos, "Shader interface filename does not end with .if.h");

		std::string nameBody	= ifName.substr(0, pos - 4);

		switch (type)
		{
		case desc::ShaderType::Compute:
			nameBody.append(".cs");
			break;
		case desc::ShaderType::Vertex:
			nameBody.append(".vs");
			break;
		case desc::ShaderType::Pixel:
			nameBody.append(".ps");
			break;
		default:
			SP_ASSERT(false, "Shader type unknown - Cannot determine file ending");
			break;
		}

		std::string shaderName	= nameBody.append(".hlsl");

		return shaderName;
	}

	std::string ShaderManagerImpl::getShaderTarget(desc::ShaderType type)
	{
		std::string target = "";
		switch (type)
		{
		case desc::ShaderType::Compute:
			target = "cs_5_0";
			break;
		case desc::ShaderType::Vertex:
			target = "vs_5_0";
			break;
		case desc::ShaderType::Pixel:
			target = "ps_5_0";
			break;
		default:
			SP_ASSERT(false, "Shader type unknown - Cannot determine target");
			break;
		}

		return target;
	}
}
