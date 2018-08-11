/*
    Copyright 2018 Samuel Siltanen
    PatchCache.hpp
*/

#pragma once

#include "../graphics/Graphics.hpp"
#include "../FreeList.hpp"
#include "Patch.hpp"

#include <unordered_map>
#include <queue>

namespace rendering
{
    // There are 21 levels of mipmaps:
    // 256 km [  2 km], 128 km [  1 km], 64 km [ 512 m ], 32 km [ 256 m ], 16 km [ 128 m ],
    //   8 km [ 64 m ],   4 km [ 32  m],  2 km [  16 m ],  1 km [   8 m ], 512 m [   4 m ],
    // 256 m  [  2 m ], 128 m  [  1  m], 64 m  [51.2 cm], 32 m  [25.6 cm],  16 m [12.8 cm],
    //   8 m  [6.4 cm],   4 m  [3.2 cm],  2 m  [ 1.6 cm],  1 m  [ 0.8 cm], 0.5 m [ 0.4 cm],
    // 0.25 m [0.2 cm]
    constexpr uint32_t PatchMipLevels        = 21;
    constexpr uint32_t PatchesOnMipSqrt      = 16;
    constexpr uint32_t PatchCacheSize        = PatchResolution * PatchesOnMipSqrt;
    constexpr uint32_t PatchesOnMip          = PatchesOnMipSqrt * PatchesOnMipSqrt;
    constexpr uint32_t PatchCacheMaxElements = PatchMipLevels * PatchesOnMip;
    constexpr uint32_t PatchMipsAlwaysResident = 5;

    class PatchCache
    {
    public:
        PatchCache(graphics::Device& device);

        // Returns cache offset of the requested page or one of its parent in the hierarchy
        uint32_t request(PatchId id);

        // Evict a patch that is no longer needed
        void evict(PatchId id);

        // Inform that some patch related data has changed and should be updated to GPU
        void dataReady(PatchId id);

        // Access patch data
        const graphics::Image& patchData(uint32_t i) const { return m_patchDataCPU[i]; }
        graphics::Image& patchData(uint32_t i)             { return m_patchDataCPU[i]; }
        const graphics::TextureView patchDataGPU() const   { return m_patchDataSRV; }

        // Access patch metadata
        const Patch& patchMetadata(PatchId id) const;
        Patch& patchMetadata(PatchId id);
        const graphics::BufferView patchMetadataGPU() const { return m_patchMetadataSRV; }

        PatchId nextGenerationRequest();

        void updateGPUBuffersAndTextures(graphics::CommandBuffer& gfx);
    private:
        void addPatch(PatchId id);
        void addPermanentlyResidentPatches();

        graphics::Texture                   m_patchData;
        graphics::TextureView               m_patchDataSRV;
        std::array<graphics::Image, PatchMipLevels>   m_patchDataCPU;

        graphics::Buffer                    m_patchMetadata;
        graphics::BufferView                m_patchMetadataSRV;
        std::vector<Patch>                  m_patchMetadataCPU;

        FreeList                            m_patchAllocator;

        std::unordered_map<PatchId, uint32_t>   m_idToOffset;
        std::queue<PatchId>                 m_generationRequests;

        std::vector<PatchId>                m_dirtyPatches;
    };
}
