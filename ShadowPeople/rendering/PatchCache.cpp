#include "PatchCache.hpp"
#include "../Errors.hpp"

using namespace graphics;

namespace rendering
{
    PatchCache::PatchCache(Device& device) :
        m_patchAllocator(PatchCacheMaxElements)
    {

    }

    uint32_t PatchCache::request(PatchId id)
    {
        // If the item exists, return it
        auto it = m_idToOffset.find(id);
        if (it != m_idToOffset.end()) return it->second;
        
        // Else, request generation/load, and return the nearest parent.
        std::vector<PatchId> missingPatches;
        missingPatches.emplace_back(id);

        // The loop should terminate, because there is always a root node.
        PatchId parent = id.parent();
        while ((it = m_idToOffset.find(parent)) == m_idToOffset.end())
        {
            missingPatches.emplace_back(parent);
            parent = parent.parent();
        }

        // Push the missing patches from highest parent downwards, so that
        // parents always exist during the generation step
        for (int i = missingPatches.size(); i >= 0; i--)
        {
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

    PatchId PatchCache::nextGenerationRequest()
    {
        if (m_generationRequests.empty()) return PatchId::InvalidId;

        PatchId id = m_generationRequests.front();
        m_generationRequests.pop();
        return id;
    }
}
