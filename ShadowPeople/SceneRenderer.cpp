#include "SceneRenderer.hpp"
#include "Camera.hpp"
#include "shaders/Test.if.h"
#include "shaders/Test2.if.h"
#include "imgui\imgui.h"

using namespace graphics;

namespace rendering
{
	SceneRenderer::SceneRenderer(Device device) :
		m_imGuiRenderer(device)
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

		m_renderTarget = device.createTexture(desc::Texture()
			.width(s[0])
			.height(s[1])
			.usage(desc::Usage::RenderTarget));
	
		m_renderTargetRTV = device.createTextureView(
			desc::TextureView(m_renderTarget.descriptor())
				.type(desc::ViewType::RTV), m_renderTarget);

		desc::Format depthFormat;
		depthFormat.channels	= desc::FormatChannels::Depth;
		depthFormat.bytes		= desc::FormatBytesPerChannel::B32;
		depthFormat.type		= desc::FormatType::Float;

		m_depthBuffer = device.createTexture(desc::Texture()
			.width(s[0])
			.height(s[1])
			.format(depthFormat)
			.usage(desc::Usage::DepthBuffer));

		m_depthBufferDSV = device.createTextureView(
			desc::TextureView(m_depthBuffer.descriptor())
				.type(desc::ViewType::DSV), m_depthBuffer);

		m_computePipeline = device.createComputePipeline(desc::ComputePipeline()
			.binding<shaders::TestCS>());

		m_graphicsPipeline = device.createGraphicsPipeline(desc::GraphicsPipeline()
			.binding<shaders::Test2GS>()
			.setPrimitiveTopology(desc::PrimitiveTopology::TriangleList)
			.numRenderTargets(1)
			.rasterizerState(desc::RasterizerState().cullMode(desc::CullMode::None))
			.depthStencilState(desc::DepthStencilState()
				.depthTestingEnable(true)
				.depthFunc(desc::ComparisonMode::Less)));
	}

	void SceneRenderer::render(CommandBuffer& gfx, const Camera& camera)
	{
		ImGui::NewFrame();

		gfx.clear(m_clearTextureUAV, 1.f, 0.5f, 0.f, 1.f);

		{
			auto binding = m_computePipeline.bind<shaders::TestCS>(gfx);

			binding->constants.size	= { 128, 256 };
			binding->backBuffer		= m_clearTextureUAV;
		
			gfx.dispatch(*binding, 256, 256, 1);
		}
		gfx.copyToBackBuffer(m_clearTexture);

		gfx.clear(m_renderTargetRTV, 0.f, 0.f, 0.f, 0.f);
		gfx.clear(m_depthBufferDSV, 1.f);

		gfx.setRenderTargets(m_depthBufferDSV, m_renderTargetRTV);
		{
			auto binding = m_graphicsPipeline.bind<shaders::Test2GS>(gfx);

			binding->constants.view = camera.viewMatrix();
			binding->constants.proj = camera.projectionMatrix();			

			gfx.draw(*binding, 12 * 3, 0);
		}

		ImGui::ShowDemoWindow();

		ImGui::EndFrame();
		ImGui::Render();

		m_imGuiRenderer.render(gfx, ImGui::GetDrawData());

		gfx.setRenderTargets();

		gfx.copyToBackBuffer(m_renderTarget);
	}
}
