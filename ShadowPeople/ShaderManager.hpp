#pragma once

namespace graphics
{
	class ShaderManager
	{
	public:
		ShaderManager();

		//void		registerShader();
		//const void* compiledByteCode();

		bool compileFile(const char* filename);

	private:

	};
}
