#pragma once

#include "../Graphics.hpp"

namespace rendering
{
	class ScreenBuffers
	{
	public:
		ScreenBuffers(graphics::Device& device, int2 screenSize);

		void clear(graphics::CommandBuffer& gfx);
		void setRenderTargets(graphics::CommandBuffer& gfx);

		const graphics::TextureView& gBuffer() { return m_gBufferSRV; }
		const graphics::TextureView& zBuffer() { return m_zBufferSRV; }
	private:
		graphics::Texture		m_gBuffer;
		graphics::TextureView	m_gBufferRTV;
		graphics::TextureView	m_gBufferSRV;
		graphics::Texture		m_zBuffer;
		graphics::TextureView	m_zBufferDSV;
		graphics::TextureView	m_zBufferSRV;
	};
}