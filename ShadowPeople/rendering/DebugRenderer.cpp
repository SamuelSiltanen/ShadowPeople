#include "DebugRenderer.hpp"
#include "Camera.hpp"

#include "../shaders/LineRenderer.if.h"

using namespace graphics;

namespace rendering
{
    DebugRenderer::DebugRenderer(Device& device)
    {
        m_lineRenderer = device.createGraphicsPipeline(desc::GraphicsPipeline()
            .binding<shaders::LineRenderer>()
            .setPrimitiveTopology(desc::PrimitiveTopology::LineList)
            .numRenderTargets(1)
            .rasterizerState(desc::RasterizerState().cullMode(desc::CullMode::None))
            .depthStencilState(desc::DepthStencilState()
                .depthTestingEnable(true)
                .depthWriteEnable(false)
                .depthFunc(desc::ComparisonMode::Less))
        );
    }

    void DebugRenderer::render(CommandBuffer& gfx, const Camera& camera,
                               const BufferView& vertexBuffer, int2 verticesToRender)
    {
        static const int VerticesPerGizmo = 6;

        auto binding = m_lineRenderer.bind<shaders::LineRenderer>(gfx);

        binding->constants.view = camera.viewMatrix();
        binding->constants.proj = camera.projectionMatrix();
        binding->vertexBuffer   = vertexBuffer;

        gfx.draw(*binding, verticesToRender[1] * VerticesPerGizmo, verticesToRender[0] * VerticesPerGizmo);
    }
}
