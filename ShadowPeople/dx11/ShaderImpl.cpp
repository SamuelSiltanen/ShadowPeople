#include "ShaderImpl.hpp"

namespace graphics
{
	ShaderImpl::ShaderImpl(const std::string& bindingName, desc::ShaderType type) :
		m_bindingName(bindingName),
		m_type(type),
		m_compiledSource(nullptr),
		m_shader(nullptr)
	{}

	ShaderImpl::~ShaderImpl()
	{
		SAFE_RELEASE(m_compiledSource);
		SAFE_RELEASE(m_shader);
	}
}
