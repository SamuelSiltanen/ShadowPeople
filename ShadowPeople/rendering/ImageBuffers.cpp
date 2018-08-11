#include "ImageBuffers.hpp"

using namespace graphics;

namespace rendering
{
	ImageBuffers::ImageBuffers(Device& device, int2 screenSize)
	{
		m_litBuffer = device.createTexture(desc::Texture()
			.width(screenSize[0])
			.height(screenSize[1])
			.usage(desc::Usage::GpuReadWrite)
            .name("Lit buffer"));
	
		m_litBufferUAV = device.createTextureView(m_litBuffer,
			desc::TextureView(m_litBuffer.descriptor())
				.type(desc::ViewType::UAV));
	}

	void ImageBuffers::copyTo(graphics::CommandBuffer& gfx, graphics::Texture& texture)
	{
		gfx.copy(texture, m_litBuffer);
	}
}
