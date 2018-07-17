#include "PatchGenerator.hpp"
#include "PatchCache.hpp"

namespace rendering
{
    PatchGenerator::PatchGenerator(PatchCache& patchCache) :
        m_patchCache(patchCache)
    {}

    void PatchGenerator::generatePatchData(PatchId id)
    {
        if (id.mip() == 0)
        {
            // Root has no parent - generate it from scratch
        }
        else
        {
            PatchId parent = id.parent();

        }
    }
}
