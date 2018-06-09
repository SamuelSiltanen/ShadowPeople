#pragma once

#include <d3d11.h>

#include "DX11Utils.hpp"

namespace graphics
{
	class ShaderImpl
	{
	public:
		ShaderImpl(const std::string& bindingName, desc::ShaderType type);
		~ShaderImpl();

		NO_COPY_CLASS(ShaderImpl);
	private:
		friend class ShaderManagerImpl;
		friend class CommandBufferImpl;

		std::string				m_bindingName;
		desc::ShaderType		m_type;
		ID3DBlob*				m_compiledSource;
		ID3D11DeviceChild*		m_shader;
	};
}
