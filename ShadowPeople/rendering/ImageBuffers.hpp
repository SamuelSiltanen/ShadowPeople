/*
    Copyright 2018 Samuel Siltanen
    ImageBuffers.hpp
*/

#pragma once

#include "../graphics/Graphics.hpp"

namespace rendering
{
	class ImageBuffers
	{
	public:
		ImageBuffers(graphics::Device& device, int2 screenSize);

		void copyTo(graphics::CommandBuffer& gfx, graphics::Texture& texture);

		graphics::TextureView litBuffer() { return m_litBufferUAV; }
	private:
		graphics::Texture		m_litBuffer;
		graphics::TextureView	m_litBufferUAV;
	};
}
