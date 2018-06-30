#include "SceneRenderer.hpp"

using namespace graphics;

namespace rendering
{
	ScreenBuffers::ScreenBuffers(Device& device, int2 screenSize)
	{
		desc::Format depthFormat;
		depthFormat.channels	= desc::FormatChannels::Depth;
		depthFormat.bytes		= desc::FormatBytesPerChannel::B32;
		depthFormat.type		= desc::FormatType::Float;

		m_zBuffer = device.createTexture(desc::Texture()
			.width(screenSize[0])
			.height(screenSize[1])
			.format(depthFormat)
			.usage(desc::Usage::DepthBuffer));

		m_zBufferDSV = device.createTextureView(
			desc::TextureView(m_zBuffer.descriptor())
				.type(desc::ViewType::DSV), m_zBuffer);

		m_zBufferSRV = device.createTextureView(
			desc::TextureView(m_zBuffer.descriptor())
				.type(desc::ViewType::SRV), m_zBuffer);

		desc::Format gFormat;
		gFormat.channels	= desc::FormatChannels::RGBA;
		gFormat.bytes		= desc::FormatBytesPerChannel::B32;
		gFormat.type		= desc::FormatType::UInt;

		m_gBuffer = device.createTexture(desc::Texture()
			.width(screenSize[0])
			.height(screenSize[1])
			.format(gFormat)
			.usage(desc::Usage::RenderTarget));
	
		m_gBufferRTV = device.createTextureView(
			desc::TextureView(m_gBuffer.descriptor())
				.type(desc::ViewType::RTV), m_gBuffer);

		m_gBufferSRV = device.createTextureView(
			desc::TextureView(m_gBuffer.descriptor())
				.type(desc::ViewType::SRV), m_gBuffer);
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
