/*
    Copyright 2018 Samuel Siltanen
    SceneRenderer.cpp
*/

#include "SceneRenderer.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "GeometryCache.hpp"
#include "../shaders/Test.if.h"
#include "../shaders/Test2.if.h"
#include "../shaders/GeometryRenderer.if.h"
#include "../imgui/imgui.h"

using namespace graphics;

namespace rendering
{
	SceneRenderer::SceneRenderer(Device& device, GeometryCache& geometry) :
		m_screenBuffers(device, device.swapChainSize()),
		m_imageBuffers(device, device.swapChainSize()),
		m_imGuiRenderer(device),
		m_screenSize(device.swapChainSize()),
        m_geometry(geometry)
	{
		m_outputImage.output = device.createTexture(desc::Texture()
			.width(m_screenSize[0])
			.height(m_screenSize[1])
			.usage(desc::Usage::RenderTarget));
	
		m_outputImage.outputRTV = device.createTextureView(m_outputImage.output,
			desc::TextureView(m_outputImage.output.descriptor())
				.type(desc::ViewType::RTV));

		// Just testing - replace with real code later
		m_computePipeline = device.createComputePipeline(desc::ComputePipeline()
			.binding<shaders::TestCS>());

		m_graphicsPipeline = device.createGraphicsPipeline(desc::GraphicsPipeline()
			.binding<shaders::GeometryRenderer>()
			.setPrimitiveTopology(desc::PrimitiveTopology::TriangleList)
			.numRenderTargets(1)
			.rasterizerState(desc::RasterizerState().cullMode(desc::CullMode::None))
			.depthStencilState(desc::DepthStencilState()
				.depthTestingEnable(true)
				.depthFunc(desc::ComparisonMode::Less)));
	}

	void SceneRenderer::render(CommandBuffer& gfx, const Scene& scene)
	{
		culling(gfx, scene.camera());
		geometryRendering(gfx, scene.camera(), scene);
		lighting(gfx, scene.camera());
		postprocess(gfx);
		gfx.copyToBackBuffer(m_outputImage.output);
	}

	void SceneRenderer::culling(CommandBuffer& gfx, const Camera& camera)
	{

	}
	
	void SceneRenderer::geometryRendering(CommandBuffer& gfx, const Camera& camera, const Scene& scene)
	{
        m_geometry.updateGPUBuffers(gfx);

		m_screenBuffers.clear(gfx);
		m_screenBuffers.setRenderTargets(gfx);

        gfx.setIndexBuffer(m_geometry.indexBuffer());

        for (const auto& obj : scene.objects())
		{
			auto binding = m_graphicsPipeline.bind<shaders::GeometryRenderer>(gfx);

			binding->constants.view = camera.viewMatrix();
			binding->constants.proj = camera.projectionMatrix();
            binding->vertexBuffer   = m_geometry.vertexBuffer();

			gfx.drawIndexed(*binding, obj.meshStartSize[1], obj.meshStartSize[0], 0);
		}

        gfx.setIndexBuffer();

		gfx.setRenderTargets();
	}

	void SceneRenderer::lighting(CommandBuffer& gfx, const Camera& camera)
	{
		{
			auto binding = m_computePipeline.bind<shaders::TestCS>(gfx);

			binding->constants.size	= m_screenSize;
			binding->gBuffer		= m_screenBuffers.gBuffer();
			binding->litBuffer		= m_imageBuffers.litBuffer();
		
			gfx.dispatch(*binding, m_screenSize[0], m_screenSize[1], 1);
		}

		m_imageBuffers.copyTo(gfx, m_outputImage.output);
	}

	void SceneRenderer::postprocess(CommandBuffer& gfx)
	{
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		ImGui::EndFrame();
		ImGui::Render();

		gfx.setRenderTargets(m_outputImage.outputRTV);
		m_imGuiRenderer.render(gfx, ImGui::GetDrawData());
		gfx.setRenderTargets();
	}
}
