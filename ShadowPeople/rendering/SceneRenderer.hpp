#pragma once

#include "../graphics/Graphics.hpp"
#include "ScreenBuffers.hpp"
#include "ImageBuffers.hpp"
#include "ImGuiRenderer.hpp"

namespace rendering
{
    class Camera;
    class Scene;

    struct VisibleGeometry
    {
        graphics::Buffer    objects;
    };

    struct OutputImage
    {
        graphics::Texture       output;
        graphics::TextureView   outputRTV;
    };

    class SceneRenderer
    {
    public:
        SceneRenderer(graphics::Device& device);

        void render(graphics::CommandBuffer& gfx, const Scene& scene);
    private:
        void culling(graphics::CommandBuffer& gfx, const Camera& camera);
        void geometryRendering(graphics::CommandBuffer& gfx, const Camera& camera);
        void lighting(graphics::CommandBuffer& gfx, const Camera& camera);
        void postprocess(graphics::CommandBuffer& gfx);

        VisibleGeometry             m_visibleGeometry;
        ScreenBuffers               m_screenBuffers;
        ImageBuffers                m_imageBuffers;
        OutputImage                 m_outputImage;

        graphics::ComputePipeline   m_computePipeline;
        graphics::GraphicsPipeline  m_graphicsPipeline;

        ImGuiRenderer               m_imGuiRenderer;

        int2                        m_screenSize;
    };
}
