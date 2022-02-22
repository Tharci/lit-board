//
// Created by tmarc on 14/07/2021.
//

#include <LitBoardDriver.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <thread>
#include <iostream>
#include <vector>
#include <chrono>
#include "AudioVisualizer.h"


lbd::comp::AudioVisualizer::AudioVisualizer() {
    startLoopbackAudio();
}

lbd::comp::ComponentId lbd::comp::AudioVisualizer::getComponentId() const {
    return ComponentId::AudioVisualizer;
}

void lbd::comp::AudioVisualizer::onMessageReceived(uint8_t* data, size_t length) {
    if (length < 1) {
        std::cout << "[ERROR][AUDIO_VISUALIZER] Unexpected message length of '" << length << "' received. Expected length of 1.\n";
        return;
    }

    switch (data[0]) {
        case (int) Commands::START:
            start();
            break;
        case (int) Commands::STOP:
            stop();
            break;
        default:
            std::cout << "[ERROR][AUDIO_VISUALIZER] Unexpected message '" << data[0] << "' received. Expected '0' or '1'.\n";
    }
}

void lbd::comp::AudioVisualizer::start() {
    running = true;
    std::cout << "[INFO][AUDIO_VISUALIZER] Started.\n";
}

void lbd::comp::AudioVisualizer::stop() {
    if (running) {
        std::cout << "[INFO][AUDIO_VISUALIZER] Stopped.\n";
    }
    running = false;
}

void lbd::comp::AudioVisualizer::onKeyboardDisconnected() {
    stop();
}

uint64_t getTimeMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

uint64_t largest2PowerBelow(uint64_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v / 2;
}

HRESULT lbd::comp::AudioVisualizer::SetFormat(WAVEFORMATEX* format) {
    if (format->nChannels != 2) {
        throw std::exception("Expected number of channels is 2.");
    }

    this->format = *format;
    // The buffer contains 200ms of data.
    auto bufferSize = largest2PowerBelow(format->nAvgBytesPerSec / 5 / 8 / (format->wBitsPerSample / 8));
    buffer.resize(bufferSize);
    fftBuffer.resize(bufferSize);
    firstChannel.resize(bufferSize / 2);
    secondChannel.resize(bufferSize / 2);
    std::memset(buffer.data(), 0, buffer.size() * (format->wBitsPerSample / 8));
    bufferIdx = 1;

    return 0;
}

HRESULT lbd::comp::AudioVisualizer::CopyData(byte* byteBuff, unsigned buffLen, BOOL* b) {
    auto* buff = (float*) byteBuff;
    buffLen /= 4;

    if (byteBuff == nullptr) {
        for (int i = 0; i < buffLen; i++) {
            buffer[(bufferIdx + i) % buffer.size()] = 0;
        }
    }
    else {
        for (int i = 0; i < buffLen; i++) {
            buffer[(bufferIdx + i) % buffer.size()] = buff[i];
        }
    }

    bufferIdx = (bufferIdx + buffLen) % buffer.size();

    auto currTime = getTimeMs();
    if (running && currTime - packageSentMs > 80) {
        fftBufferIdx = bufferIdx;
        memcpy(fftBuffer.data(), buffer.data(), buffer.size() * 4);

        std::thread([&] {
            doFFT();
        }).detach();

        packageSentMs = currTime;
    }

    return 0;
}

void lbd::comp::AudioVisualizer::doFFT() {
    for (int i = 0; i < firstChannel.size(); i++) {
        firstChannel[i].real(fftBuffer[(i * 2 + fftBufferIdx) % fftBuffer.size()]);
        firstChannel[i].imag(0);
        secondChannel[i].real(fftBuffer[(i * 2 + fftBufferIdx + 1) % fftBuffer.size()]);
        secondChannel[i].imag(0);
    }

    const char* error = nullptr;
    auto b = simple_fft::FFT(firstChannel, firstChannel.size(), error);
    if (!b) {
        printf("%s\n", error);
    }
    else {
        const auto maxFreq = 21000;
        const float deltaFreq = (float) maxFreq / firstChannel.size()
                                    * 1.6; // No idea why this is needed.
        const int intervalCount = 14;
        const int intervalStart = 40 / deltaFreq;
        const int intervalLength = 3000 / deltaFreq;
        std::vector<unsigned char> soundData;
        soundData.reserve(intervalCount);

        for (int i = 0; i < intervalCount; i++) {
            double sum = 0;
            const int subIntervalLength = intervalLength / intervalCount;
            for (int j = i * subIntervalLength; j < (i + 1) * subIntervalLength; j++) {
                sum += std::abs(firstChannel[intervalStart + j].real());
                sum += std::abs(secondChannel[intervalStart + j].real());
            }
            soundData.push_back((unsigned char)min(sum, 300) / 1.5);
        }

        for (int i = 0; i < intervalCount; i++) {
            soundData[i] *= (1 + i / 14. * 1.25);
        }

        LitBoardDriver::getInstance().getMessageHandler().send(*this, soundData.data(), intervalCount);
    }
}



#define RECONNECT_ON_ERROR(hres)  \
                  if (FAILED(hres)) { packetLength = 0; audioClientProvider.reconnect(); continue; }


const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

void lbd::comp::AudioVisualizer::startLoopbackAudio() {
    std::thread([&] {
        HRESULT hr;
        UINT32 bufferFrameCount;
        BOOL bDone = FALSE;
        UINT32 packetLength = 0;
        BYTE* pData;
        UINT32 numFramesAvailable;
        DWORD flags;

        CoInitialize(NULL);

        // Get the size of the allocated buffer.
        hr = audioClientProvider.getAudioClient()->GetBufferSize(&bufferFrameCount);
        // RECONNECT_ON_ERROR(hr)

        // Notify the audio sink which format to use.
        SetFormat(audioClientProvider.getFormat());

        const auto sleepTime = audioClientProvider.getActualDuration()/REFTIMES_PER_MILLISEC/2/30;
        // Each loop fills about half of the shared buffer.
        while (bDone == FALSE)
        {
            // Sleep for half the buffer duration.
            Sleep(sleepTime);

            hr = audioClientProvider.getCaptureClient()->GetNextPacketSize(&packetLength);
            RECONNECT_ON_ERROR(hr)

            while (packetLength != 0)
            {
                // Get the available data in the shared buffer.
                hr = audioClientProvider.getCaptureClient()->GetBuffer(
                        &pData,
                        &numFramesAvailable,
                        &flags, NULL, NULL);
                RECONNECT_ON_ERROR(hr)

                if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
                {
                    pData = NULL;  // Tell CopyData to write silence.
                }

                // Copy the available capture data to the audio sink.
                hr = CopyData(pData, numFramesAvailable, &bDone);
                RECONNECT_ON_ERROR(hr)

                hr = audioClientProvider.getCaptureClient()->ReleaseBuffer(numFramesAvailable);
                RECONNECT_ON_ERROR(hr)

                hr = audioClientProvider.getCaptureClient()->GetNextPacketSize(&packetLength);
                RECONNECT_ON_ERROR(hr)
            }
        }

        hr = audioClientProvider.getAudioClient()->Stop();  // Stop recording.

        // return hr;
    }).detach();
}
