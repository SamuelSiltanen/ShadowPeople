/*
    Copyright 2018 Samuel Siltanen
    PatchGenerator.hpp
*/

#pragma once

#include "Patch.hpp"

namespace rendering
{
    class PatchCache;

    class PatchGenerator
    {
    public:
        PatchGenerator(PatchCache& patchCache);

        void generatePatchData(PatchId id);
    private:
        PatchCache& m_patchCache;
    };
}
