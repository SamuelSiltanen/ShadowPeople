/*
    Copyright 2018 Samuel Siltanen
    PatchGenerator.hpp
*/

#pragma once

#include "Patch.hpp"

#include <random>

namespace graphics
{
    class Image;
}

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

        PatchCache&     m_patchCache;
        std::mt19937    m_random;
    };
}
