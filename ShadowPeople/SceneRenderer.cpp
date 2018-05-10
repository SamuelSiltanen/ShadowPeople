#include "SceneRenderer.hpp"
#include "shaders\Test.if.h"

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

		m_computePipeline = device.createComputePipeline(desc::ComputePipeline(shaders::iTestCS));
	}

	void SceneRenderer::render(CommandBuffer& gfx)
	{		
		gfx.clear(m_clearTextureUAV, 1.f, 0.5f, 0.f, 1.f);

		auto binding = m_computePipeline.bind<shaders::TestCS>(gfx);
		binding.constants.size	= { 256, 256 };
		binding.backBuffer		= m_clearTextureUAV;

		uint32_t numCBs = static_cast<uint32_t>(binding.cbs().size());
		uint32_t numSRVs = static_cast<uint32_t>(binding.srvs().size());
		uint32_t numUAVs = static_cast<uint32_t>(binding.uavs().size());
		uint32_t numSamplers = static_cast<uint32_t>(binding.samplers().size());
		printf("Bound %d CBs\n", numCBs);
		printf("Bound %d SRVs\n", numSRVs);
		printf("Bound %d UAVs\n", numUAVs);
		printf("Bound %d samplers\n", numSamplers);

		gfx.dispatch(binding, 256, 256, 1);

		gfx.copyToBackBuffer(m_clearTexture);	
	}
}
