/*
    Copyright 2018 Samuel Siltanen
    SceneRenderer.hpp
*/

#pragma once

#include "../graphics/Graphics.hpp"
#include "ScreenBuffers.hpp"
#include "ImageBuffers.hpp"
#include "ImGuiRenderer.hpp"
#include "DebugRenderer.hpp"

namespace rendering
{
    class Camera;
    class Scene;
    class GeometryCache;
    class MaterialCache;
    class PatchCache;

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
        SceneRenderer(graphics::Device& device, GeometryCache& geometry, MaterialCache& materials, PatchCache& patches);

        void render(graphics::CommandBuffer& gfx, const Scene& scene);
    private:
        void culling(graphics::CommandBuffer& gfx, const Camera& camera);
        void geometryRendering(graphics::CommandBuffer& gfx, const Camera& camera, const Scene& scene);
        void lighting(graphics::CommandBuffer& gfx, const Camera& camera);
        void postprocess(graphics::CommandBuffer& gfx);
        void debugRendering(graphics::CommandBuffer& gfx, const Camera& camera, const Scene& scene);

        VisibleGeometry             m_visibleGeometry;
        ScreenBuffers               m_screenBuffers;
        ImageBuffers                m_imageBuffers;
        OutputImage                 m_outputImage;

        graphics::ComputePipeline   m_lightingPipeline;
        graphics::GraphicsPipeline  m_geometryRenderingPipeline;
        graphics::GraphicsPipeline  m_patchRenderingPipeline;

        graphics::Sampler           m_bilinearSampler;

        ImGuiRenderer               m_imGuiRenderer;
        DebugRenderer               m_debugRenderer;

        int2                        m_screenSize;

        GeometryCache&              m_geometry;
        MaterialCache&              m_materials;
        PatchCache&                 m_patches;
    };
}
