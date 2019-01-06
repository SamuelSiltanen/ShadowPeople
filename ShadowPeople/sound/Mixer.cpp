#include "Mixer.hpp"
#include "RawAudioBuffer.hpp"
#include "../Errors.hpp"
#include "../Math.hpp"

#include <cmath>
#include <random>

namespace sound
{
    Mixer::Mixer(const AudioFormat& format) :
        m_format(format),
        m_mixerTime(0.f),
        m_gen(0x52952388)
    {}

    void Mixer::setFormat(const AudioFormat& format)
    {
        m_format = format;
    }

    void Mixer::mix(std::shared_ptr<RawAudioBuffer> rawAudioBuffer)
    {
        if (m_format.numericFormat == AudioFormat::NumericFormat::Unorm)
        {
            if (m_format.bitsPerSample == 8)
            {
                mixUnorm8(rawAudioBuffer->data());
            }
            else if (m_format.bitsPerSample == 16)
            {
                mixUnorm16(Range<uint16_t>(reinterpret_cast<uint16_t*>(rawAudioBuffer->data().begin()),
                                                                       rawAudioBuffer->data().size()));
            }
            else if (m_format.bitsPerSample == 32)
            {
                mixUnorm32(Range<uint32_t>(reinterpret_cast<uint32_t*>(rawAudioBuffer->data().begin()),
                                                                       rawAudioBuffer->data().size()));
            }
            else
            {
                SP_ASSERT(false, "Unsupported mixing format.");
            }
        }
        else if (m_format.numericFormat == AudioFormat::NumericFormat::Float)
        {
            mixFloat32(Range<float>(reinterpret_cast<float*>(rawAudioBuffer->data().begin()),
                                                             rawAudioBuffer->data().size()));
        }        
        else
        {
            SP_ASSERT(false, "Unsupported mixing format.");
        }
    }

    void Mixer::mixUnorm8(Range<uint8_t> data)
    {

    }

    void Mixer::mixUnorm16(Range<uint16_t> data)
    {

    }

    void Mixer::mixUnorm32(Range<uint32_t> data)
    {

    }

    void Mixer::mixFloat32(Range<float> data)
    {
        std::uniform_real_distribution<float> dist(-1, 1);

        float timeStep      = 1.f / static_cast<float>(m_format.samplesPerSec);
        uint32_t numSamples = data.size() / m_format.numChannels;
        for (uint32_t i = 0; i < numSamples; i++)
        {
#if 0
            float A1 = 0.1f * (0.5f + 0.5f * sinf(m_mixerTime * 2.f * math::Pi * 0.512f));
            float A2 = 0.1f * (0.5f + 0.5f * sinf(m_mixerTime * 2.f * math::Pi * 0.237f));
            float A3 = 0.1f * (0.5f + 0.5f * sinf(m_mixerTime * 2.f * math::Pi * 0.134f));
            float noise1 = dist(m_gen);
            float noise2 = dist(m_gen);
            float noise3 = dist(m_gen);
            float value = A1 * noise1 + A2 * noise2 + A3 * noise3;
            for (uint32_t ch = 0; ch < m_format.numChannels; ch++)
            {                
                data[i * m_format.numChannels + ch] = value;
            }
#else
            for (uint32_t ch = 0; ch < m_format.numChannels; ch++)
            {                
                data[i * m_format.numChannels + ch] = 0.f;
            }
#endif

            m_mixerTime += timeStep;
        }
    }
}