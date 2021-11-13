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
        AudioVisualizer();
        [[nodiscard]] ComponentId getComponentId() const override;

    protected:
        void onMessageReceived(uint8_t *data, size_t length) override;

    private:
        enum class Commands {
            STOP  = 0,
            START = 1
        };

        void start();
        void stop();
        void asyncTaskCycle();
    };
}



#endif //LITBOARD_AUDIOVISUALIZER_H
