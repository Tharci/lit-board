//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_AUDIOVISUALIZER_H
#define LITBOARD_AUDIOVISUALIZER_H

#include "CyclicComponent.h"
#include "simple_fft/fft.h"
#include "AudioClientProvider.h"


namespace lbd::comp {
    class AudioVisualizer : public Component {
    public:
        AudioVisualizer();
        [[nodiscard]] ComponentId getComponentId() const override;

    protected:
        void onMessageReceived(uint8_t* data, size_t length) override;

    private:
        enum class Commands {
            STOP  = 0,
            START = 1
        };

        void start();
        void stop();
        void onKeyboardDisconnected() override;
        void startLoopbackAudio();

        std::atomic<bool> running = false;

        tWAVEFORMATEX format;
        std::vector<float> buffer;
        size_t bufferIdx;
        uint64_t packageSentMs {};

        std::vector<complex_type> firstChannel;
        std::vector<complex_type> secondChannel;
        std::vector<float> fftBuffer;
        size_t fftBufferIdx;

        AudioClientProvider audioClientProvider;

        HRESULT SetFormat(WAVEFORMATEX* format);
        HRESULT CopyData(byte* byteBuff, unsigned buffLen, BOOL* b);
        void doFFT();
    };
}



#endif //LITBOARD_AUDIOVISUALIZER_H
