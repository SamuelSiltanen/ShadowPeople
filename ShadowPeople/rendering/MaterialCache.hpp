/*
    Copyright 2018 Samuel Siltanen
    MaterialCache.hpp
*/

#pragma once

#include "../graphics/Graphics.hpp"

namespace rendering
{
    enum class MaterialChannel
    {
        Albedo,
        Roughness,
        Normal
    };

    class MaterialCache
    {
    public:
        MaterialCache(graphics::Device& device);

        Rect<int, 2> allocate(int2 size);
        void preloadMaterial(graphics::Image& image, Rect<int, 2> dstRect, MaterialChannel channel);

        void updateGPUTextures(graphics::CommandBuffer& gfx);

        const graphics::TextureView albedoRougness() const { return m_albedoRoughnessSRV; }
        const graphics::TextureView normal() const { return m_normalSRV; }
    private:
        graphics::Texture       m_albedoRoughness;
        graphics::TextureView   m_albedoRoughnessSRV;
        graphics::Texture       m_normal;
        graphics::TextureView   m_normalSRV;

        graphics::Image         m_albedoRoughnessCache;
        graphics::Image         m_normalCache;
    };
}
