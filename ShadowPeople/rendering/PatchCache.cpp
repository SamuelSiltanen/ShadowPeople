#include "PatchCache.hpp"
#include "../Errors.hpp"

using namespace graphics;

namespace rendering
{
    PatchCache::PatchCache(Device& device) :
        m_patchAllocator(PatchCacheMaxElements)
    {
        m_patchData = device.createTexture(desc::Texture()
            .format(desc::Format(desc::FormatChannels::R, desc::FormatBytesPerChannel::B16, desc::FormatType::UInt))
            .width(PatchCacheSize)
            .height(PatchCacheSize)
            .arraySize(PatchMipLevels)
            .usage(desc::Usage::GpuReadWrite)
            .name("Patch data")
        );
        m_patchDataSRV = device.createTextureView(m_patchData, 
                            desc::TextureView(m_patchData.descriptor()).type(desc::ViewType::SRV));

        for (uint32_t i = 0; i < PatchMipLevels; i++)
        {
            m_patchDataCPU[i].setDimensions(16, PatchCacheSize, PatchCacheSize);
        }

        m_patchMetadata = device.createBuffer(desc::Buffer()
            .format<Patch>()
            .elements(PatchCacheMaxElements)
            .structured(true)
            .usage(desc::Usage::GpuReadWrite)
            .name("Patch metadata"));
        m_patchMetadataSRV = device.createBufferView(m_patchMetadata,
                                desc::BufferView(m_patchMetadata.descriptor()).type(desc::ViewType::SRV));

        m_patchMetadataCPU.resize(PatchCacheMaxElements);

        addPermanentlyResidentPatches();
    }

    uint32_t PatchCache::request(PatchId id)
    {
        // If the item exists, return it
        auto it = m_idToOffset.find(id);
        bool patchMetadataExists = (it != m_idToOffset.end());
        if (patchMetadataExists)
        {
            if (m_patchMetadataCPU[it->second].dataReady) return it->second;
        }
        
        // Else, request generation/load, and return the nearest parent.
        std::vector<PatchId> missingPatches;

        // Request missing patch generation only, if has not been requested before,
        // i.e. it has not metedatas
        if (!patchMetadataExists)
        {
            missingPatches.emplace_back(id);
        }

        // The loop should terminate, because there is always a root node.
        PatchId parent = id.parent();
        do
        {
            it = m_idToOffset.find(parent);
            patchMetadataExists = (it != m_idToOffset.end());

            if (patchMetadataExists)
            {
                if (m_patchMetadataCPU[it->second].dataReady) break;
            }
            else
            {
                missingPatches.emplace_back(parent);
            }

            parent = parent.parent();
        } while(true);

        // Push the missing patches from highest parent downwards, so that
        // parents always exist during the generation step
        for (int i = missingPatches.size(); i >= 0; i--)
        {
            addPatch(missingPatches[i]);
            m_generationRequests.push(missingPatches[i]);
        }

        return it->second;
    }

    void PatchCache::evict(PatchId id)
    {
        auto it = m_idToOffset.find(id);
        SP_ASSERT(it != m_idToOffset.end(), "Trying to evict a patch that was not in the cache");

        m_patchAllocator.release(it->second);
        m_idToOffset.erase(it);
    }

    void PatchCache::dataReady(PatchId id)
    {
        m_dirtyPatches.emplace_back(id);
    }

    const Patch& PatchCache::patchMetadata(PatchId id) const
    {
        auto it = m_idToOffset.find(id);
        SP_ASSERT(it != m_idToOffset.end(), "Trying to read a patch that was not in the cache");

        return m_patchMetadataCPU[it->second];
    }

    Patch& PatchCache::patchMetadata(PatchId id)
    {
        auto it = m_idToOffset.find(id);
        SP_ASSERT(it != m_idToOffset.end(), "Trying to read a patch that was not in the cache");

        return m_patchMetadataCPU[it->second];
    }

    PatchId PatchCache::nextGenerationRequest()
    {
        if (m_generationRequests.empty()) return PatchId::InvalidId;

        PatchId id = m_generationRequests.front();
        m_generationRequests.pop();
        return id;
    }

    void PatchCache::updateGPUBuffersAndTextures(graphics::CommandBuffer& gfx)
    {
        for (auto id : m_dirtyPatches)
        {
            auto it = m_idToOffset.find(id);
            if (it == m_idToOffset.end()) continue; // Note: Patch was evicted while the data was being generated
            m_patchMetadataCPU[it->second].dataReady = true;
            
            int x = static_cast<int>(id.x()) * PatchResolution;
            int y = static_cast<int>(id.y()) * PatchResolution;
            int s = static_cast<int>(PatchResolution);
            int2 dstPos{ x, y };
            Rect<int, 2> srcRect({ x, y }, { s, s });
            Subresource dstSubresource{ 0, static_cast<int>(id.mip()) };
            gfx.update(m_patchData, m_patchDataCPU[id.mip()], dstPos, srcRect, dstSubresource);
        }

        // TODO: Measure if it is faster to update the whole buffer once than separate patches here and there.
        // Currently, the buffer takes 189 kB
        if (!m_dirtyPatches.empty())
        {

            gfx.update(m_patchMetadata, vectorAsByteRange(m_patchMetadataCPU));
        }

        m_dirtyPatches.clear();
    }

    void PatchCache::addPatch(PatchId id)
    {
        Patch patch;
        patch.id            = id;
        patch.cacheOffset   = m_patchAllocator.allocate();
        patch.seed          = (id.mip() << 8) | (id.y() << 4) | id.x(); // TODO: Replace later with scene dependent seeds
        patch.steepness     = powf(0.5f, static_cast<float>(id.mip())); // TODO: Replace later with fancier steepness profiles

        m_patchMetadataCPU[patch.cacheOffset]   = patch;
        m_idToOffset[id]                        = patch.cacheOffset;
    }

    void PatchCache::addPermanentlyResidentPatches()
    {
        for (uint32_t mip = 0; mip < PatchMipsAlwaysResident; mip++)
        {
            for (uint32_t y = 0; y < (1U << mip); y++)
            {
                for (uint32_t x = 0; x < (1U << mip); x++)
                {
                    PatchId id(x, y, mip);
                    addPatch(id);
                }
            }            
        }
    }
}
