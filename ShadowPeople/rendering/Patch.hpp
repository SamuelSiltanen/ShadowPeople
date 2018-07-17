#pragma once

#include <functional>
#include <stdint.h>

#include "../Types.hpp"

namespace rendering
{
    constexpr uint32_t PatchResolution = 256;

    struct PatchId
    {
        uint64_t id;

        static const uint64_t InvalidId = ~0;

        PatchId(uint32_t x, uint32_t y, uint32_t mip)
        {
            id = static_cast<uint64_t>(x & 0xfffff) | 
                 (static_cast<uint64_t>(y & 0xfffff) << 20) |
                 (static_cast<uint64_t>(mip) << 40);
        }
        PatchId(uint64_t id) : id(id) {}

        bool operator==(const PatchId& patchId) const noexcept
        {
            return id == patchId.id;
        }

        uint32_t x() const     { return static_cast<uint32_t>(id & 0xfffff); }
        uint32_t y() const     { return static_cast<uint32_t>((id & 20) & 0xfffff); }
        uint32_t mip() const   { return static_cast<uint32_t>(id >> 40); }
        PatchId parent() const { return PatchId(x() / 2, y() / 2, mip() - 1); }
    };

    struct Patch
    {
        PatchId id;

        uint32_t cacheOffset;

        uint32_t seed;
        uint32_t steepness;

        uint32_t minHeight;
        uint32_t maxHeight;

        DataBlob<uint32_t> data;
    };
}

// Inject a hash function for the PatchId
namespace std
{
    template<> struct hash<rendering::PatchId>
    {
        std::size_t operator()(rendering::PatchId const& patchId) const noexcept
        {
            return std::hash<uint64_t>{}(patchId.id);
        }
    };
}

