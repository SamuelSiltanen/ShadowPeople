/*
    Copyright 2018 Samuel Siltanen
    PatchGenerator.hpp
*/

#pragma once

#include "Patch.hpp"
#include "../graphics/Image.hpp"
#include "../Hash.hpp"

namespace rendering
{
    class PatchCache;

    class PatchGenerator
    {
    public:
        PatchGenerator(PatchCache& patchCache);

        void generatePatchData(PatchId id);
    private:
        void generatePermanentlyResidentPatches();
        void generateRootPatch(Patch& patch, graphics::Image& targetLayer);
        void generateChildPatch(Patch& patch, graphics::Image& targetLayer);

        void collectProcessedPatch(graphics::Image& targetLayer, float minH, float maxH,
                                   int xOffset = 0, int yOffset = 0);

        PatchCache&     m_patchCache;
        FNV1a           m_random;
        graphics::Image m_processingPatch;
    };
}
