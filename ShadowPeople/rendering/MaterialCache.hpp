/*
    Copyright 2018 Samuel Siltanen
    MaterialCache.hpp
*/

#pragma once

#include "../graphics/Graphics.hpp"

#include "Image.hpp"

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
        void preloadMaterial(Image& image, Rect<int, 2> dstRect, MaterialChannel channel);

        void updateGPUTextures(graphics::CommandBuffer& gfx);

        const graphics::TextureView albedoRougness() const { return m_albedoRoughnessSRV; }
        const graphics::TextureView normal() const { return m_normalSRV; }
    private:
        graphics::Texture       m_albedoRoughness;
        graphics::TextureView   m_albedoRoughnessSRV;
        graphics::Texture       m_normal;
        graphics::TextureView   m_normalSRV;

        Image                   m_albedoRoughnessCache;
        Image                   m_normalCache;
    };
}
