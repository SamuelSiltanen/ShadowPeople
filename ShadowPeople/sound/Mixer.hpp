#pragma once

#include <memory>
#include <random>
#include "../Types.hpp"
#include "AudioFormat.hpp"

namespace sound
{
    class RawAudioBuffer;

    class Mixer
    {
    public:
        Mixer(const AudioFormat& format);

        void setFormat(const AudioFormat& format);

        void mix(std::shared_ptr<RawAudioBuffer> rawAudioBuffer);
    private:
        void mixUnorm8(Range<uint8_t> data);
        void mixUnorm16(Range<uint16_t> data);
        void mixUnorm32(Range<uint32_t> data);
        void mixFloat32(Range<float> data);

        AudioFormat m_format;
        float       m_mixerTime;

        std::mt19937 m_gen;
    };
}
