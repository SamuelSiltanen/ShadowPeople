/*
    Copyright 2018 Samuel Siltanen
    MaterialCache.cpp
*/

#include "MaterialCache.hpp"
#include "../Math.hpp"

using namespace graphics;

namespace rendering
{
    constexpr uint32_t MaterialCacheTextureSize = 2048;//8192;

    MaterialCache::MaterialCache(Device& device) :
        m_albedoRoughnessCache(32, MaterialCacheTextureSize, MaterialCacheTextureSize),
        m_normalCache(32, MaterialCacheTextureSize, MaterialCacheTextureSize)
    {
        // 8-bit RGBA - albedo + roughness
        m_albedoRoughness = device.createTexture(desc::Texture()
            .width(MaterialCacheTextureSize)
            .height(MaterialCacheTextureSize)
            .format(desc::Format())
            .usage(desc::Usage::GpuReadWrite));
        m_albedoRoughnessSRV = device.createTextureView(m_albedoRoughness,
            desc::TextureView(m_albedoRoughness.descriptor()).type(desc::ViewType::SRV));

        // 16-bit RG - octahedral packed normal
        m_normal = device.createTexture(desc::Texture()
            .width(MaterialCacheTextureSize)
            .height(MaterialCacheTextureSize)
            .format(desc::Format()
                .channels(desc::FormatChannels::RG)
                .bytes(desc::FormatBytesPerChannel::B16)
                .type(desc::FormatType::UNorm))
            .usage(desc::Usage::GpuReadWrite));
        m_normalSRV = device.createTextureView(m_normal,
            desc::TextureView(m_normal.descriptor()).type(desc::ViewType::SRV));
    }

    Rect<int, 2> MaterialCache::allocate(int2 size)
    {
        // TODO: A proper allocator
        Rect<int, 2> rect = Rect<int, 2>({ 0, 0 }, size);
        return rect;
    }

    void MaterialCache::preloadMaterial(Image& image, Rect<int, 2> dstRect, MaterialChannel channel)
    {
        struct RGBA
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };

        struct BGR
        {
            uint8_t b;
            uint8_t g;
            uint8_t r;
        };

        struct RG16
        {
            uint16_t r;
            uint16_t g;
        };

        auto RGB2RGBX = [](void *dst, const void *src)
        {
            RGBA* dstTyped = reinterpret_cast<RGBA*>(dst);
            const BGR* srcTyped = reinterpret_cast<const BGR*>(src);
            dstTyped->r = srcTyped->r;
            dstTyped->g = srcTyped->g;
            dstTyped->b = srcTyped->b;
        };

        auto RGB2XXXR = [](void *dst, const void *src)
        {
            RGBA* dstTyped = reinterpret_cast<RGBA*>(dst);
            const BGR* srcTyped = reinterpret_cast<const BGR*>(src);
            dstTyped->a = srcTyped->r;
        };

        auto RGB82RG16 = [](void *dst, const void *src)
        {
            RG16* dstTyped = reinterpret_cast<RG16*>(dst);
            const BGR* srcTyped = reinterpret_cast<const BGR*>(src);
            float3 norm{ (static_cast<float>(srcTyped->r) - 128.f) / 128.f,
                         (static_cast<float>(srcTyped->g) - 128.f) / 128.f,
                         (static_cast<float>(srcTyped->b) - 128.f) / 128.f };
            float2 octaNorm = math::encodeOctahedral(normalize(norm));
            dstTyped->r = static_cast<uint16_t>(octaNorm[0] * 65535.f + 0.5f);
            dstTyped->g = static_cast<uint16_t>(octaNorm[1] * 65535.f + 0.5f);
        };

        switch(channel)
        {
        case MaterialChannel::Albedo:
            m_albedoRoughnessCache.copy(image, dstRect, RGB2RGBX);
            break;
        case MaterialChannel::Roughness:
            m_albedoRoughnessCache.copy(image, dstRect, RGB2XXXR);
            break;
        case MaterialChannel::Normal:
            m_normalCache.copy(image, dstRect, RGB82RG16);
            break;
        default:
            break;
        }
    }

    void MaterialCache::updateGPUTextures(graphics::CommandBuffer& gfx)
    {
        // TODO: Better logic here
        gfx.update(m_albedoRoughness, Range<const uint8_t>(m_albedoRoughnessCache.data(), 
                                                           m_albedoRoughnessCache.dataSize()));
        gfx.update(m_normal, Range<const uint8_t>(m_normalCache.data(), 
                                                  m_normalCache.dataSize()));
    }
}
