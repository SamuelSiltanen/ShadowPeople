#include "SoundDevice.hpp"
#include "RawAudioBuffer.hpp"
#include "AudioFormat.hpp"
#include "../Errors.hpp"

#include <objbase.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

#define SAFE_RELEASE(ptr) if (ptr) { ptr->Release(); ptr = nullptr; }

namespace sound
{
    static const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
    static const IID IID_IMMDeviceEnumerator    = __uuidof(IMMDeviceEnumerator);
    static const IID IID_IAudioClient           = __uuidof(IAudioClient);
    static const IID IID_IAudioRenderClient     = __uuidof(IAudioRenderClient);

    static const REFERENCE_TIME RefsPerSec      = 10000000;

    struct SoundDeviceDetails
    {
        IMMDeviceEnumerator* pEnumerator;
        IMMDevice*           pDevice;
        IAudioClient*        pAudioClient;
        IAudioRenderClient*  pRenderClient;
        WAVEFORMATEX*        pwfx;

        SoundDeviceDetails() :
            pEnumerator(nullptr),
            pDevice(nullptr),
            pAudioClient(nullptr),
            pRenderClient(nullptr),
            pwfx(nullptr) {}
        ~SoundDeviceDetails()
        {
            CoTaskMemFree(pwfx);
            SAFE_RELEASE(pEnumerator);
            SAFE_RELEASE(pDevice);
            SAFE_RELEASE(pAudioClient);
            SAFE_RELEASE(pRenderClient);
        }
    };

    SoundDevice::SoundDevice()
    {
        CoInitializeEx(NULL, COINITBASE_MULTITHREADED);
        m_details = std::make_shared<SoundDeviceDetails>();
        if (!initWASAPIDevice())
        {
            releaseWASAPIDevice();
        }
    }

    SoundDevice::~SoundDevice()
    {
        releaseWASAPIDevice();
        CoUninitialize();
    }



    AudioFormat SoundDevice::getFormat() const
    {
        AudioFormat format;
        WAVEFORMATEX* pwfx = m_details->pwfx;

        if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
        {
            SP_ASSERT(pwfx->cbSize >= 22, "Not enough data for WAVE_FORMAT_EXTENSIBLE");
            WAVEFORMATEXTENSIBLE *pwfe = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx);
            if (pwfe->SubFormat == KSDATAFORMAT_SUBTYPE_PCM)
            {
                format.numericFormat = AudioFormat::NumericFormat::Unorm;
            }
            else if (pwfe->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)
            {
                format.numericFormat = AudioFormat::NumericFormat::Float;
            }
            else
            {
                SP_ASSERT(false, "Only PCM and float mixing formats currentl supported.");
            }
            format.numChannels      = pwfe->Format.nChannels;
            format.bitsPerSample    = pwfe->Format.wBitsPerSample;
            format.samplesPerSec    = pwfe->Format.nSamplesPerSec;
        }
        else
        {
            SP_ASSERT(pwfx->wFormatTag == WAVE_FORMAT_PCM, "Only PCM mixing format currently supported.");
            format.numericFormat    = AudioFormat::NumericFormat::Unorm;
            format.numChannels      = pwfx->nChannels;
            format.bitsPerSample    = pwfx->wBitsPerSample;
            format.samplesPerSec    = pwfx->nSamplesPerSec;
        }

        return format;
    }

    std::shared_ptr<RawAudioBuffer> SoundDevice::getBuffer(AudioBufferType type)
    {
        uint32_t numFramesAvailable = m_bufferFrameCount;
        if (type == AudioBufferType::PartialBuffer)
        {
            uint32_t numFramesPadding = 0;
            if (FAILED(m_details->pAudioClient->GetCurrentPadding(&numFramesPadding)))
            {
                numFramesAvailable = 0;
            }
            else
            {
                numFramesAvailable -= numFramesPadding;
            }
        }
        return std::make_shared<RawAudioBuffer>(
            [this, numFramesAvailable]() -> Range<uint8_t>
        {
            BYTE* pData = nullptr;
            if (FAILED(m_details->pRenderClient->GetBuffer(numFramesAvailable, &pData))) return Range<uint8_t>();
            return Range<uint8_t>(pData, numFramesAvailable * getFormat().elementSize());
        },
            [this, numFramesAvailable](uint32_t flags) -> bool
        {
            if (FAILED(m_details->pRenderClient->ReleaseBuffer(numFramesAvailable, flags))) return false;
            return true;
        });
    }

    bool SoundDevice::play()
    {
        if (FAILED(m_details->pAudioClient->Start())) return false;
    }

    bool SoundDevice::stop()
    {
        if (FAILED(m_details->pAudioClient->Stop())) return false;
    }

    bool SoundDevice::initWASAPIDevice()
    {
        REFERENCE_TIME hnsBufferDuration = RefsPerSec;

        if (FAILED(CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL,
                                    IID_IMMDeviceEnumerator, (void **)&m_details->pEnumerator))) return false;
        if (FAILED(m_details->pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                                   &m_details->pDevice))) return false;
        if (FAILED(m_details->pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL,
                                                (void **)&m_details->pAudioClient))) return false;

        auto client = m_details->pAudioClient;
        if (FAILED(client->GetMixFormat(&m_details->pwfx))) return false;
        if (FAILED(client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0,
                                      hnsBufferDuration, 0, m_details->pwfx, NULL))) return false;
        if (FAILED(client->GetBufferSize(&m_bufferFrameCount))) return false;
        if (FAILED(client->GetService(IID_IAudioRenderClient, (void**)&m_details->pRenderClient))) return false;        

        return true;
    }

    void SoundDevice::releaseWASAPIDevice()
    {
        // TODO: Make sure no raw audio buffers are alive
        m_details = nullptr;
    }
}
