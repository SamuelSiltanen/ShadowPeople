#pragma once

#include "../Types.hpp"
#include <functional>

namespace sound
{
    class RawAudioBuffer
    {
    public:        
        RawAudioBuffer(std::function<Range<uint8_t>()> acquire,
                       std::function<bool(uint32_t flags)> release);
        ~RawAudioBuffer();

        Range<uint8_t> data() { return m_data; }
    private:        
        std::function<bool(uint32_t flags)> m_release;
        uint32_t        m_flags;
        Range<uint8_t>  m_data;
    };
}
