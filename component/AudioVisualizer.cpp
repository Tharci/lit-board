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


lbd::comp::AudioVisualizer::AudioVisualizer()
    : audioClientProvider(std::bind(&AudioVisualizer::onConnectedCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)) {
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
        std::cout << "[ERROR] Cannot listen to audio channel. Expected number of channels is 2.\n";
        captureClient = nullptr;
        audioClient = nullptr;
    }

    // The buffer contains 200ms of data.
    auto bufferSize = largest2PowerBelow(format->nAvgBytesPerSec / 5 / 4 / (format->wBitsPerSample / 8));
    buffer.resize(bufferSize);
    fftBuffer.resize(bufferSize);
    singleChannel.resize(bufferSize / 2);
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
    if (running && currTime - packageSentMs > 30) {
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
    for (int i = 0; i < singleChannel.size(); i++) {
        singleChannel[i].real(fftBuffer[(i * 2 + fftBufferIdx) % fftBuffer.size()] + fftBuffer[(i * 2 + fftBufferIdx + 1) % fftBuffer.size()]);
        singleChannel[i].imag(0);
    }

    const char* error1 = nullptr;
    auto b1 = simple_fft::FFT(singleChannel, singleChannel.size(), error1);
    if (!b1) {
        printf("%s\n", error1);
    }
    else {
        const auto maxFreq = 21000;
        const float deltaFreq = (float) maxFreq / singleChannel.size();
        const int intervalCount = 14;
        const int intervalStart = 20 / deltaFreq;
        const int intervalLength = 3000 / deltaFreq;
        std::vector<unsigned char> soundData;
        soundData.reserve(intervalCount);

        for (int i = 0; i < intervalCount; i++) {
            double sum = 0;
            const int subIntervalLength = intervalLength / intervalCount;
            for (int j = i * subIntervalLength; j < (i + 1) * subIntervalLength; j++) {
                sum += std::abs(singleChannel[intervalStart + j].real());
            }
            soundData.push_back((unsigned char)min(sum / 11 * (1 + i / 14. * 1.7), 100));
        }

        LitBoardDriver::getInstance().getMessageHandler().send(*this, soundData.data(), intervalCount);
    }
}



#define RETURN_ON_ERROR(hres)  \
                  if (FAILED(hres)) { packetLength = 0; audioClient = nullptr; captureClient = nullptr; audioClientProvider.reconnect(); Sleep(100); return false; }


const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

void lbd::comp::AudioVisualizer::startLoopbackAudio() {
    std::thread([&] {
        bool done = false;

        CoInitialize(nullptr);

        audioClientProvider.reconnect();

        // Each loop fills about half of the shared buffer.
        while (!done) {
            // Sleep for half the buffer duration.
            audioClientProvider.keepConnection();
            const auto sleepTime = hnsActualDuration/REFTIMES_PER_MILLISEC/2/30;
            Sleep(sleepTime);
            done = copyAudioBufferData();
        }

        audioClient->Stop();  // Stop recording.

        // return hr;
    }).detach();
}

void lbd::comp::AudioVisualizer::onConnectedCallback(
        IAudioClient* newAudioClient, IAudioCaptureClient* newCaptureClient, WAVEFORMATEX* format,  UINT32 bufferFrameCount) {
    audioClient = newAudioClient;
    captureClient = newCaptureClient;

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double) REFTIMES_PER_SEC * bufferFrameCount / format->nSamplesPerSec;

    SetFormat(format);
}

bool lbd::comp::AudioVisualizer::copyAudioBufferData() {
    UINT32 packetLength = 0;
    HRESULT hr;
    BYTE* pData;
    UINT32 numFramesAvailable;
    DWORD flags;
    BOOL done = false;

    if (!captureClient) {
        audioClientProvider.reconnect();
        Sleep(500);
        return false;
    }

    hr = captureClient->GetNextPacketSize(&packetLength);
    RETURN_ON_ERROR(hr)

    while (packetLength != 0) {
        // Get the available data in the shared buffer.
        hr = captureClient->GetBuffer(
                &pData,
                &numFramesAvailable,
                &flags, nullptr, nullptr);
        RETURN_ON_ERROR(hr)

        if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
            pData = nullptr;  // Tell CopyData to write silence.
        }

        // Copy the available capture data to the audio sink.
        hr = CopyData(pData, numFramesAvailable, &done);
        RETURN_ON_ERROR(hr)

        hr = captureClient->ReleaseBuffer(numFramesAvailable);
        RETURN_ON_ERROR(hr)

        hr = captureClient->GetNextPacketSize(&packetLength);
        RETURN_ON_ERROR(hr)
    }

    return done;
}
