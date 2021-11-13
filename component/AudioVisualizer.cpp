//
// Created by tmarc on 14/07/2021.
//

#include <iostream>
#include <thread>
#include <LitBoardDriver.h>
#include "AudioVisualizer.h"


lbd::comp::AudioVisualizer::AudioVisualizer()
        : Component([this] { asyncTaskCycle(); }, false) { }

lbd::comp::ComponentId lbd::comp::AudioVisualizer::getComponentId() const {
    return ComponentId::AudioVisualizer;
}

void lbd::comp::AudioVisualizer::onMessageReceived(uint8_t *data, size_t length) {
    if (length != 1) {
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
    startAsyncCyclicTask();
}

void lbd::comp::AudioVisualizer::stop() {
    stopAsyncCyclicTask();
}

void lbd::comp::AudioVisualizer::asyncTaskCycle() {
    // TODO: send audio data

    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 20 fps
}



/*



//-----------------------------------------------------------
// Record an audio stream from the default audio capture
// device. The RecordAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data from the
// capture device. The main loop runs every 1/2 second.
//-----------------------------------------------------------


#include <windows.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <thread>
#include <chrono>
#include <iostream>
#include "bass.h"


class MyAudioSink {
public:
    HRESULT SetFormat(WAVEFORMATEX*);
    HRESULT CopyData(BYTE*, UINT32, BOOL*);
};

HRESULT MyAudioSink::CopyData(BYTE * buffer, UINT32 length, BOOL* b) {
    auto* data = (float*) buffer;

    if (buffer) {
        std::cout << length << std::endl;

        for (int i = 0; i < length; i++) {
            std::cout << data[i] << ' ';
        }

        std::cout << std::endl;


        //fftw_complex *in, *out;
        //in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * length2);
        //out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * length2);
        //fftw_plan p = fftw_plan_dft_1d(length2, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
        //
        //for (int i = 0; i < length2; i++) {
        //    in[i][0] = (double)data2[i];
        //    in[i][1] = 0;
        //}
        //
        //fftw_execute(p);
        //fftw_destroy_plan(p);
        //fftw_free(in); fftw_free(out);
        //
}
else {
std::cout << "-1\n";
}

return 0;
}

HRESULT MyAudioSink::SetFormat(WAVEFORMATEX* format) {
    //if (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
    //    auto waveformatextensible = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(format);
    //
    //    waveformatextensible
    //}
    return 0;
}




// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  20
#define REFTIMES_PER_MILLISEC  (REFTIMES_PER_SEC/1000)

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

HRESULT RecordAudioStream(MyAudioSink *pMySink)
{
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioCaptureClient *pCaptureClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 packetLength = 0;
    BOOL bDone = FALSE;
    BYTE *pData = NULL;
    DWORD flags;

    CoInitialize(NULL);

    hr = CoCreateInstance(
            CLSID_MMDeviceEnumerator, NULL,
            CLSCTX_ALL, IID_IMMDeviceEnumerator,
            (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->GetDefaultAudioEndpoint(
            eRender, eConsole, &pDevice);
    EXIT_ON_ERROR(hr)

    hr = pDevice->Activate(
            IID_IAudioClient, CLSCTX_ALL,
            NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Initialize(
            AUDCLNT_SHAREMODE_SHARED,
            AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
            hnsRequestedDuration,
            0,
            pwfx,
            NULL);
    EXIT_ON_ERROR(hr)

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService(
            IID_IAudioCaptureClient,
            (void**)&pCaptureClient);
    EXIT_ON_ERROR(hr)

    // Notify the audio sink which format to use.
    hr = pMySink->SetFormat(pwfx);
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                        bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr)


    // Each loop fills about half of the shared buffer.
    while (bDone == FALSE) {
        // Sleep for half the buffer duration.
        // Sleep(hnsActualDuration/REFTIMES_PER_SEC*1000/2);
        Sleep(500);

        hr = pCaptureClient->GetNextPacketSize(&packetLength);
        EXIT_ON_ERROR(hr)

        numFramesAvailable = 0;

        while (packetLength != 0) {
            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer(
                    &pData,
                    &numFramesAvailable,
                    &flags, NULL, NULL);
            EXIT_ON_ERROR(hr)

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
                pData = NULL;  // Tell CopyData to write silence.
            }

            // Copy the available capture data to the audio sink.
            hr = pMySink->CopyData(
                    pData, numFramesAvailable, &bDone);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            EXIT_ON_ERROR(hr)
        }

        std::cout << "end of loop\n";
        std::flush(std::cout);
    }

    hr = pAudioClient->Stop();  // Stop recording.
    EXIT_ON_ERROR(hr)

    Exit:
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pCaptureClient)

    return hr;
}



#include "LitBoardDriver.h"

int main(int argc, char* argv[])
{
    MyAudioSink asd;
    RecordAudioStream(&asd);

    auto& driver = lbd::LitBoardDriver::getInstance();
    // driver.run();
    return 0;
}

*/
