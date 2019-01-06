#pragma once

#include <memory>

namespace sound
{
    struct SoundDeviceDetails;
    struct AudioFormat;
    class RawAudioBuffer;

    enum class AudioBufferType
    {
        WholeBuffer,
        PartialBuffer
    };

    class SoundDevice
    {
    public:
        SoundDevice();
        ~SoundDevice();

        AudioFormat getFormat() const;
        std::shared_ptr<RawAudioBuffer> getBuffer(AudioBufferType type);

        bool play();
        bool stop();
    private:
        bool initWASAPIDevice();
        void releaseWASAPIDevice();

        std::shared_ptr<SoundDeviceDetails>  m_details;

        unsigned                             m_bufferFrameCount;
    };
}
