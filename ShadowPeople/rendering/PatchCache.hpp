#pragma once

#include "../graphics/Graphics.hpp"
#include "../FreeList.hpp"
#include "Patch.hpp"

#include <unordered_map>
#include <queue>

namespace rendering
{
    // There are 21 levels of mipmaps
    constexpr uint32_t PatchMipLevels = 21;
    // Given patch storage requirement, 256 kB per patch,
    // and assuming we have 1 Gb to spare, we've got 4 k elements
    constexpr uint32_t PatchCacheMaxElements = (1 << 12);

    class PatchCache
    {
    public:
        PatchCache(graphics::Device& device);

        // Returns cache offset of the requested page or one of its parent in the hierarchy
        uint32_t request(PatchId id);

        // Evict a patch that is no longer needed
        void evict(PatchId id);

        

        PatchId nextGenerationRequest();
    private:
        graphics::Buffer        m_patches;
        graphics::BufferView    m_patchesSRV;

        std::vector<Patch>      m_patchMetadata;

        FreeList                m_patchAllocator;

        std::unordered_map<PatchId, uint32_t>   m_idToOffset;
        std::queue<PatchId>     m_generationRequests;
    };
}
