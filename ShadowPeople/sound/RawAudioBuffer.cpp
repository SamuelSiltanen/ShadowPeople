#include "RawAudioBuffer.hpp"
#include "SoundDevice.hpp"
#include "../Errors.hpp"

namespace sound
{
    RawAudioBuffer::RawAudioBuffer(std::function<Range<uint8_t>()> acquire,
                                   std::function<bool(uint32_t flags)> release) :
        m_release(release),
        m_flags(0)
    {
        m_data = acquire();
        SP_ASSERT(m_data.size() != 0, "Something went wrong when requesting an audio buffer!");
    }

    RawAudioBuffer::~RawAudioBuffer()
    {        
        SP_ASSERT(m_release(m_flags), "Failed to release an audio buffer!");
    }
}
