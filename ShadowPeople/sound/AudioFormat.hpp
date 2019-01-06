#pragma once

#include <cstdint>

namespace sound
{
    struct AudioFormat
    {
        enum class NumericFormat
        {
            Unorm,
            Snorm,
            Float
        };

        NumericFormat numericFormat;
        uint32_t numChannels;
        uint32_t bitsPerSample;
        uint32_t samplesPerSec;

        uint32_t elementSize() const { return numChannels * bitsPerSample / 8; }
    };
}
