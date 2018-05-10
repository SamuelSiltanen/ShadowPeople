#pragma once

#include "Graphics.hpp"

namespace rendering
{
	class SceneRenderer
	{
	public:
		SceneRenderer(graphics::Device device);

		void render(graphics::CommandBuffer& gfx);
	private:
		graphics::ComputePipeline	m_computePipeline;

		graphics::Texture			m_clearTexture;
		graphics::TextureView		m_clearTextureUAV;
	};
}
