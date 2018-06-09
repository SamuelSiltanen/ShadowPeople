#include "SceneRenderer.hpp"
#include "shaders/Test.if.h"
#include "shaders/Test2.if.h"

using namespace graphics;

namespace rendering
{
	SceneRenderer::SceneRenderer(Device device)
	{
		// Just testing - replace with real code later
		int2 s = device.swapChainSize();

		m_clearTexture = device.createTexture(desc::Texture()
			.width(s[0])
			.height(s[1])
			.usage(desc::Usage::GpuReadWrite));
	
		m_clearTextureUAV = device.createTextureView(
			desc::TextureView(m_clearTexture.descriptor())
				.type(desc::ViewType::UAV), m_clearTexture);

		m_computePipeline = device.createComputePipeline(desc::ComputePipeline()
			.binding<shaders::TestCS>());
		m_graphicsPipeline = device.createGraphicsPipeline(desc::GraphicsPipeline()
			.binding<shaders::Test2GS>());
	}

	void SceneRenderer::render(CommandBuffer& gfx)
	{		
		gfx.clear(m_clearTextureUAV, 1.f, 0.5f, 0.f, 1.f);

		auto binding = m_computePipeline.bind<shaders::TestCS>(gfx);

		binding->constants.size	= { 128, 256 };
		binding->backBuffer		= m_clearTextureUAV;
		
		gfx.dispatch(*binding, 256, 256, 1);

		gfx.copyToBackBuffer(m_clearTexture);	
	}
}
