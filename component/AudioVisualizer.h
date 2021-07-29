//
// Created by tmarc on 14/07/2021.
//

#ifndef LITBOARD_AUDIOVISUALIZER_H
#define LITBOARD_AUDIOVISUALIZER_H

#include "Component.h"

#include <atomic>


namespace lbd::comp {
    class AudioVisualizer : public Component {
    public:
        [[nodiscard]] ComponentId getComponentId() const override;
        void onMessageReceived(uint8_t *data, size_t length) override;

    private:
        void start();
        void stop();

        std::atomic<bool> running = false;
    };
}



#endif //LITBOARD_AUDIOVISUALIZER_H
