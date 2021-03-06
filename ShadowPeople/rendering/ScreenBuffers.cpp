#include "SceneRenderer.hpp"

using namespace graphics;

namespace rendering
{
	ScreenBuffers::ScreenBuffers(Device& device, int2 screenSize)
	{
		desc::Format depthFormat(desc::FormatChannels::Depth,
                                 desc::FormatBytesPerChannel::B32,
                                 desc::FormatType::Float);

		m_zBuffer = device.createTexture(desc::Texture()
			.width(screenSize[0])
			.height(screenSize[1])
			.format(depthFormat)
			.usage(desc::Usage::DepthBuffer)
            .name("Z-buffer"));

		m_zBufferDSV = device.createTextureView(m_zBuffer,
			desc::TextureView(m_zBuffer.descriptor())
				.type(desc::ViewType::DSV));

		m_zBufferSRV = device.createTextureView(m_zBuffer,
			desc::TextureView(m_zBuffer.descriptor())
				.type(desc::ViewType::SRV));

		desc::Format gFormat(desc::FormatChannels::RGBA,
                             desc::FormatBytesPerChannel::B32,
                             desc::FormatType::UInt);

		m_gBuffer = device.createTexture(desc::Texture()
			.width(screenSize[0])
			.height(screenSize[1])
			.format(gFormat)
			.usage(desc::Usage::RenderTarget)
            .name("G-buffer"));
	
		m_gBufferRTV = device.createTextureView(m_gBuffer,
			desc::TextureView(m_gBuffer.descriptor())
				.type(desc::ViewType::RTV));

		m_gBufferSRV = device.createTextureView(m_gBuffer,
			desc::TextureView(m_gBuffer.descriptor())
				.type(desc::ViewType::SRV));
	}

	void ScreenBuffers::clear(CommandBuffer& gfx)
	{
		gfx.clear(m_gBufferRTV, 0.f, 0.f, 0.f, 0.f);
		gfx.clear(m_zBufferDSV, 1.f);
	}

	void ScreenBuffers::setRenderTargets(CommandBuffer& gfx)
	{
		gfx.setRenderTargets(m_zBufferDSV, m_gBufferRTV);
	}
}
