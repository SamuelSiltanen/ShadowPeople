/*
    Copyright 2018 Samuel Siltanen
    DebugRenderer.hpp
*/

#pragma once

#include "../graphics/Graphics.hpp"

namespace rendering
{
    class Camera;

    class DebugRenderer
    {
    public:
        DebugRenderer(graphics::Device& device);

        void render(graphics::CommandBuffer& gfx, const Camera& camera,
                    const graphics::BufferView& vertexBuffer, int2 verticesToRender);
    private:
        graphics::GraphicsPipeline m_lineRenderer;
    };
}
