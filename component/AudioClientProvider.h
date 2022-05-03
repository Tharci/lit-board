//
// Created by tmarc on 22/02/2022.
//

#ifndef LITBOARD_AUDIOCLIENTPROVIDER_H
#define LITBOARD_AUDIOCLIENTPROVIDER_H


#include <windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <mutex>
#include <functional>
#include <utility>


#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define SAFE_RELEASE(punk)  \
                  if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }

#define RELEASE_ON_ERROR(hres)  \
                  if (FAILED(hres)) { releaseResources(); return; }


namespace lbd::comp {
    class AudioClientProvider : public IMMNotificationClient {
    public:
        explicit AudioClientProvider(std::function<void(IAudioClient*, IAudioCaptureClient*, WAVEFORMATEX*, UINT32)> onConnectedCallback)
            : onConnectedCallback(std::move(onConnectedCallback)) { }

        void keepConnection() {
            std::lock_guard lockGuard(reconnectMutex);
            if (shouldReconnect) {
                connect();
                shouldReconnect = false;
            }
        }

        HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) override {
            // Who cares.
            return 0;
        }

        HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId) override {
            // Who cares.
            return 0;
        }

        HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId) override {
            // Who cares.
            return 0;
        }

        HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(
                EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId) override {
            reconnect();
            return 0;
        }

        HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(
                LPCWSTR pwstrDeviceId, const PROPERTYKEY key) override {
            // Who cares.
            return 0;
        }

        HRESULT STDMETHODCALLTYPE QueryInterface (
                REFIID riid, void __RPC_FAR* __RPC_FAR *ppvObject) override {
            // Who cares.
            return 0;
        }

        ULONG STDMETHODCALLTYPE AddRef() override {
            // Who cares.
            return 0;
        }

        ULONG STDMETHODCALLTYPE Release() override {
            // Who cares.
            return 0;
        }

        void reconnect() {
            std::lock_guard lockGuard(reconnectMutex);
            shouldReconnect = true;
        }

    private:
        void releaseResources() {
            CoTaskMemFree(pwfx);
            SAFE_RELEASE(pEnumerator)
            SAFE_RELEASE(pDevice)
            SAFE_RELEASE(pAudioClient)
            SAFE_RELEASE(pCaptureClient)

            pCaptureClient = nullptr;
            pCaptureClient = nullptr;
            pDevice = nullptr;
            pAudioClient = nullptr;
        }

        void connect() {
            releaseResources();

            HRESULT hr;

            hr = CoCreateInstance(
                    CLSID_MMDeviceEnumerator, NULL,
                    CLSCTX_ALL, IID_IMMDeviceEnumerator,
                    (void **) &pEnumerator);
            RELEASE_ON_ERROR(hr);

            hr = pEnumerator->GetDefaultAudioEndpoint(
                    eRender, eConsole, &pDevice);
            RELEASE_ON_ERROR(hr);

            pEnumerator->RegisterEndpointNotificationCallback(this);

            hr = pDevice->Activate(
                    IID_IAudioClient, CLSCTX_ALL,
                    NULL, (void **) &pAudioClient);
            RELEASE_ON_ERROR(hr);

            hr = pAudioClient->GetMixFormat(&pwfx);
            RELEASE_ON_ERROR(hr);

            hr = pAudioClient->Initialize(
                    AUDCLNT_SHAREMODE_SHARED,
                    AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
                    hnsRequestedDuration,
                    0,
                    pwfx,
                    NULL);
            RELEASE_ON_ERROR(hr);

            // Get the size of the allocated buffer.
            hr = pAudioClient->GetBufferSize(&bufferFrameCount);
            RELEASE_ON_ERROR(hr);

            hr = pAudioClient->GetService(
                    IID_IAudioCaptureClient,
                    (void **) &pCaptureClient);
            RELEASE_ON_ERROR(hr);

            hr = pAudioClient->Start();  // Start recording.
            RELEASE_ON_ERROR(hr);

            onConnectedCallback(pAudioClient, pCaptureClient, pwfx, bufferFrameCount);
        }

        std::function<void(IAudioClient*, IAudioCaptureClient*, WAVEFORMATEX*,  UINT32)> onConnectedCallback;
        std::mutex reconnectMutex;
        bool shouldReconnect = true;

        REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
        UINT32 bufferFrameCount {};
        IMMDeviceEnumerator* pEnumerator = nullptr;
        IMMDevice* pDevice = nullptr;
        IAudioClient* pAudioClient = nullptr;
        IAudioCaptureClient* pCaptureClient = nullptr;
        WAVEFORMATEX* pwfx = nullptr;
    };
}


#endif //LITBOARD_AUDIOCLIENTPROVIDER_H
