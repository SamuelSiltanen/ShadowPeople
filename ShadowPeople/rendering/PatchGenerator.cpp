#include "PatchGenerator.hpp"
#include "PatchCache.hpp"
#include "../Math.hpp"
#include "../graphics/Image.hpp"

#include "../Errors.hpp"
#include "../Timer.hpp"

#include <random>

using namespace graphics;

namespace rendering
{
    constexpr int PatchBorder          = 2;
    constexpr int PatchSizeWithBorders = PatchResolution + 2 * PatchBorder;

    PatchGenerator::PatchGenerator(PatchCache& patchCache) :
        m_patchCache(patchCache),
        m_processingPatch(32, PatchSizeWithBorders, PatchSizeWithBorders)
    {
        generatePermanentlyResidentPatches();
    }

    // Generate layers that are always assumed to be resident in cache
    void PatchGenerator::generatePermanentlyResidentPatches()
    {
        Timer timer;
        for (uint32_t mip = 0; mip < PatchMipsAlwaysResident; mip++)
        {
            for (uint32_t y = 0; y < (1U << mip); y++)
            {
                for (uint32_t x = 0; x < (1U << mip); x++)
                {
                    timer.start();
                    generatePatchData(PatchId(x, y, mip));
                    float t = timer.stop();
                    std::string msg("Generation time: ");
                    msg.append(std::to_string(t)).append(" s\n");
                    OutputDebugString(msg.c_str());
                }
            }            
        }
    }

    void PatchGenerator::generatePatchData(PatchId id)
    {
        std::string msg("Generating patch x = ");
        msg.append(std::to_string(id.x())).append(" y = ");
        msg.append(std::to_string(id.y())).append(" mip = ");
        msg.append(std::to_string(id.mip())).append("\n");
        OutputDebugString(msg.c_str());

        Patch& patch        = m_patchCache.patchMetadata(id);
        Image& targetLayer  = m_patchCache.patchData(patch.id.mip());

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

        float amplitude = MaxAmplitude;

        auto data       = m_processingPatch.asRange<float>();
        uint32_t io     = PatchBorder * PatchSizeWithBorders + PatchBorder;
        data[io]        = amplitude + amplitude * dist(m_random.consume(patch.id.seed(0, 0)));

        float minH      = data[io];
        float maxH      = data[io];

        auto wrappingIndex = [&](int x, int y) -> uint32_t
        {
            x = ((x + PatchResolution) & (PatchResolution - 1)) + PatchBorder;
            y = ((y + PatchResolution) & (PatchResolution - 1)) + PatchBorder;
            return static_cast<uint32_t>(y * PatchSizeWithBorders + x);
        };        

        int parts       = 1;
        int step        = PatchResolution;
        uint32_t loops  = math::log2(PatchResolution);
        
        for (uint32_t i = 0; i < loops; i++)
        {
            amplitude *= 0.5f;

            // Square step
            for (int y = 0; y < parts; y++)
            {
                for (int x = 0; x < parts; x++)
                {
                    // Corner indices
                    uint32_t i0     = wrappingIndex(      x * step,       y * step);
                    uint32_t i1     = wrappingIndex((x + 1) * step,       y * step);
                    uint32_t i2     = wrappingIndex(      x * step, (y + 1) * step);
                    uint32_t i3     = wrappingIndex((x + 1) * step, (y + 1) * step);
                 
                    // Mid-point value
                    int mx          = x * step + step / 2;
                    int my          = y * step + step / 2;
                    float avg       = 0.25f * (data[i0] + data[i1] + data[i2] + data[i3]);
                    float bump      = amplitude * dist(m_random.consume(patch.id.seed(mx, my)));
                    float midValue  = avg + bump;

                    // Store mid-point value
                    uint32_t im     = wrappingIndex(mx, my);
                    data[im]        = midValue;

                    // Record min and max
                    minH            = std::min<float>(minH, midValue);
                    maxH            = std::max<float>(maxH, midValue);
                }
            }

            // Diamond step
            for (int y = 0; y < parts; y++)
            {
                for (int x = 0; x < parts; x++)
                {
                    // Corner indices
                    uint32_t i0     = wrappingIndex(      x * step,                  y * step);
                    uint32_t i1     = wrappingIndex((x + 1) * step,                  y * step);
                    uint32_t i2     = wrappingIndex(      x * step,            (y + 1) * step);
                    uint32_t im     = wrappingIndex(      x * step + step / 2,       y * step + step / 2);

                    // Wrapping left and top indices
                    uint32_t i4     = wrappingIndex(x * step - step / 2, y * step + step / 2);
                    uint32_t i5     = wrappingIndex(x * step + step / 2, y * step - step / 2);

                    // Left value
                    int mx          = x * step;
                    int my          = y * step + step / 2;
                    float avg       = 0.25f * (data[i0] + data[i4] + data[i2] + data[im]);
                    float bump      = amplitude * dist(m_random.consume(patch.id.seed(mx, my)));
                    float leftValue = avg + bump;

                    // Store left value
                    uint32_t il     = wrappingIndex(mx, my);
                    data[il]        = leftValue;

                    // Record min and max
                    minH            = std::min<float>(minH, leftValue);
                    maxH            = std::max<float>(maxH, leftValue);

                    // Top value
                    mx              = x * step + step / 2;
                    my              = y * step;
                    avg             = 0.25f * (data[i0] + data[i1] + data[i5] + data[im]);
                    bump            = amplitude * dist(m_random.consume(patch.id.seed(mx, my)));
                    float topValue  = avg + bump;

                    // Store top value
                    uint32_t it     = wrappingIndex(mx, my);
                    data[it]        = topValue;

                    // Record min and max
                    minH            = std::min<float>(minH, topValue);
                    maxH            = std::max<float>(maxH, topValue);
                }
            }

            step >>= 1;
            parts <<= 1;
        }
        
        collectProcessedPatch(targetLayer, minH, maxH);
        
        patch.minHeight = minH;
        patch.maxHeight = maxH;
        
        m_patchCache.dataReady(patch.id);
    }    

    // Use the parent layer as a basis and generate data in between its samples
    void PatchGenerator::generateChildPatch(Patch& patch, Image& targetLayer)
    {
        std::uniform_real_distribution<float> dist(-patch.steepness, patch.steepness);

        float amplitude     = MaxAmplitude * powf(0.5f, static_cast<float>(patch.id.mip() + 7));

        const Image& pLayer = m_patchCache.patchData(patch.id.mip() - 1);
        
        auto data           = m_processingPatch.asRange<float>();
        auto pData          = pLayer.asRange<const uint16_t>();
        
        uint32_t parentWrapSize = std::min<uint32_t>(PatchResolution << (patch.id.mip() - 1), PatchCacheSize);

        auto indexWithWrapping = [&](int x, int y) -> uint32_t
        {
            x = (x + parentWrapSize) & (parentWrapSize - 1);
            y = (y + parentWrapSize) & (parentWrapSize - 1);
            return static_cast<uint32_t>(y * targetLayer.width() + x);
        };

        auto indexWithBorders = [&](int x, int y) -> uint32_t
        {
            x = x + PatchBorder;
            y = y + PatchBorder;
            return static_cast<uint32_t>(y * PatchSizeWithBorders + x);
        };        

        auto fValue = [&](uint16_t scaledValue, int px, int py) -> float
        {
            int x = (px + parentWrapSize) & (parentWrapSize - 1);
            int y = (py + parentWrapSize) & (parentWrapSize - 1);
            PatchId parent(x / PatchResolution, y / PatchResolution, patch.id.mip() - 1);
            // TODO: Fetching parent over and over again is expensive, although for the majority of pixels
            //       it stays the same. Optimize this!
            Patch& parentPatch  = m_patchCache.patchMetadata(parent);
            float hScaleInv     = (parentPatch.maxHeight - parentPatch.minHeight) / 65535.f;
            return scaledValue * hScaleInv + parentPatch.minHeight;
        };

        int vx = patch.id.x() * PatchResolution / 2;
        int vy = patch.id.y() * PatchResolution / 2;

        uint32_t topLeft = indexWithWrapping(vx, vy);

        float value      = fValue(pData[topLeft], vx, vy);
        float minH       = value;
        float maxH       = value;

        // Generate 2x2 square of pixels at a time
        for (int y = 0; y < PatchSizeWithBorders; y += 2)
        {
            int dy      = (y - PatchBorder) / 2;
            int py      = vy + dy;

            for (int x = 0; x < PatchSizeWithBorders; x += 2)
            {
                int dx          = (x - PatchBorder) / 2;
                int px          = vx + dx;

                // Parent corner indices
                uint32_t i0     = indexWithWrapping(    px,     py);
                uint32_t i1     = indexWithWrapping(px + 1,     py);
                uint32_t i2     = indexWithWrapping(    px, py + 1);
                uint32_t i3     = indexWithWrapping(px + 1, py + 1);

                // Copy parent top-left corner
                float topLeftValue  = fValue(pData[i0], px, py);

                uint32_t it     = indexWithBorders(x - PatchBorder, y - PatchBorder);
                data[it]        = topLeftValue;
                
                // Record min and max
                minH            = std::min<float>(minH, topLeftValue);
                maxH            = std::max<float>(maxH, topLeftValue);

                // Mid-point value
                int mx          = x + 1 - PatchBorder;
                int my          = y + 1 - PatchBorder;
                float avg       = 0.25f * (fValue(pData[i0], px, py) +
                                           fValue(pData[i1], px + 1, py) +
                                           fValue(pData[i2], px, py + 1) +
                                           fValue(pData[i3], px + 1, py + 1));
                float bump      = amplitude * dist(m_random.consume(patch.id.seed(mx, my)));
                float midValue  = avg + bump;

                // Store mid-point value
                uint32_t im     = indexWithBorders(mx, my);
                data[im]        = midValue;

                // Record min and max
                minH            = std::min<float>(minH, midValue);
                maxH            = std::max<float>(maxH, midValue);
            }
        }

        auto diamond = [&](int x, int y)
        {
            // Corner indices
            uint32_t i0     = indexWithBorders(    x,     y);
            uint32_t i1     = indexWithBorders(x + 2,     y);
            uint32_t i2     = indexWithBorders(    x, y + 2);
            uint32_t i3     = indexWithBorders(x + 1, y + 1);
            uint32_t i4     = indexWithBorders(x - 1, y + 1);
            uint32_t i5     = indexWithBorders(x + 1, y - 1);

            int mx          = x;
            int my          = y + 1;
            float avg       = 0.25f * (data[i0] + data[i2] + data[i3] + data[i4]);
            float bump      = amplitude * dist(m_random.consume(patch.id.seed(mx, my)));
            float leftValue = avg + bump;

            uint32_t il     = indexWithBorders(mx, my);
            data[il]        = leftValue;

            // Record min and max
            minH            = std::min<float>(minH, leftValue);
            maxH            = std::max<float>(maxH, leftValue);

            mx              = x + 1;
            my              = y;
            avg             = 0.25f * (data[i0] + data[i1] + data[i3] + data[i5]);
            bump            = amplitude * dist(m_random.consume(patch.id.seed(mx, my)));
            float topValue  = avg + bump;

            uint32_t it     = indexWithBorders(mx, my);
            data[it]        = topValue;

            // Record min and max
            minH            = std::min<float>(minH, topValue);
            maxH            = std::max<float>(maxH, topValue);
        };
        
        for (int y = 0; y < PatchResolution; y += 2)
        {            
            for (int x = 0; x < PatchResolution; x += 2)
            {
                diamond(x, y);
            }
        }
        
        collectProcessedPatch(targetLayer, minH, maxH, patch.id.x() * PatchResolution, patch.id.y() * PatchResolution);
        
        patch.minHeight = minH;
        patch.maxHeight = maxH;
        
        m_patchCache.dataReady(patch.id);
    }

    void PatchGenerator::collectProcessedPatch(Image& targetLayer, float minH, float maxH, int xOffset, int yOffset)
    {
        auto data       = m_processingPatch.asRange<const float>();
        auto output     = targetLayer.asRange<uint16_t>();
        float hScale    = 65535.f / (maxH - minH);

        for (int y = 0; y < PatchResolution; y++)
        {
            for (int x = 0; x < PatchResolution; x++)
            {                
                float value = data[(y + PatchBorder) * PatchSizeWithBorders + x + PatchBorder];
                uint32_t i  = (y + yOffset) * targetLayer.width() + (x + xOffset);
                output[i]   = static_cast<uint16_t>((value - minH) * hScale);
            }
        }
    }
}
