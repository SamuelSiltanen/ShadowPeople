#include "PatchGenerator.hpp"
#include "PatchCache.hpp"
#include "../Math.hpp"
#include "../graphics/Image.hpp"

using namespace graphics;

namespace rendering
{
    PatchGenerator::PatchGenerator(PatchCache& patchCache) :
        m_patchCache(patchCache)
    {
        generatePermanentlyResidentPatches();
    }

    // Generate layers that are always assumed to be resident in cache
    void PatchGenerator::generatePermanentlyResidentPatches()
    {
        for (uint32_t mip = 0; mip < PatchMipsAlwaysResident; mip++)
        {
            for (uint32_t y = 0; y < (1 << mip); y++)
            {
                for (uint32_t x = 0; x < (1 << mip); x++)
                {
                    generatePatchData(PatchId(x, y, mip));
                }
            }            
        }
    }

    void PatchGenerator::generatePatchData(PatchId id)
    {
        Patch& patch        = m_patchCache.patchMetadata(id);
        Image& targetLayer  = m_patchCache.patchData(patch.id.mip());

        m_random.seed(patch.seed);

        if (id.mip() == 0)
        {
            generateRootPatch(patch, targetLayer);
        }
        else
        {
            generateChildPatch(patch, targetLayer);
        }
    }

    // Root has no parent - generate it from scratch
    void PatchGenerator::generateRootPatch(Patch& patch, Image& targetLayer)
    {
        std::uniform_real_distribution<float> dist(-patch.steepness, patch.steepness);

        const uint16_t HalfOffset = (1 << 15);

        auto data   = targetLayer.asRange<uint16_t>();

        data[0]     = static_cast<uint16_t>(HalfOffset + HalfOffset * dist(m_random));

        uint16_t minH = data[0];
        uint16_t maxH = data[0];

        auto wrappingIndex = [&](int x, int y) -> uint32_t
        {
            x = (x + PatchResolution) & (PatchResolution - 1);
            y = (y + PatchResolution) & (PatchResolution - 1);
            return static_cast<uint32_t>(y * targetLayer.width() + x);
        };

        uint32_t scale  = HalfOffset / 2;
        uint32_t parts  = 1;
        int step        = PatchResolution;
        uint32_t loops  = math::log2(PatchResolution);
        for (uint32_t i = 0; i < loops; i++)
        {
            for (int y = 0; y < parts; y++)
            {
                for (int x = 0; x < parts; x++)
                {
                    // Corner indices
                    uint32_t c0Index = wrappingIndex(x * step, y * step);
                    uint32_t c1Index = wrappingIndex((x + 1) * step, y * step);
                    uint32_t c2Index = wrappingIndex(x * step, (y + 1) * step);
                    uint32_t c3Index = wrappingIndex((x + 1) * step, (y + 1) * step);
                 
                    // Mid-point value
                    uint32_t avg = static_cast<uint32_t>(data[c0Index]) +
                        static_cast<uint32_t>(data[c1Index]) +
                        static_cast<uint32_t>(data[c2Index]) +
                        static_cast<uint32_t>(data[c3Index]);
                    uint16_t midValue = static_cast<uint16_t>(avg + scale * dist(m_random));

                    // Store mid-point value
                    uint32_t midIndex = wrappingIndex(x * step + step / 2, y * step + step / 2);
                    data[midIndex] = midValue;

                    // Record min and max
                    minH = std::min<uint16_t>(minH, midValue);
                    maxH = std::max<uint16_t>(maxH, midValue);

                    // Wrapping left and top indices
                    uint32_t c4Index = wrappingIndex(x * step - step / 2, y * step);
                    uint32_t c5Index = wrappingIndex(x * step, y * step - step / 2);

                    // Left value
                    avg = static_cast<uint32_t>(data[c0Index]) +
                        static_cast<uint32_t>(data[c4Index]) +
                        static_cast<uint32_t>(data[c2Index]) +
                        static_cast<uint32_t>(data[midIndex]);
                    uint16_t leftValue = static_cast<uint16_t>(avg + scale * dist(m_random));

                    // Store left value
                    uint32_t leftIndex = wrappingIndex(x * step, y * step + step / 2);
                    data[leftIndex] = leftValue;

                    // Top value
                    avg = static_cast<uint32_t>(data[c0Index]) +
                        static_cast<uint32_t>(data[c1Index]) +
                        static_cast<uint32_t>(data[c5Index]) +
                        static_cast<uint32_t>(data[midIndex]);
                    uint16_t topValue = static_cast<uint16_t>(avg + scale * dist(m_random));

                    // Store top value
                    uint32_t topIndex = wrappingIndex(x * step + step / 2, y);
                    data[topIndex] = topValue;
                }
            }

            step >>= 1;
            parts <<= 1;
            scale >>= 1;
        }

        for (int y = 0; y < PatchResolution; y++)
        {
            for (int x = 0; x < PatchResolution; x++)
            {
                uint32_t index = wrappingIndex(x, y);
                data[index] -= minH;
            }
        }

        patch.minHeight = ZeroLevelHeight + minH;
        patch.maxHeight = ZeroLevelHeight + maxH;
        
        m_patchCache.dataReady(patch.id);
    }

    // Use the parent layer as a basis and generate data in between its samples
    void PatchGenerator::generateChildPatch(Patch& patch, Image& targetLayer)
    {
        // TODO

        PatchId parent          = patch.id.parent();
        const Image& parentData = m_patchCache.patchData(parent.mip());



    }
}
