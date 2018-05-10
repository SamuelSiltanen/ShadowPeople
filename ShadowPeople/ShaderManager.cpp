#include "ShaderManager.hpp"
#include "Errors.hpp"

#include <d3dcompiler.h>

namespace graphics
{
	ShaderManager::ShaderManager()
	{
	}

	bool ShaderManager::compileFile(const char* filename)
	{
		UINT flags = 0;
		UINT flagsFx = 0;
		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;
		HRESULT hr = D3DCompileFromFile(LPCWSTR(filename), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE,
										"main", "cs_5_0", flags, flagsFx, &shaderBlob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugString((char *)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}

			if (shaderBlob)
			{
				shaderBlob->Release();
			}

			return false;
		}

		// Store shaderBlob

		return true;
	}
}