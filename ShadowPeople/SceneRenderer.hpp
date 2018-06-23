#pragma once

#include "Graphics.hpp"
#include "ImGuiRenderer.hpp"

namespace rendering
{
	class Camera;

	class SceneRenderer
	{
	public:
		SceneRenderer(graphics::Device& device);

		void render(graphics::CommandBuffer& gfx, const Camera& camera);
	private:
		graphics::ComputePipeline	m_computePipeline;
		graphics::GraphicsPipeline	m_graphicsPipeline;

		graphics::Texture			m_clearTexture;
		graphics::TextureView		m_clearTextureUAV;

		graphics::Texture			m_renderTarget;
		graphics::TextureView		m_renderTargetRTV;

		graphics::Texture			m_depthBuffer;
		graphics::TextureView		m_depthBufferDSV;

		ImGuiRenderer				m_imGuiRenderer;
	};
}
